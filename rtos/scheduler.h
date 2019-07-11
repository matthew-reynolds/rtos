/**
 * Fixed-priority preemptive scheduler
 * @author Matt Reynolds
 * @author Dawson Hemphill
 */
#ifndef __RTOS_SCHEDULER_H
#define __RTOS_SCHEDULER_H

#include "globals.h"
#include "task.h"

#define SCHEDULER_TIMESLICE 5

rtosPriority_t          rtosGetHighestReadyPriority(void);
rtosTaskControlBlock_t* rtosGetReadyTask(rtosPriority_t priority);

void rtosInvokeScheduler(void);
void rtosPerformContextSwitch(void);

#endif  // __RTOS_SCHEDULER_H
