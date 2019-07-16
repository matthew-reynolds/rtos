/**
 * Mutexes
 * @author Matt Reynolds
 * @author Dawson Hemphill
 */
#ifndef __RTOS_MUTEX_H
#define __RTOS_MUTEX_H

#include <stdint.h>
#include "task.h"

#include "status.h"

// Define mutex attribute options
#define rtosMutexPrioInherit 0x00000002U
#define rtosMutexRobust 0x00000008U

/// Mutex attributes
typedef struct {
  const char* name;
  uint32_t    attr_bits;
} rtosMutexAttr_t;

/// Mutex
typedef struct {
  uint32_t         count;
  rtosTaskHandle_t blocked;
  rtosTaskHandle_t acquired;
  rtosPriority_t initPriority;
  rtosMutexAttr_t attrs;
} rtosMutex_t;

typedef rtosMutex_t* rtosMutexHandle_t;

rtosStatus_t rtosMutexNew(const rtosMutexAttr_t* attrs, rtosMutexHandle_t* mutex);
rtosStatus_t rtosMutexDelete(rtosMutexHandle_t mutex);
rtosStatus_t rtosMutexAcquire(rtosMutexHandle_t mutex, uint32_t timeout);
rtosStatus_t rtosMutexRelease(rtosMutexHandle_t mutex);

#endif  // __RTOS_MUTEX_H
