/**
 * Fixed-Priority Preemptive Scheduler
 * @author Matt Reynolds
 * @author Dawson Hemphill
 */
#ifndef __RTOS_SCHEDULER_H
#define __RTOS_SCHEDULER_H

#include "task.h"


extern rtosTaskControlBlock_t* rtos_ready_tasks[RTOS_PRIORITY_COUNT];
extern rtosTaskControlBlock_t* rtos_running_task;

/**
 * Get the priority of the highest-priority non-empty queue of ready tasks.
 *
 * @returns The priority of the queue, or RTOS_PRIORITY_NONE of no ready tasks found.
 */
rtosPriority_t rtosGetReadyTask(void);

void rtosInvokeScheduler(void);

#endif  // __RTOS_SCHEDULER_H
