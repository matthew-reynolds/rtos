/**
 * Semaphores
 * @author Matt Reynolds
 * @author Dawson Hemphill
 */
#ifndef __RTOS_SEMAPHORE_H
#define __RTOS_SEMAPHORE_H

#include "status.h"
#include <stdint.h>

#define MAX_SEMAPHORES 10

// Define semaphore attribute options
#define rtosSemaphoreSomething 0x00000000U

/// Semaphore attributes
// Gonna keep this in but I put the attributes just in the semaphore struct
typedef struct {
  const char* name;
  uint32_t    attr_bits;
} rtosSemaphoreAttr_t;

/// Semaphore
typedef struct semaphore {
  uint32_t    max;
  uint32_t    count;
  const char* name;
} rtosSemaphore_t;

typedef rtosSemaphore_t* rtosSemaphoreHandle_t;

rtosStatus_t rtosSemaphoreNew(uint32_t                   max,
                              uint32_t                   init,
                              const rtosSemaphoreAttr_t* attrs,
                              rtosSemaphoreHandle_t*     semaphore);
rtosStatus_t rtosSemaphoreDelete(const rtosSemaphoreHandle_t semaphore);
rtosStatus_t rtosSemaphoreAcquire(const rtosSemaphoreHandle_t semaphore, uint32_t timeout);
rtosStatus_t rtosSemaphoreRelease(const rtosSemaphoreHandle_t semaphore);

#endif  // __RTOS_SEMAPHORE_H
