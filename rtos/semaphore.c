/**
 * Semaphore implementation
 * @author Matt Reynolds
 * @author Dawson Hemphill
 */

#include <stdint.h>
#include <stdlib.h>

#include "rtos.h"
#include "semaphore.h"

uint32_t sem_count = 0;

rtosStatus_t rtosSemaphoreNew(uint32_t                   max,
                              uint32_t                   init,
                              const rtosSemaphoreAttr_t* attrs,
                              rtosSemaphoreHandle_t      semaphore) {

  if (semaphore == NULL) {
    return RTOS_ERROR_PARAMETER;
  }

  // Initialize the semaphore struct fields
  semaphore->name  = attrs->name;
  semaphore->count = init;
  semaphore->max   = max;

  return RTOS_OK;
}

rtosStatus_t rtosSemaphoreDelete(const rtosSemaphoreHandle_t semaphore) {
  return RTOS_OK;
}

rtosStatus_t rtosSemaphoreAcquire(const rtosSemaphoreHandle_t semaphore, uint32_t timeout) {
  uint32_t startTicks = rtosGetSysTickCount();
  __disable_irq();

  // Timeout value is set to zero so try once and then exit
  if (timeout == 0) {
    if (semaphore->count <= 0) {
      return RTOS_ERROR_RESOURCE;
    } else {

      semaphore->count--;
      __enable_irq();
      return RTOS_OK;
    }
    // Timeout value is set to wat forever so loop until it is ready
  } else if (timeout == rtosWaitForever) {
    while (semaphore->count <= 0) {
      __enable_irq();
      __disable_irq();
    }

    semaphore->count--;
    __enable_irq();
    return RTOS_OK;
    // Timeout value is a given number of ticks, will do it once and check if
    // desired time has been reached
  } else {
    do {
      __enable_irq();
      __disable_irq();
    } while ((uint32_t)(startTicks - rtosGetSysTickCount()) < timeout && semaphore->count <= 0);

    if (semaphore->count <= 0) {
      return RTOS_ERROR_TIMEOUT;
    } else {
      semaphore->count--;
      return RTOS_OK;
    }
  }
}

rtosStatus_t rtosSemaphoreRelease(const rtosSemaphoreHandle_t semaphore) {
  // Disable the IRQ and then increment the count
  __disable_irq();
  semaphore->count++;
  __enable_irq();
  return RTOS_OK;
}
