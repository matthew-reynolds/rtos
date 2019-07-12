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

rtosPriority_t    rtosGetHighestReadyPriority(void);
rtosTaskHandle_t* rtosGetReadyTaskQueue(rtosPriority_t priority);
rtosTaskHandle_t  rtosGetReadyTask(rtosPriority_t priority);

void rtosInvokeScheduler(void);
void rtosPerformContextSwitch(void);

rtosStatus_t rtosDelay(uint32_t ticks);
rtosStatus_t rtosDelayUntil(uint32_t ticks);

#endif  // __RTOS_SCHEDULER_H
