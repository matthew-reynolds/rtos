/**
 * Tasks
 * @author Matt Reynolds
 * @author Dawson Hemphill
 */
#ifndef __RTOS_TASK_H
#define __RTOS_TASK_H

#include <stdint.h>

#include "status.h"

#define TOTAL_STACK_SIZE 0x8000  // TODO: Check dynamically? Can we pull from startup_LPC17xx.s?
#define MAIN_STACK_SIZE 0x2000
#define TASK_STACK_SIZE 0x1000
#define MAX_TASKS 6

/// Task priorities
typedef enum {
  RTOS_PRIORITY_NONE,  // No priority
  RTOS_PRIORITY_IDLE,
  RTOS_PRIORITY_LOW,
  RTOS_PRIORITY_BELOW_NORMAL,
  RTOS_PRIORITY_NORMAL,
  RTOS_PRIORITY_ABOVE_NORMAL,
  RTOS_PRIORITY_HIGH,
  RTOS_PRIORITY_REALTIME,
  RTOS_PRIORITY_COUNT = RTOS_PRIORITY_REALTIME,  // The total number of priorities
} rtosPriority_t;

/// Task execution states
typedef enum {
  RTOS_TASK_INACTIVE,
  RTOS_TASK_READY,
  RTOS_TASK_RUNNING,
  RTOS_TASK_BLOCKED,
  RTOS_TASK_TERMINATED,
} rtosTaskState_t;

/// Task control block
typedef struct rtosTaskControlBlock_tag {
  uint32_t                         id;
  rtosPriority_t                   priority;
  rtosTaskState_t                  state;
  uint32_t                         stack_pointer;
  struct rtosTaskControlBlock_tag* next;
} rtosTaskControlBlock_t;

// Function pointer
typedef void (*rtosTaskFunc_t)(void* args);

/**
 * Create a new task given the specified function and priority
 */
uint32_t rtosTaskNew(rtosTaskFunc_t func, rtosPriority_t priority);

#endif  // __RTOS_TASK_H
