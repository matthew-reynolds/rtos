/**
 * Semaphore implementation
 * @author Matt Reynolds
 * @author Dawson Hemphill
 */

#include <stdint.h>
#include <stdlib.h>

#include "rtos.h"
#include "semaphore.h"

/**
 * Create a new semaphore
 *
 * @param max       The maximum value the semaphore can hold
 * @param init      The initial value of the semaphore
 * @param attrs     Any additional semaphore attributes
 * @param semaphore The semaphore object to initialize
 *
 * @return  - RTOS_OK on success
 *          - RTOS_ERROR_PARAMETER if the semaphore is NULL or invalid
 */
rtosStatus_t rtosSemaphoreNew(uint32_t                   max,
                              uint32_t                   init,
                              const rtosSemaphoreAttr_t* attrs,
                              rtosSemaphoreHandle_t      semaphore) {

  // Ensure the semaphore handle is valid
  if (semaphore == NULL) {
    return RTOS_ERROR_PARAMETER;
  }

  // Initialize the semaphore struct fields
  semaphore->name  = attrs->name;
  semaphore->count = init;
  semaphore->max   = max;

  return RTOS_OK;
}

/**
 * Delete the specified semaphore object
 *
 * @return  - RTOS_OK on success
 *          - RTOS_ERROR_PARAMETER if the semaphore is NULL or invalid
 */
rtosStatus_t rtosSemaphoreDelete(rtosSemaphoreHandle_t semaphore) {

  // Ensure the semaphore handle is valid
  if (semaphore == NULL) {
    return RTOS_ERROR_PARAMETER;
  }

  // TODO: Clean up

  return RTOS_OK;
}

/**
 * Attempt to acquire (decrement) the specified semaphore
 *
 * @return  - RTOS_OK on success
 *          - RTOS_ERROR_TIMEOUT if the semaphore could not be acquired in the specified timeout
 *          - RTOS_ERROR_PARAMETER if the semaphore is NULL or invalid
 *          - RTOS_ERROR_PARAMETER if the semaphore could not be acquired and no timeout was specified
 */
rtosStatus_t rtosSemaphoreAcquire(rtosSemaphoreHandle_t semaphore, uint32_t timeout) {

  // Ensure the semaphore handle is valid
  if (semaphore == NULL) {
    return RTOS_ERROR_PARAMETER;
  }

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

/**
 * Release (increment) the specified semaphore
 *
 * @return  - RTOS_OK on success
 *          - RTOS_ERROR_PARAMETER if the semaphore is NULL or invalid
 */
rtosStatus_t rtosSemaphoreRelease(rtosSemaphoreHandle_t semaphore) {

  // Ensure the semaphore handle is valid
  if (semaphore == NULL) {
    return RTOS_ERROR_PARAMETER;
  }

  // Disable the IRQ and then increment the count
  __disable_irq();
  semaphore->count++;
  __enable_irq();

  return RTOS_OK;
}
