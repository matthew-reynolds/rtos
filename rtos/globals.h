/**
 * Global data
 * @author Matt Reynolds
 * @author Dawson Hemphill
 */
#ifndef __RTOS_GLOBALS_H
#define __RTOS_GLOBALS_H

#include <stdint.h>

#include "mutex.h"      // For rtosMutex_t, MAX_MUTEXES
#include "scheduler.h"  // For RTOS_PRIORITY_COUNT
#include "task.h"       // For rtosTaskControlBlock_t, rtosTaskHandle_t, MAX_TASKS

extern uint32_t               rtos_ticks;                             // Defined in rtos.c
extern rtosMutex_t            rtos_mutexes[MAX_MUTEXES];              // Defined in mutex.c
extern rtosTaskControlBlock_t rtos_tasks[MAX_TASKS];                  // Defined in task.c
extern rtosTaskHandle_t       rtos_inactive_tasks;                    // Defined in scheduler.c
extern rtosTaskHandle_t       rtos_ready_tasks[RTOS_PRIORITY_COUNT];  // Defined in scheduler.c
extern rtosTaskHandle_t       rtos_running_task;                      // Defined in scheduler.c
extern rtosTaskHandle_t       rtos_delayed_tasks;                     // Defined in scheduler.c
extern rtosTaskHandle_t       rtos_blocked_tasks;                     // Defined in scheduler.c
extern rtosTaskHandle_t       rtos_terminated_tasks;                  // Defined in scheduler.c

#endif  // __RTOS_GLOBALS_H
