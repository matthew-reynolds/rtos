/**
 * Mutex implementation
 * @author Matt Reynolds
 * @author Dawson Hemphill
 */

#include <stdint.h>
#include <stdlib.h>

#include "globals.h"
#include "mutex.h"
#include "rtos.h"

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
  mutex->name          = attrs->name;
  mutex->attr_bits     = attrs->attr_bits;
  mutex->count         = 1;
  mutex->acquired      = NULL;
  mutex->blocked       = NULL;
  mutex->init_priority = RTOS_PRIORITY_NONE;

  // Add the mutex to the global list of mutex
  mutex->next  = rtos_mutexes;
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

  // Remove the mutex from the global list of mutexes
  rtosMutexHandle_t prev_mutex = NULL;
  rtosMutexHandle_t cur_mutex  = rtos_mutexes;
  while (cur_mutex != NULL) {
    if (cur_mutex == mutex) {
      if (prev_mutex == NULL) {
        rtos_mutexes = cur_mutex->next;
      } else {
        prev_mutex->next = cur_mutex->next;
      }
      break;
    }
    cur_mutex = cur_mutex->next;
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
 * Attempt to acquire the specified mutex
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

    mutex->count         = 0;
    mutex->acquired      = rtos_running_task;
    mutex->init_priority = rtos_running_task->priority;
    __enable_irq();
    return RTOS_OK;
  }

  // Timeout value is set to forever so block until the mutex is available
  else if (timeout == RTOS_WAIT_FOREVER) {

    // If the mutex is unavailable, block the current task
    while (mutex->count == 0) {
      rtos_running_task->state = RTOS_TASK_BLOCKED;
      rtosInsertTaskListTail(&mutex->blocked, rtos_running_task);

      // If priority inheritance is enabled, promote the priority of the task that acquired the mutex
      if ((mutex->attr_bits & RTOS_MUTEX_PRIO_INHERIT) && rtos_running_task->priority > mutex->acquired->priority) {
        mutex->acquired->priority = rtos_running_task->priority;
        rtosInsertTaskListTail(rtosGetReadyTaskQueue(mutex->acquired->priority), mutex->acquired);
      }

      __enable_irq();
      rtosInvokeScheduler();
      __disable_irq();
    }

    // Once the mutex is available, acquire it
    mutex->count         = 0;
    mutex->acquired      = rtos_running_task;
    mutex->init_priority = rtos_running_task->priority;
    __enable_irq();
    return RTOS_OK;

  }

  // Timeout value is a given number of ticks, block until the mutex is available or the timeout expires
  else {

    // If the mutex is unavailable, block the current task
    while (mutex->count == 0) {
      rtos_running_task->state           = RTOS_TASK_BLOCKED_TIMEOUT;
      rtos_running_task->wake_time_ticks = rtosGetSysTickCount() + timeout;
      rtosInsertTaskListTail(&mutex->blocked, rtos_running_task);

      // If priority inheritance is enabled, promote the priority of the task that acquired the mutex
      if ((mutex->attr_bits & RTOS_MUTEX_PRIO_INHERIT) && rtos_running_task->priority > mutex->acquired->priority) {
        mutex->acquired->priority = rtos_running_task->priority;
        rtosInsertTaskListTail(rtosGetReadyTaskQueue(mutex->acquired->priority), mutex->acquired);
      }

      __enable_irq();
      rtosInvokeScheduler();
      __disable_irq();
    }

    // Once the mutex is available, acquire it
    mutex->count         = 0;
    mutex->acquired      = rtos_running_task;
    mutex->init_priority = rtos_running_task->priority;
    __enable_irq();
    return RTOS_OK;
  }
}

/**
 * Release the specified mutex
 *
 * @return  - RTOS_OK               on success
 *          - RTOS_ERROR_PARAMETER  if the mutex is NULL or invalid
 *          - RTOS_ERROR_RESOURCE   if the mutex is already released or releasing thread did not acquire the mutex
 */
rtosStatus_t rtosMutexRelease(const rtosMutexHandle_t mutex) {

  // Ensure the mutex handle is valid
  if (mutex == NULL) {
    return RTOS_ERROR_PARAMETER;
  }

  // Disable interrupts
  __disable_irq();

  // Ensure the mutex is acquired and that the releasing thread
  if (mutex->count == 1 || rtos_running_task != mutex->acquired) {
    __enable_irq();
    return RTOS_ERROR_RESOURCE;
  }

  // Release the mutex
  mutex->count = 1;

  // If there are blocked tasks, unblock the first task
  if (mutex->blocked != NULL) {
    rtosTaskHandle_t unblocked = rtosPopTaskListHead(&mutex->blocked);
    unblocked->state           = RTOS_TASK_READY;
    rtosInsertTaskListTail(rtosGetReadyTaskQueue(unblocked->priority), unblocked);

    // If priority inheritance is enabled, ensure the priority is demoted back to its original value
    if ((mutex->attr_bits & RTOS_MUTEX_PRIO_INHERIT) && rtos_running_task->priority != mutex->init_priority) {
      rtos_running_task->priority = mutex->init_priority;
    }

    __enable_irq();
    rtosInvokeScheduler();
    __disable_irq();
  }

  __enable_irq();
  return RTOS_OK;
}
