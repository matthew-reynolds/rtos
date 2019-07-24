/**
 * test_mutex_prioinherit.c
 *
 * Test mutex priority inheritance
 */
#if TEST_MUTEX_PRIOINHERIT

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../rtos/rtos.h"

rtosMutex_t      mutex;
rtosTaskHandle_t tcb1;
rtosTaskHandle_t tcb2;
rtosTaskHandle_t tcb3;

void task1(void* arg) {

  rtosDelay(1000);

  printf("High priority task: Attempting acquire mutex...\n");
  rtosStatus_t stat = rtosMutexAcquire(&mutex, RTOS_WAIT_FOREVER);
  printf("High priority task: Acquired mutex!\n");

  printf("High priority task: Releasing mutex...\n");
  rtosMutexRelease(&mutex);
  printf("High priority task: Released mutex!\n");

  rtosTaskExit();
}

void task2(void* arg) {

  rtosDelay(500);

  printf("Med  priority task: Starting long task...\n");
  while (true) {
  }
}

void task3(void* arg) {

  printf("Low  priority task: Attempting acquire mutex...\n");
  rtosMutexAcquire(&mutex, RTOS_WAIT_FOREVER);
  printf("Low  priority task: Acquired mutex\n");

  printf("Low  priority task: Starting long task...\n");
  uint32_t i = 0;
  while (i < 50000000) {
    i++;
  }
  printf("Low  priority task: Finished long task!\n");
  printf("Low  priority task: Releasing mutex...\n");
  rtosMutexRelease(&mutex);
  printf("Low  priority task: Released mutex!\n");
  rtosTaskExit();
}

int main(void) {
  printf("\n\n\n\n\n");

  rtosInitialize();
  rtosTaskNew(task1, NULL, RTOS_PRIORITY_HIGH, &tcb1);    // High prio
  rtosTaskNew(task2, NULL, RTOS_PRIORITY_NORMAL, &tcb2);  // Med prio
  rtosTaskNew(task3, NULL, RTOS_PRIORITY_LOW, &tcb3);     // Low prio

  // If priority inherit disabled, high-prio task should fail to acquire and deadlock
  // If priority inherit enabled, low-prio task will be elevated and avoid deadlock
  rtosMutexAttr_t attributes = {"", RTOS_MUTEX_PRIO_INHERIT};
  // rtosMutexAttr_t attributes = {"", 0};
  rtosMutexNew(&attributes, &mutex);

  rtosBegin();
}

#endif
