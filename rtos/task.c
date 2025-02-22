/**
 * Task implementation
 * @author Matt Reynolds
 * @author Dawson Hemphill
 */

#include <stdlib.h>

#include "globals.h"
#include "task.h"

rtosTaskControlBlock_t rtos_tasks[MAX_TASKS];

/**
 * Initialize all task control blocks
 */
void rtosTaskInitAll(void) {
  for (uint32_t task_id = 0; task_id < MAX_TASKS; task_id++) {
    rtosTaskInit(task_id);
  }
}

/**
 * Initialize the task control block
 *
 * Sets the task ID and adds the task to the inactive list.
 */
void rtosTaskInit(uint32_t task_id) {
  rtosTaskHandle_t tcb_ref = &rtos_tasks[task_id];

  // Initialize the TCB
  tcb_ref->id              = task_id;
  tcb_ref->next            = NULL;
  tcb_ref->priority        = RTOS_PRIORITY_NONE;
  tcb_ref->state           = RTOS_TASK_INACTIVE;
  tcb_ref->stack_pointer   = BASE_STACK_PTR - MAIN_STACK_SIZE - TASK_STACK_SIZE * tcb_ref->id;
  tcb_ref->wake_time_ticks = 0;

  // Add the task to the inactive list
  rtosInsertTaskListHead(&rtos_inactive_tasks, tcb_ref);
}

/**
 * Terminate the current task
 *
 * Terminates execution of the task and adds the task's control block back to the pool of available tasks.
 * Note that terminated tasks and inactive tasks are treated the same way in this RTOS.
 *
 * Analagous to a return statement at the end of the task function body.
 */
void rtosTaskExit(void) {
  rtos_running_task->state = RTOS_TASK_TERMINATED;
  rtosInsertTaskListTail(&rtos_inactive_tasks, rtos_running_task);
  rtosInvokeScheduler();
}

/**
 * Create a new task given the specified function and priority
 *
 * @param func      The function that the task executes
 * @param arg       The argument to pass to the function
 * @param priority  The priority of the task. If NULL, default priority = RTOS_PRIORITY_NORMAL
 * @param task      A handle to the created task
 *
 * @return  - RTOS_OK on success
 *          - RTOS_ERROR_RESOURCE if no more tasks can be created
 *          - RTOS_ERROR_PARAMETER if the function pointer is NULL
 */
rtosStatus_t rtosTaskNew(rtosTaskFunc_t func, void* arg, rtosPriority_t priority, rtosTaskHandle_t* task) {

  if (rtos_inactive_tasks == NULL) {
    *task = NULL;
    return RTOS_ERROR_RESOURCE;
  }

  if (func == NULL) {
    *task = NULL;
    return RTOS_ERROR_PARAMETER;
  }

  if (priority == RTOS_PRIORITY_NONE) {
    priority = RTOS_PRIORITY_NORMAL;
  }

  rtosTaskHandle_t tcb_ref = rtosPopTaskListHead(&rtos_inactive_tasks);

  // Setup the tcb and add the task to the ready queue
  tcb_ref->next          = NULL;
  tcb_ref->priority      = priority;
  tcb_ref->state         = RTOS_TASK_READY;
  tcb_ref->stack_pointer = BASE_STACK_PTR - MAIN_STACK_SIZE - TASK_STACK_SIZE * tcb_ref->id;
  rtosInsertTaskListHead(rtosGetReadyTaskQueue(priority), tcb_ref);

  // Initialize stack. Set all unspecified registers to 0. (Note: This is unnecessary)
  *(uint32_t*) (tcb_ref->stack_pointer - 0x40) = 0x00000000;       // R4
  *(uint32_t*) (tcb_ref->stack_pointer - 0x3C) = 0x00000000;       // R5
  *(uint32_t*) (tcb_ref->stack_pointer - 0x38) = 0x00000000;       // R6
  *(uint32_t*) (tcb_ref->stack_pointer - 0x34) = 0x00000000;       // R7
  *(uint32_t*) (tcb_ref->stack_pointer - 0x30) = 0x00000000;       // R8
  *(uint32_t*) (tcb_ref->stack_pointer - 0x2C) = 0x00000000;       // R9
  *(uint32_t*) (tcb_ref->stack_pointer - 0x28) = 0x00000000;       // R10
  *(uint32_t*) (tcb_ref->stack_pointer - 0x24) = 0x00000000;       // R11
  *(uint32_t*) (tcb_ref->stack_pointer - 0x20) = (uint32_t) arg;   // R0
  *(uint32_t*) (tcb_ref->stack_pointer - 0x1C) = 0x00000000;       // R1
  *(uint32_t*) (tcb_ref->stack_pointer - 0x18) = 0x00000000;       // R2
  *(uint32_t*) (tcb_ref->stack_pointer - 0x14) = 0x00000000;       // R3
  *(uint32_t*) (tcb_ref->stack_pointer - 0x10) = 0x00000000;       // R12
  *(uint32_t*) (tcb_ref->stack_pointer - 0x0C) = 0x00000000;       // R14 = LR
  *(uint32_t*) (tcb_ref->stack_pointer - 0x08) = (uint32_t) func;  // R15 = PC
  *(uint32_t*) (tcb_ref->stack_pointer - 0x04) = 0x01000000;       // PSR
  tcb_ref->stack_pointer -= 0x40;

  if (task != NULL) {
    *task = tcb_ref;
  }
  return RTOS_OK;
}

/**
 * Remove and return the head of the specified singly-linked list
 */
rtosTaskHandle_t rtosPopTaskListHead(rtosTaskHandle_t* list) {
  rtosTaskHandle_t task = *list;
  *list                 = (*list)->next;
  task->next            = NULL;
  return task;
}

/**
 * Insert the specified task to the head of the specified singly-linked list
 */
void rtosInsertTaskListHead(rtosTaskHandle_t* list, rtosTaskHandle_t task) {
  task->next = *list;
  *list      = task;
}

/**
 * Insert the specified task to the tail of the specified singly-linked list
 */
void rtosInsertTaskListTail(rtosTaskHandle_t* list, rtosTaskHandle_t task) {
  if (*list == NULL) {
    *list = task;
  } else {

    rtosTaskHandle_t cur = *list;
    while (cur->next != NULL) {
      cur = cur->next;
    }
    cur->next = task;
  }
}
