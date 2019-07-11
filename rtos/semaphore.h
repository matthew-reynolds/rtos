/**
 * Semaphores
 * @author Matt Reynolds
 * @author Dawson Hemphill
 */
#ifndef __RTOS_SEMAPHORE_H
#define __RTOS_SEMAPHORE_H

#include <stdint.h>

#include "status.h"

#define MAX_SEMAPHORES 10

// Define semaphore attribute options
#define rtosSemaphoreSomething 0x00000000U

/// Semaphore attributes
typedef struct {
  const char* name;
  uint32_t    attr_bits;
} rtosSemaphoreAttr_t;

/// Semaphore
typedef struct {

} rtosSemaphore_t;

typedef rtosSemaphore_t* rtosSemaphoreHandle_t;

rtosStatus_t osSemaphoreNew(uint32_t max,
                            uint32_t init,
                            const rtosSemaphoreAttr_t* attrs,
                            rtosSemaphoreHandle_t* semaphore);
rtosStatus_t osSemaphoreDelete(const rtosSemaphoreHandle_t semaphore);
rtosStatus_t osSemaphoreAcquire(const rtosSemaphoreHandle_t semaphore, uint32_t timeout);
rtosStatus_t osSemaphoreRelease(const rtosSemaphoreHandle_t semaphore);

#endif  // __RTOS_SEMAPHORE_H
