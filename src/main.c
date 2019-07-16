/**
 * main.c
 */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../rtos/rtos.h"

rtosSemaphore_t semaphore;

void task1(void* arg) {
  uint32_t task_id = (uint32_t) arg;

  rtosDelay(500);

  while (true) {
    rtosDelay(900);
    printf("Task 1 - Attempting acquire...\n");
    switch (rtosSemaphoreAcquire(&semaphore, 100)) {
      case RTOS_OK:
        printf("Task 1 - Status OK...\n");
        break;
      case RTOS_ERROR:
        printf("Task 1 - Status ERROR...\n");
        break;
      case RTOS_ERROR_TIMEOUT:
        printf("Task 1 - Status timeout...\n");
        break;
      case RTOS_ERROR_RESOURCE:
        printf("Task 1 - Status resource...\n");
        break;
      case RTOS_ERROR_PARAMETER:
        printf("Task 1 - Status parameter...\n");
        break;
    }
  }
}

void task2(void* arg) {
  uint32_t task_id = (uint32_t) arg;

  while (true) {
    rtosDelay(2000);
    printf("Task 2 - Attempting release...\n");
    rtosSemaphoreRelease(&semaphore);
  }
}

int main(void) {
  rtosInitialize();
  rtosTaskHandle_t tcb1;
  rtosTaskHandle_t tcb2;
  rtosTaskNew(task1, (void*) 1, RTOS_PRIORITY_NORMAL, &tcb1);
  rtosTaskNew(task2, (void*) 2, RTOS_PRIORITY_NORMAL, &tcb2);

  rtosSemaphoreNew(0, 0, NULL, &semaphore);

  rtosBegin();
}
