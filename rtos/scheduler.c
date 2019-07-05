#include <stdlib.h>

#include "scheduler.h"


rtosTaskControlBlock_t* rtos_ready_tasks[RTOS_PRIORITY_COUNT] = {NULL};
rtosTaskControlBlock_t* rtos_running_task                     = NULL;

rtosPriority_t rtosGetReadyTask(void) {
  uint32_t queue_vec = 0;
  for (unsigned prio = RTOS_PRIORITY_IDLE; prio < RTOS_PRIORITY_COUNT; prio++) {
    queue_vec |= !!rtos_ready_tasks[prio] << (prio - RTOS_PRIORITY_IDLE);
  }

  uint32_t leading_zeros;
  asm("CLZ leading_zeros, queue_vec");

  // If there are no ready tasks, return RTOS_PRIORITY_NONE. Otherwise, return the highest non-empty priority
  return (rtosPriority_t)(32 - leading_zeros);
}


void rtosInvokeScheduler(void) {

  // TODO: Calculate timeslice

  rtosPriority_t highest_ready_priority = rtosGetReadyTask();

  // A higher-priority task is ready, OR
  // An equal-priority task is ready and the timeslice has expired
  if (highest_ready_priority > rtos_running_task->priority
      || (highest_ready_priority == rtos_running_task && 1 /* Timeslice expired */)) {
    // Perform context switch to next task
  }
}
