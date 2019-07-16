/**
 * main.c
 */
#include <stdbool.h>
#include <stdlib.h>

#include "../rtos/rtos.h"

rtosSemaphore_t semaphore;

void task1(void* arg) {
  uint32_t task_id = (uint32_t) arg;

  while (true) {
    rtosDelay(5);
    rtosSemaphoreAcquire(&semaphore, rtosWaitForever);
  }
}

void task2(void* arg) {
  uint32_t task_id = (uint32_t) arg;

  while (true) {
    rtosDelay(100);
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
