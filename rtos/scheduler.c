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
 * Get the ready task with the specified priority
 */
rtosTaskHandle_t rtosGetReadyTask(rtosPriority_t priority) {
  if (priority == RTOS_PRIORITY_NONE) {
    return NULL;
  }

  return rtos_ready_tasks[priority - RTOS_PRIORITY_IDLE];
};

/**
 * Invoke the scheduler
 *
 * Perform a context switch to the next ready highest priority task by triggering the PendSV exception if:
 *  - There is no running task, OR
 *  - A higher-priority task is ready, OR
 *  - An equal-priority task is ready and the timeslice has expired
 */
void rtosInvokeScheduler(void) {
  static uint32_t last_switch_ticks = 0;

  rtosPriority_t highest_ready_priority = rtosGetHighestReadyPriority();

  // Check if a context switch is required
  if (rtos_running_task->state != RTOS_TASK_RUNNING || highest_ready_priority > rtos_running_task->priority
      || (highest_ready_priority == rtos_running_task->priority
          && (rtos_ticks - last_switch_ticks) > SCHEDULER_TIMESLICE)) {

    // Determine the next task to run, and invoke the PendSV exception to perform the context switch
    last_switch_ticks += SCHEDULER_TIMESLICE;
    _next_task = rtosGetReadyTask(highest_ready_priority);
    SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
  }
}

/**
 * Perform a context switch to the task stored in next_task
 */
void rtosPerformContextSwitch(void) {
  rtos_running_task->stack_pointer = rtosStoreContext();

  // Remove the current task
  // TODO: Only do this when the current task is being preempted
  rtos_running_task->state   = RTOS_TASK_READY;
  rtosTaskHandle_t last_task = rtosGetReadyTask(rtos_running_task->priority);
  if (last_task == NULL) {
    rtos_ready_tasks[rtos_running_task->priority - RTOS_PRIORITY_IDLE] = rtos_running_task;
  } else {
    while (last_task->next != NULL) {
      last_task = last_task->next;
    }
    last_task->next = rtos_running_task;
  }

  // Set the running task to the new task
  rtos_ready_tasks[_next_task->priority - RTOS_PRIORITY_IDLE] = _next_task->next;
  _next_task->next                                            = NULL;
  _next_task->state                                           = RTOS_TASK_RUNNING;
  rtos_running_task                                           = _next_task;
  _next_task                                                  = NULL;

  rtosRestoreContext(rtos_running_task->stack_pointer);
}
