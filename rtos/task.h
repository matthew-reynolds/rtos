/**
 * Tasks
 * @author Matt Reynolds
 * @author Dawson Hemphill
 */
#ifndef __RTOS_TASK_H
#define __RTOS_TASK_H

#include <stdint.h>

#include <LPC17xx.h>

#include "status.h"

#define BASE_STACK_PTR *(uint32_t*) (0x00 + SCB->VTOR)
#define TOTAL_STACK_SIZE 0x2000
#define MAIN_STACK_SIZE 0x800
#define TASK_STACK_SIZE 0x400
#define MAX_TASKS ((TOTAL_STACK_SIZE - MAIN_STACK_SIZE) / TASK_STACK_SIZE)

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

typedef rtosTaskControlBlock_t* rtosTaskHandle_t;

// Function pointer
typedef void (*rtosTaskFunc_t)(void* args);

rtosStatus_t rtosTaskNew(rtosTaskFunc_t func, void* arg, rtosPriority_t priority, rtosTaskHandle_t* task);
rtosStatus_t rtosTaskDelete(const rtosTaskHandle_t task);

#endif  // __RTOS_TASK_H
