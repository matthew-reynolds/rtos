/**
 * test_mutex_prioinherit.c
 *
 * Test mutex priority inheritance
 */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../rtos/rtos.h"

rtosMutex_t      mutex;
rtosTaskHandle_t tcb1;
rtosTaskHandle_t tcb2;
rtosTaskHandle_t tcb3;

void task1(void* arg) {

  rtosDelay(500);

  rtosStatus_t stat = rtosMutexAcquire(&mutex, RTOS_WAIT_FOREVER);
  switch (stat) {
    case RTOS_OK:
      printf("Task 1 - Status OK...\n");
      break;
    case RTOS_ERROR:
      printf("Task 1 - Status ERROR...\n");
      break;
    case RTOS_ERROR_TIMEOUT:
      printf("Task 1 - Status TIMEOUT...\n");
      break;
    case RTOS_ERROR_RESOURCE:
      printf("Task 1 - Status RESOURCE...\n");
      break;
    case RTOS_ERROR_PARAMETER:
      printf("Task 1 - Status PARAMETER...\n");
      break;
  }
  rtosMutexRelease(&mutex);

  rtosTaskExit();
}

void task2(void* arg) {

  rtosDelay(100);

  while (true) {
    int i = 0;
    // asm("nop")
  }
}

void task3(void* arg) {

  rtosMutexAcquire(&mutex, RTOS_WAIT_FOREVER);
  uint32_t i = 0;
  while (i < 100000000) {
    i++;
  }
  rtosMutexRelease(&mutex);
  rtosTaskExit();
}

// Essentially main()
void test_mutex_prioinherit(void) {
  printf("\n\n\n\n\n");

  rtosInitialize();
  rtosTaskNew(task1, NULL, RTOS_PRIORITY_HIGH, &tcb1);    // High prio
  rtosTaskNew(task2, NULL, RTOS_PRIORITY_NORMAL, &tcb2);  // Med prio
  rtosTaskNew(task3, NULL, RTOS_PRIORITY_LOW, &tcb3);     // Low prio

  rtosMutexAttr_t attributes = {"", RTOS_MUTEX_PRIO_INHERIT};  // RTOS_MUTEX_PRIO_INHERIT
  rtosMutexNew(&attributes, &mutex);

  rtosBegin();
}
