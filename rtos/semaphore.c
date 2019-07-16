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

rtosStatus_t osSemaphoreNew(uint32_t                   max,
                            uint32_t                   init,
                            const rtosSemaphoreAttr_t* attrs,
                            rtosSemaphoreHandle_t*     semaphore) {

  if (sem_count < MAX_SEMAPHORES) {
    const char*      name = attrs->name;
    rtosSemaphore_t* sem  = malloc(sizeof(rtosSemaphore_t));
    sem->name             = name;
    sem->count            = init;
    sem->max              = max;
    sem_count++;
    return RTOS_OK;
  } else {
    return RTOS_ERROR;
  }
}

rtosStatus_t osSemaphoreDelete(rtosSemaphore_t* id) {

  free(id);
  return RTOS_OK;
}

rtosStatus_t osSemaphoreAcquire(rtosSemaphore_t* id, uint32_t timeout) {

  uint32_t startTicks = rtosGetSysTickCount();
  __disable_irq();

  // Timeout value is set to zero so try once and then exit
  if (timeout == 0) {
    if (id->count <= 0) {
      return RTOS_ERROR_RESOURCE;
    } else {

      id->count--;
      __enable_irq();
      return RTOS_OK;
    }
    // Timeout value is set to wat forever so loop until it is ready
  } else if (timeout == rtosWaitForever) {
    while (id->count <= 0) {
      __enable_irq();
      __disable_irq();
    }

    id->count--;
    __enable_irq();
    return RTOS_OK;
    // Timeout value is a given number of ticks, will do it once and check if
    // desired time has been reached
  } else {
    do {
      __enable_irq();
      __disable_irq();
    } while ((uint32_t)(startTicks - rtosGetSysTickCount()) < timeout && id->count <= 0);

    if (id->count <= 0) {
      return RTOS_ERROR_TIMEOUT;
    } else {
      id->count--;
      return RTOS_OK;
    }
  }
}

rtosStatus_t osSemaphoreRelease(rtosSemaphore_t* id) {
  // Disable the IRQ and then increment the count
  __disable_irq();
  id->count++;
  __enable_irq();
  return RTOS_OK;
}
