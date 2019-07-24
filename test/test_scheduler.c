/**
 * test_scheduler.c
 *
 * Test mutex priority inheritance
 */
#if TEST_SCHEDULER
 
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../rtos/rtos.h"

rtosMutex_t print_mutex;

void task(void* arg) {
  uint32_t task_id = (uint32_t) arg;

  uint32_t       last_time = rtosGetSysTickCount();
  const uint32_t step      = 100 * task_id;

  while (true) {
    rtosDelayUntil(last_time + step);
    last_time += step;
    rtosMutexAcquire(&print_mutex, RTOS_WAIT_FOREVER);
    printf("%d\t%d\n", rtosGetSysTickCount(), task_id);
    rtosMutexRelease(&print_mutex);
  }
}

int main(void) {
  printf("\n\n\n\n\n");

  rtosInitialize();
  rtosTaskNew(task, (void*) 1, RTOS_PRIORITY_HIGH, NULL);
  rtosTaskNew(task, (void*) 2, RTOS_PRIORITY_NORMAL, NULL);
  rtosTaskNew(task, (void*) 3, RTOS_PRIORITY_NORMAL, NULL);
  rtosTaskNew(task, (void*) 4, RTOS_PRIORITY_LOW, NULL);
  rtosTaskNew(task, (void*) 5, RTOS_PRIORITY_LOW, NULL);

  rtosMutexAttr_t attributes = {"", RTOS_MUTEX_PRIO_INHERIT};
  rtosMutexNew(&attributes, &print_mutex);
  
  rtosBegin();
}

#endif
