/**
 * Mutexes
 * @author Matt Reynolds
 * @author Dawson Hemphill
 */
#ifndef __RTOS_MUTEX_H
#define __RTOS_MUTEX_H

#include <stdint.h>

#include "status.h"
#include "task.h"

// Define mutex attribute options
#define RTOS_MUTEX_PRIO_INHERIT 0x00000002U
#define RTOS_MUTEX_ROBUST 0x00000008U

/// Mutex attributes
typedef struct {
  const char* name;
  uint32_t    attr_bits;
} rtosMutexAttr_t;

/// Mutex
typedef struct rtosMutex_tag {
  const char*           name;
  uint32_t              count;
  uint32_t              attr_bits;
  rtosTaskHandle_t      blocked;
  rtosTaskHandle_t      acquired;
  rtosPriority_t        init_priority;
  struct rtosMutex_tag* next;
} rtosMutex_t;

typedef rtosMutex_t* rtosMutexHandle_t;

rtosStatus_t rtosMutexNew(const rtosMutexAttr_t* attrs, rtosMutexHandle_t mutex);
rtosStatus_t rtosMutexDelete(rtosMutexHandle_t mutex);
rtosStatus_t rtosMutexAcquire(rtosMutexHandle_t mutex, uint32_t timeout);
rtosStatus_t rtosMutexRelease(rtosMutexHandle_t mutex);

#endif  // __RTOS_MUTEX_H
