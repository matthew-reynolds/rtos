/**
 * Mutexes
 * @author Matt Reynolds
 * @author Dawson Hemphill
 */
#ifndef __RTOS_MUTEX_H
#define __RTOS_MUTEX_H

#include <stdint.h>

#include "status.h"
#include "task.h"

// Define mutex attribute options
#define RTOS_MUTEX_PRIO_INHERIT 0x00000002U
#define RTOS_MUTEX_ROBUST 0x00000008U

/// Mutex attributes
typedef struct {
  const char* name;
  uint32_t    attr_bits;
} rtosMutexAttr_t;

/// Mutex
typedef struct rtosMutex_tag {
  const char*           name;           ///< The name of the mutex
  uint32_t              count;          ///< The current mutex value
  uint32_t              attr_bits;      ///< Attribute bits. Default=0
  rtosTaskHandle_t      blocked;        ///< The list of tasks blocked by the mutex
  rtosTaskHandle_t      acquired;       ///< The task that acquired the mutex
  rtosPriority_t        init_priority;  ///< The priority of acquired
  struct rtosMutex_tag* next;           ///< The next mutex in the global list
} rtosMutex_t;

typedef rtosMutex_t* rtosMutexHandle_t;

rtosStatus_t rtosMutexNew(const rtosMutexAttr_t* attrs, rtosMutexHandle_t mutex);
rtosStatus_t rtosMutexDelete(rtosMutexHandle_t mutex);
rtosStatus_t rtosMutexAcquire(rtosMutexHandle_t mutex, uint32_t timeout);
rtosStatus_t rtosMutexRelease(rtosMutexHandle_t mutex);

#endif  // __RTOS_MUTEX_H
