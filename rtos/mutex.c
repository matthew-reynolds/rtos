/**
 * Mutex implementation
 * @author Matt Reynolds
 * @author Dawson Hemphill
 */

#include <stdint.h>
#include <stdlib.h>

#include "mutex.h"
#include "rtos.h"
#include "globals.h"

rtosMutexHandle_t rtos_mutexes = NULL;

/**
 * Create a new mutex
 *
 * @param attrs     Any additional mutex attributes
 * @param mutex     The mutex object to initialize
 *
 * @return  - RTOS_OK               on success
 *          - RTOS_ERROR_PARAMETER  if the mutex is NULL or invalid
 */
rtosStatus_t rtosMutexNew(const rtosMutexAttr_t* attrs, rtosMutexHandle_t mutex) {
    
  // Ensure the mutex handle is valid
  if (mutex == NULL) {
    return RTOS_ERROR_PARAMETER;
  }

  // Initialize the mutex struct fields
  mutex->attrs.name  = attrs->name;
  mutex->attrs.attr_bits = attrs->attr_bits;
  mutex->count   = 1;
  mutex->acquired = NULL;
  mutex->blocked = NULL;
  mutex->initPriority = rtos_running_task->priority;

  mutex->next = rtos_mutexes;
  rtos_mutexes = mutex;

  return RTOS_OK;
}

/**
 * Delete the specified mutex object
 *
 * @return  - RTOS_OK               on success
 *          - RTOS_ERROR_PARAMETER  if the mutex is NULL or invalid
 */
rtosStatus_t rtosMutexDelete(const rtosMutexHandle_t mutex) {

  // Ensure the mutex handle is valid
  if (mutex == NULL) {
    return RTOS_ERROR_PARAMETER;
  }

  // Unblock all blocked tasks
  // NOTE: Tasks unblocked via mutex deletion return a unique error since the mutex never became available
  __disable_irq();
  while (mutex->blocked != NULL) {
    rtosTaskHandle_t unblocked = rtosPopTaskListHead(&mutex->blocked);
    rtosInsertTaskListTail(rtosGetReadyTaskQueue(unblocked->priority), unblocked);
  }
  __enable_irq();

  rtosInvokeScheduler();

  return RTOS_OK;
}

/**
 * Attempt to acquire (decrement) the specified mutex
 *
 * @return  - RTOS_OK               on success
 *          - RTOS_ERROR            if the mutex was deleted while trying to acquire it
 *          - RTOS_ERROR_TIMEOUT    if the mutex could not be acquired in the specified timeout
 *          - RTOS_ERROR_PARAMETER  if the mutex is NULL or invalid
 *          - RTOS_ERROR_RESOURCE   if the mutex could not be acquired and no timeout was specified
 */
rtosStatus_t rtosMutexAcquire(const rtosMutexHandle_t mutex, uint32_t timeout) {
    
// Ensure the mutex handle is valid
  if (mutex == NULL) {
    return RTOS_ERROR_PARAMETER;
  }

  // Disable interrupts to ensure count is read and written atomically
  __disable_irq();

  // Timeout value is set to zero so try once and then exit
  if (timeout == 0) {
    if (mutex->count == 0) {
      __enable_irq();
      return RTOS_ERROR_RESOURCE;
    }

    mutex->count--;
    __enable_irq();
    return RTOS_OK;
  }

  // Timeout value is set to wait forever so loop until it is ready
  else if (timeout == rtosWaitForever) {

    // If the mutex is unavailable, block the current task
    if (mutex->count == 0) {
      rtosInsertTaskListTail(&mutex->blocked, rtos_running_task);
      rtos_running_task->state = RTOS_TASK_BLOCKED;

      if (rtos_running_task->priority > mutex->acquired->priority && mutex->attrs.attr_bits & rtosMutexPrioInherit) {
          mutex->acquired->priority = rtos_running_task->priority;
      }

      __enable_irq();
      rtosInvokeScheduler();
      __disable_irq();
    }

    // Task was unblocked in an unusual way, such as by mutex deletion, leaving the mutex still unavailable.
    // Return an error indicating that the mutex was deleted but is still unavailable
    if (mutex->count == 0) {
      __enable_irq();
      return RTOS_ERROR;
    }

    // Once the mutex is available, acquire it
    mutex->count--;
    mutex->acquired = rtos_running_task;
    __enable_irq();
    return RTOS_OK;

  }

  // Timeout value is a given number of ticks, will do it once and check if desired time has been reached
  else {

    // If the semaphore is unavailable, block the current task
    if (mutex->count == 0) {
      rtos_running_task->state           = RTOS_TASK_BLOCKED_TIMEOUT;
      rtos_running_task->wake_time_ticks = rtosGetSysTickCount() + timeout;
      rtosInsertTaskListTail(&mutex->blocked, rtos_running_task);

      if (rtos_running_task->priority > mutex->acquired->priority && mutex->attrs.attr_bits & rtosMutexPrioInherit) {
          mutex->acquired->priority = rtos_running_task->priority;
      }

      __enable_irq();
      rtosInvokeScheduler();
      __disable_irq();
    }

    // TODO: Detect if the task was unblocked due to the semaphore becoming available or due to the semaphore being
    // deleted
    if (mutex->count == 0) {
      __enable_irq();
      return RTOS_ERROR_TIMEOUT;
    }

    // Once the mutex is available, acquire it
    mutex->count--;
    __enable_irq();
    return RTOS_OK;
  }
}

/**
 * Release (increment) the specified mutex
 *
 * @return  - RTOS_OK               on success
 *          - RTOS_ERROR_PARAMETER  if the mutex is NULL or invalid
 *          - RTOS_ERROR_RESOURCE   if the mutex is already released or thread releasing did not acquire the mutex
 */
rtosStatus_t rtosMutexRelease(const rtosMutexHandle_t mutex) {

// Ensure the mutex handle is valid
  if (mutex == NULL) {
    return RTOS_ERROR_PARAMETER;
  }

  // TODO: Check max value

  // Disable interrupts and then increment the count
  __disable_irq();

  if (mutex->count == 1 | mutex->acquired != rtos_running_task) {
    return RTOS_ERROR_RESOURCE;
  }
  mutex->count++;

  // If there are blocked tasks, unblock the first task
  if (mutex->blocked != NULL) {
    rtosTaskHandle_t unblocked = rtosPopTaskListHead(&mutex->blocked);
    rtosInsertTaskListTail(rtosGetReadyTaskQueue(unblocked->priority), unblocked);

    if (rtos_running_task->priority != mutex->initPriority && mutex->attrs.attr_bits & rtosMutexPrioInherit) {
        rtos_running_task->priority = mutex->initPriority;
    }

    __enable_irq();
    rtosInvokeScheduler();
    __disable_irq();
  }

  __enable_irq();

  return RTOS_OK;
}
