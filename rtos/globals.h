/**
 * Global data
 * @author Matt Reynolds
 * @author Dawson Hemphill
 */
#ifndef __RTOS_GLOBALS_H
#define __RTOS_GLOBALS_H

#include <stdint.h>

#include "scheduler.h"  // For RTOS_PRIORITY_COUNT
#include "semaphore.h"  // For rtosSemaphoreHandle_t
#include "mutex.h"      // For rtosMutexHandle_t
#include "task.h"       // For rtosTaskControlBlock_t, rtosTaskHandle_t, MAX_TASKS

extern uint32_t               rtos_ticks;                             // Defined in rtos.c
extern rtosTaskControlBlock_t rtos_tasks[MAX_TASKS];                  // Defined in task.c
extern rtosSemaphoreHandle_t  rtos_semaphores;                        // Defined in semaphore.c
extern rtosMutexHandle_t      rtos_mutexes;                           // Defined in mutex.c
extern rtosTaskHandle_t       rtos_inactive_tasks;                    // Defined in scheduler.c
extern rtosTaskHandle_t       rtos_ready_tasks[RTOS_PRIORITY_COUNT];  // Defined in scheduler.c
extern rtosTaskHandle_t       rtos_running_task;                      // Defined in scheduler.c
extern rtosTaskHandle_t       rtos_delayed_tasks;                     // Defined in scheduler.c

#endif  // __RTOS_GLOBALS_H
