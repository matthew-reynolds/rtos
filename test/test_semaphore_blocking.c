/**
 * test_semaphore_blocking.c
 *
 * Test blocking semaphores using a barrier
 */
#if TEST_SEMAPHORE

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../rtos/rtos.h"

typedef struct {
  rtosMutex_t     mutex;
  rtosSemaphore_t turnstile1, turnstile2;
  uint32_t        count, n;
} barrier_t;

void init(barrier_t* b, uint32_t n) {
  rtosMutexNew(NULL, &b->mutex);
  rtosSemaphoreNew(n, 0, NULL, &b->turnstile1);
  rtosSemaphoreNew(n, 1, NULL, &b->turnstile2);
  b->count = 0;
  b->n     = n;
}

barrier_t        b;
rtosMutex_t      print_mutex;
rtosTaskHandle_t task1;
rtosTaskHandle_t task2;
rtosTaskHandle_t task3;

void sync(barrier_t* b) {

  // Increment the count
  rtosMutexAcquire(&b->mutex, RTOS_WAIT_FOREVER);
  b->count++;
  if (b->count == b->n) {
    rtosSemaphoreAcquire(&b->turnstile2, RTOS_WAIT_FOREVER);
    rtosSemaphoreRelease(&b->turnstile1);
  }
  rtosMutexRelease(&b->mutex);

  // Wait on turnstile 1
  rtosSemaphoreAcquire(&b->turnstile1, RTOS_WAIT_FOREVER);
  rtosSemaphoreRelease(&b->turnstile1);

  // Decrement the count
  rtosMutexAcquire(&b->mutex, RTOS_WAIT_FOREVER);
  b->count--;
  if (b->count == 0) {
    rtosSemaphoreAcquire(&b->turnstile1, RTOS_WAIT_FOREVER);
    rtosSemaphoreRelease(&b->turnstile2);

    rtosMutexAcquire(&print_mutex, RTOS_WAIT_FOREVER);
    printf("Tasks have reached the barrier\n");
    rtosMutexRelease(&print_mutex);
  }
  rtosMutexRelease(&b->mutex);

  // Wait on turnstile 2
  rtosSemaphoreAcquire(&b->turnstile2, RTOS_WAIT_FOREVER);
  rtosSemaphoreRelease(&b->turnstile2);
}

void task(void* args) {
  uint32_t task_id = (uint32_t) args;

  while (true) {
    rtosDelay(rtosGetSysTickFreq() / (rand() % 3 + 1));

    rtosMutexAcquire(&print_mutex, RTOS_WAIT_FOREVER);
    printf("Task %d: Syncing...\n", task_id);
    rtosMutexRelease(&print_mutex);

    sync(&b);

    rtosMutexAcquire(&print_mutex, RTOS_WAIT_FOREVER);
    printf("Task %d: Finished Sync\n", task_id);
    rtosMutexRelease(&print_mutex);
  }
}

int main(void) {
  printf("\n\n\n\n\n");

  srand(100);
  rtosInitialize();

  init(&b, 3);

  rtosTaskNew(task, (void*) 1, RTOS_PRIORITY_NORMAL, &task1);
  rtosTaskNew(task, (void*) 2, RTOS_PRIORITY_NORMAL, &task2);
  rtosTaskNew(task, (void*) 3, RTOS_PRIORITY_NORMAL, &task3);
  rtosMutexNew(NULL, &print_mutex);

  rtosBegin();
}

#endif
