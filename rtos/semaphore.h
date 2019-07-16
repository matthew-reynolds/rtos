/**
 * Semaphores
 * @author Matt Reynolds
 * @author Dawson Hemphill
 */
#ifndef __RTOS_SEMAPHORE_H
#define __RTOS_SEMAPHORE_H

#include <stdint.h>

#include "status.h"
#include "task.h"

/// Semaphore attributes
typedef struct {
  const char* name;
  uint32_t    attr_bits;  // Unused
} rtosSemaphoreAttr_t;

/// Semaphore
typedef struct semaphore {
<<<<<<< HEAD
  const char*      name;
  uint32_t         count;
  uint32_t         max;
  rtosTaskHandle_t blocked;
=======
  uint32_t max;
  uint32_t count;
	rtosSemaphoreAttr_t attr;
>>>>>>> Beginnings of Mutex
} rtosSemaphore_t;

typedef rtosSemaphore_t* rtosSemaphoreHandle_t;

rtosStatus_t rtosSemaphoreNew(uint32_t                   max,
                              uint32_t                   init,
                              const rtosSemaphoreAttr_t* attrs,
                              rtosSemaphoreHandle_t      semaphore);
rtosStatus_t rtosSemaphoreDelete(rtosSemaphoreHandle_t semaphore);
rtosStatus_t rtosSemaphoreAcquire(rtosSemaphoreHandle_t semaphore, uint32_t timeout);
rtosStatus_t rtosSemaphoreRelease(rtosSemaphoreHandle_t semaphore);

#endif  // __RTOS_SEMAPHORE_H
