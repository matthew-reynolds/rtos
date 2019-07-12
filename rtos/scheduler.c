/**
 * Fixed-priority preemtive scheduler implementation
 * @author Matt Reynolds
 * @author Dawson Hemphill
 */

#include <stdlib.h>

#include "context.h"
#include "globals.h"
#include "scheduler.h"

rtosTaskHandle_t rtos_inactive_tasks                   = NULL;
rtosTaskHandle_t rtos_ready_tasks[RTOS_PRIORITY_COUNT] = {NULL};
rtosTaskHandle_t rtos_running_task                     = NULL;
rtosTaskHandle_t rtos_delayed_tasks                    = NULL;  // Stored in order of wake time
rtosTaskHandle_t rtos_blocked_tasks                    = NULL;
rtosTaskHandle_t rtos_terminated_tasks                 = NULL;

rtosTaskHandle_t _next_task = NULL;

/**
 * Get the priority of the highest-priority non-empty queue of ready tasks.
 *
 * @returns The priority of the queue, or RTOS_PRIORITY_NONE of no ready tasks found.
 */
rtosPriority_t rtosGetHighestReadyPriority(void) {

  // Create a bit vector representing whether each queue is nonempty
  uint32_t queue_vec = 0;
  for (unsigned prio = RTOS_PRIORITY_IDLE; prio < RTOS_PRIORITY_COUNT; prio++) {
    queue_vec |= !!rtos_ready_tasks[prio - RTOS_PRIORITY_IDLE] << (prio - RTOS_PRIORITY_IDLE);
  }

  // Determine the number of leading zeros in the bit vector
  uint32_t leading_zeros;
  asm("CLZ leading_zeros, queue_vec");

  // If there are no ready tasks, return RTOS_PRIORITY_NONE. Otherwise, return the highest non-empty priority
  return (rtosPriority_t)(32 - leading_zeros);
}

/**
 * Get the ready task queue with the specified priority
 */
rtosTaskHandle_t* rtosGetReadyTaskQueue(rtosPriority_t priority) {
  if (priority == RTOS_PRIORITY_NONE) {
    return NULL;
  }

  return &rtos_ready_tasks[priority - RTOS_PRIORITY_IDLE];
}

/**
 * Get the ready task with the specified priority
 */
rtosTaskHandle_t rtosGetReadyTask(rtosPriority_t priority) {
  rtosTaskHandle_t* queue = rtosGetReadyTaskQueue(priority);
  return (queue == NULL) ? NULL : *queue;
};

/**
 * Invoke the scheduler
 *
 * Perform a context switch to the next ready highest priority task by triggering the PendSV exception if:
 *  - The running task has been blocked/terminated, OR
 *  - A higher-priority task is ready, OR
 *  - An equal-priority task is ready and the timeslice has expired
 */
void rtosInvokeScheduler(void) {
  static uint32_t last_switch_ticks = 0;

  // Unblock any delayed tasks whose delay has expired
  while (rtos_delayed_tasks != NULL && rtos_delayed_tasks->wake_time_ticks == rtos_ticks) {
    rtosTaskHandle_t unblocked_task = rtosPopTaskListHead(&rtos_delayed_tasks);
    rtosInsertTaskListHead(rtosGetReadyTaskQueue(unblocked_task->priority), unblocked_task);
  }

  rtosPriority_t highest_ready_priority = rtosGetHighestReadyPriority();

  // Check if a context switch is required
  if (rtos_running_task->state != RTOS_TASK_RUNNING || highest_ready_priority > rtos_running_task->priority
      || (highest_ready_priority == rtos_running_task->priority
          && (rtos_ticks - last_switch_ticks) > SCHEDULER_TIMESLICE)) {
    last_switch_ticks = rtos_ticks;

    // If the current task is being preempted, append the current task to the end of the appropriate ready queue
    // TODO: Should this block be in rtosInvokeScheduler or in rtosPerformContextSwitch?
    if (rtos_running_task->state == RTOS_TASK_RUNNING) {
      rtos_running_task->state = RTOS_TASK_READY;
      rtosInsertTaskListTail(rtosGetReadyTaskQueue(rtos_running_task->priority), rtos_running_task);
    }

    // Determine the next task to run, and invoke the PendSV exception to perform the context switch
    _next_task = rtosGetReadyTask(highest_ready_priority);
    SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
  }
}

/**
 * Perform a context switch to the task stored in next_task
 */
void rtosPerformContextSwitch(void) {
  rtos_running_task->stack_pointer = rtosStoreContext();

  // Set the running task to the new task
  rtosPopTaskListHead(rtosGetReadyTaskQueue(_next_task->priority));
  _next_task->next  = NULL;
  _next_task->state = RTOS_TASK_RUNNING;
  rtos_running_task = _next_task;
  _next_task        = NULL;

  rtosRestoreContext(rtos_running_task->stack_pointer);
}

/**
 * Block the current task for the specified number of ticks
 *
 * @param ticks the number of systicks to delay
 */
rtosStatus_t rtosDelay(uint32_t ticks) {
  return rtosDelayUntil(rtos_ticks + ticks);
}

/**
 * Block the current task until the systick count reaches the specified value
 *
 * @param ticks the wakeup time
 */
rtosStatus_t rtosDelayUntil(uint32_t ticks) {
  const uint32_t const_rtos_ticks = rtos_ticks;

  // TODO: Disable systick interrupt?

  rtos_running_task->wake_time_ticks = ticks;
  rtos_running_task->state           = RTOS_TASK_BLOCKED;

  // Add the current task to the list of delayed tasks, in order of wake time
  if (rtos_delayed_tasks == NULL) {
    rtos_delayed_tasks = rtos_running_task;
  } else {

    // No overflow
    if (ticks > const_rtos_ticks) {

      // Insert at front
      if (rtos_delayed_tasks->wake_time_ticks >= ticks) {
        rtosInsertTaskListHead(&rtos_delayed_tasks, rtos_running_task);
      }

      // Insert elsewhere
      else {

        rtosTaskHandle_t cur_task = rtos_delayed_tasks;
        while (cur_task->next != NULL && cur_task->next->wake_time_ticks < ticks) {
          cur_task = cur_task->next;
        }
        rtosInsertTaskListHead(&cur_task->next, rtos_running_task);
      }
    }
    // Overflow
    else {

      // Insert at front
      if (rtos_delayed_tasks->wake_time_ticks < const_rtos_ticks && rtos_delayed_tasks->wake_time_ticks >= ticks) {
        rtosInsertTaskListHead(&rtos_delayed_tasks, rtos_running_task);
      }

      // Insert elsewhere
      else {

        // Consume all non-overflowed wake times
        rtosTaskHandle_t cur_task = rtos_delayed_tasks;
        if (cur_task->wake_time_ticks > const_rtos_ticks) {
          while (cur_task->next != NULL && cur_task->next->wake_time_ticks < const_rtos_ticks) {
            cur_task = cur_task->next;
          }
        }

        // cur_task currently either points to the first overflowed or the last non-overflowed
        while (cur_task->next != NULL && cur_task->next->wake_time_ticks < ticks) {
          cur_task = cur_task->next;
        }

        rtosInsertTaskListHead(&cur_task->next, rtos_running_task);
      }
    }
  }

  rtosInvokeScheduler();
  return RTOS_OK;
}
