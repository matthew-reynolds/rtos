#include "semaphore.h"


rtosSemaphore_t* osSemaphoreNew (uint32_t max, uint32_t init, const rtosSemaphoreAttr_t* attrs) {
    const char name = attrs->name;
    rtosSemaphore_t *name = malloc(sizeof(rtosSemaphore_t));
    name->max = max;

}

rtosStatus_t     osSemaphoreDelete(rtosSemaphore_t* id) {
    free(id);
}

rtosStatus_t     osSemaphoreAcquire(rtosSemaphore_t* id, uint32_t timeout){

}

rtosStatus_t     osSemaphoreRelease(rtosSemaphore_t* id){
    
}