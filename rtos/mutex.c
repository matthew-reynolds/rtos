/**
 * Mutex implementation
 * @author Matt Reynolds
 * @author Dawson Hemphill
 */

#include <stdint.h>

#include "mutex.h"
#include "semaphore.h"

uint32_t mutex_count = 0;

rtosStatus_t rtosMutexNew(const rtosMutexAttr_t* attrs, rtosMutexHandle_t* mutex) {

    if (mutex_count < MAX_MUTEXES) {
        osSemaphoreNew(1, 1,(rtosSemaphoreAttr_t*)attrs, (rtosSemaphoreHandle_t)mutex);
        return RTOS_OK;
    } else {
        return RTOS_ERROR;
    }
}

rtosStatus_t rtosMutexDelete(const rtosMutexHandle_t mutex) {
    free(mutex);
    return RTOS_OK;
}

rtosStatus_t rtosMutexAcquire(const rtosMutexHandle_t mutex, uint32_t timeout) {
    return osSemaphoreAcquire((rtosSemaphoreHandle_t)mutex, timeout);
}

rtosStatus_t rtosMutexRelease(const rtosMutexHandle_t mutex) {
    return osSemaphoreRelease((rtosSemaphoreHandle_t)mutex);
}
