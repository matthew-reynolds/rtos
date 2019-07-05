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

// Mutex attributes
typedef struct {
  const char* name;
  uint32_t    attr_bits;
} rtosMutexAttr_t;

// Mutex ID
typedef struct {

} rtosMutex_t;

rtosMutex_t* rtosMutexNew(const rtosMutexAttr_t* attrs);
rtosStatus_t rtosMutexDelete(rtosMutex_t* id);
rtosStatus_t rtosMutexAcquire(rtosMutex_t* id, uint32_t timeout);
rtosStatus_t rtosMutexRelease(rtosMutex_t* id);

#endif  // __RTOS_MUTEX_H
