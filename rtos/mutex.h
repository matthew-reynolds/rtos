/**
 * Mutexes
 * @author Matt Reynolds
 * @author Dawson Hemphill
 */
#ifndef __RTOS_MUTEX_H
#define __RTOS_MUTEX_H

#include <stdint.h>

#include "status.h"

// Define mutex attribute options
#define osMutexRecursive 0x00000001U
#define osMutexPrioInherit 0x00000002U
#define osMutexRobust 0x00000008U

/// Mutex attributes
typedef struct {
  const char* name;
  uint32_t    attr_bits;
} rtosMutexAttr_t;

/// Mutex
typedef struct {

} rtosMutex_t;

typedef rtosMutex_t* rtosMutexHandle_t;

rtosStatus_t rtosMutexNew(const rtosMutexAttr_t* attrs, rtosMutexHandle_t* mutex);
rtosStatus_t rtosMutexDelete(const rtosMutexHandle_t mutex);
rtosStatus_t rtosMutexAcquire(const rtosMutexHandle_t mutex, uint32_t timeout);
rtosStatus_t rtosMutexRelease(const rtosMutexHandle_t mutex);

#endif  // __RTOS_MUTEX_H
