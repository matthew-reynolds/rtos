/**
 * Semaphores
 * @author Matt Reynolds
 * @author Dawson Hemphill
 */
#ifndef __RTOS_SEMAPHORE_H
#define __RTOS_SEMAPHORE_H

#include <stdint.h>

#include "status.h"


// Define semaphore attribute options
#define rtosSemaphoreSomething 0x00000000U

/// Semaphore attributes
typedef struct {
  const char* name;
  uint32_t    attr_bits;
} rtosSemaphoreAttr_t;

/// Semaphore
typedef enum {

} rtosSemaphore_t;

rtosSemaphore_t* osSemaphoreNew(uint32_t max, uint32_t init, const rtosSemaphoreAttr_t* attrs);
rtosStatus_t     osSemaphoreDelete(rtosSemaphore_t* id);
rtosStatus_t     osSemaphoreAcquire(rtosSemaphore_t* id, uint32_t timeout);
rtosStatus_t     osSemaphoreRelease(rtosSemaphore_t* id);

#endif  // __RTOS_SEMAPHORE_H
