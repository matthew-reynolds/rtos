/**
 * Semaphore implementation
 * @author Matt Reynolds
 * @author Dawson Hemphill
 */

#include <stdint.h>
#include <stdlib.h>

#include "globals.h"
#include "rtos.h"
#include "semaphore.h"

rtosSemaphoreHandle_t rtos_semaphores = NULL;

/**
 * Create a new semaphore
 *
 * @param max       The maximum value the semaphore can hold
 * @param init      The initial value of the semaphore
 * @param attrs     Any additional semaphore attributes
 * @param semaphore The semaphore object to initialize
 *
 * @return  - RTOS_OK               on success
 *          - RTOS_ERROR_PARAMETER  if the semaphore is NULL or invalid
 */
rtosStatus_t rtosSemaphoreNew(uint32_t                   max,
                              uint32_t                   init,
                              const rtosSemaphoreAttr_t* attrs,
                              rtosSemaphoreHandle_t      semaphore) {

  // Ensure the semaphore handle and initial value are valid
  if (semaphore == NULL || init > max) {
    return RTOS_ERROR_PARAMETER;
  }

  // Initialize the semaphore struct fields
  semaphore->name    = attrs->name;
  semaphore->count   = init;
  semaphore->max     = max;
  semaphore->blocked = NULL;

  // Add the semaphore to the global list of semaphores
  semaphore->next = rtos_semaphores;
  rtos_semaphores = semaphore;

  return RTOS_OK;
}

/**
 * Delete the specified semaphore object
 *
 * @return  - RTOS_OK               on success
 *          - RTOS_ERROR_PARAMETER  if the semaphore is NULL or invalid
 */
rtosStatus_t rtosSemaphoreDelete(rtosSemaphoreHandle_t semaphore) {

  // Ensure the semaphore handle is valid
  if (semaphore == NULL) {
    return RTOS_ERROR_PARAMETER;
  }

  // Remove the semaphore from the global list of semaphores
  rtosSemaphoreHandle_t prev_sem = NULL;
  rtosSemaphoreHandle_t cur_sem  = rtos_semaphores;
  while (cur_sem != NULL) {
    if (cur_sem == semaphore) {
      if (prev_sem == NULL) {
        rtos_semaphores = cur_sem->next;
      } else {
        prev_sem->next = cur_sem->next;
      }
      break;
    }
    cur_sem = cur_sem->next;
  }

  // Unblock all blocked tasks
  // NOTE: Tasks unblocked via semaphore deletion return a unique error since the semaphore never became available
  __disable_irq();
  while (semaphore->blocked != NULL) {
    rtosTaskHandle_t unblocked = rtosPopTaskListHead(&semaphore->blocked);
    rtosInsertTaskListTail(rtosGetReadyTaskQueue(unblocked->priority), unblocked);
  }
  __enable_irq();

  rtosInvokeScheduler();

  return RTOS_OK;
}

/**
 * Attempt to acquire (decrement) the specified semaphore
 *
 * @return  - RTOS_OK               on success
 *          - RTOS_ERROR            if the semaphore was deleted while trying to acquire it
 *          - RTOS_ERROR_TIMEOUT    if the semaphore could not be acquired in the specified timeout
 *          - RTOS_ERROR_PARAMETER  if the semaphore is NULL or invalid
 *          - RTOS_ERROR_RESOURCE   if the semaphore could not be acquired and no timeout was specified
 */
rtosStatus_t rtosSemaphoreAcquire(rtosSemaphoreHandle_t semaphore, uint32_t timeout) {

  // Ensure the semaphore handle is valid
  if (semaphore == NULL) {
    return RTOS_ERROR_PARAMETER;
  }

  // Disable interrupts to ensure count is read and written atomically
  __disable_irq();

  // Timeout value is set to zero so try once and then exit
  if (timeout == 0) {
    if (semaphore->count == 0) {
      __enable_irq();
      return RTOS_ERROR_RESOURCE;
    }

    semaphore->count--;
    __enable_irq();
    return RTOS_OK;
  }

  // Timeout value is set to forever so block until the semaphore is available
  else if (timeout == RTOS_WAIT_FOREVER) {

    // If the semaphore is unavailable, block the current task
    while (semaphore->count == 0) {
      rtos_running_task->state = RTOS_TASK_BLOCKED;
      rtosInsertTaskListTail(&semaphore->blocked, rtos_running_task);

      __enable_irq();
      rtosInvokeScheduler();
      __disable_irq();
    }

    // Once the semaphore is available, acquire it
    semaphore->count--;
    __enable_irq();
    return RTOS_OK;

  }

  // Timeout value is a given number of ticks, block until the semaphore is available or the timeout expires
  else {

    // If the semaphore is unavailable, block the current task
    while (semaphore->count == 0) {
      rtos_running_task->state           = RTOS_TASK_BLOCKED_TIMEOUT;
      rtos_running_task->wake_time_ticks = rtosGetSysTickCount() + timeout;
      rtosInsertTaskListTail(&semaphore->blocked, rtos_running_task);

      __enable_irq();
      rtosInvokeScheduler();
      __disable_irq();
    }

    // Once the semaphore is available, acquire it
    semaphore->count--;
    __enable_irq();
    return RTOS_OK;
  }
}

/**
 * Release (increment) the specified semaphore
 *
 * @return  - RTOS_OK               on success
 *          - RTOS_ERROR_RESOURCE   if the semaphore could not be released (count == max)
 *          - RTOS_ERROR_PARAMETER  if the semaphore is NULL or invalid
 */
rtosStatus_t rtosSemaphoreRelease(rtosSemaphoreHandle_t semaphore) {

  // Ensure the semaphore handle is valid
  if (semaphore == NULL) {
    return RTOS_ERROR_PARAMETER;
  }

  // Disable interrupts
  __disable_irq();

  // Ensure the maximum value has not been reached
  if (semaphore->count == semaphore->max) {
    __enable_irq();
    return RTOS_ERROR_RESOURCE;
  }

  // Increment the count
  semaphore->count++;

  // If there are blocked tasks, unblock the first task in the queue
  if (semaphore->blocked != NULL) {
    rtosTaskHandle_t unblocked = rtosPopTaskListHead(&semaphore->blocked);
    unblocked->state           = RTOS_TASK_READY;
    rtosInsertTaskListTail(rtosGetReadyTaskQueue(unblocked->priority), unblocked);

    __enable_irq();
    rtosInvokeScheduler();
    __disable_irq();
  }

  __enable_irq();
  return RTOS_OK;
}
