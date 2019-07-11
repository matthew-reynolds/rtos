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

rtosStatus_t rtosMutexNew(rtosMutex_t** mutex, const rtosMutexAttr_t* attrs);
rtosStatus_t rtosMutexDelete(const rtosMutex_t* mutex);
rtosStatus_t rtosMutexAcquire(const rtosMutex_t* mutex, uint32_t timeout);
rtosStatus_t rtosMutexRelease(const rtosMutex_t* mutex);

#endif  // __RTOS_MUTEX_H
