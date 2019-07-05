#include <stdlib.h>

#include "task.h"


uint32_t rtosTaskNew(rtosTaskFunc_t func, rtosPriority_t priority) {
  if (func == NULL) {
    return NULL;
  }

  if (priority == RTOS_PRIORITY_NONE) {
    priority = RTOS_PRIORITY_NORMAL;
  }

  rtosTaskControlBlock_t* tcb = malloc(sizeof(rtosTaskControlBlock_t));
  tcb->id                     = 0;  // TODO: Increment id, check that id does not exceed MAX_TASKS
  tcb->priority               = priority;
  tcb->state                  = RTOS_TASK_INACTIVE;
  tcb->stack_pointer          = TOTAL_STACK_SIZE - MAIN_STACK_SIZE - TASK_STACK_SIZE * tcb->id;
  tcb->next                   = NULL;

  return tcb->id;
}
