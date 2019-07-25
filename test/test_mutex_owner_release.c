/**
 * test_mutex_prioinherit.c
 *
 * Test mutex priority inheritance
 */
#if TEST_MUTEX_OWNER_RELEASE

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../rtos/rtos.h"

rtosMutex_t      mutex;
rtosTaskHandle_t tcb1;
rtosTaskHandle_t tcb2;

void task1(void* arg) {

  printf("First Task: Attempting acquire mutex...\n");
  rtosMutexAcquire(&mutex, RTOS_WAIT_FOREVER);
  printf("First Task: Acquired mutex!\n");

  rtosDelay(1000);
  
  printf("First Task: Releasing mutex...\n");
  rtosMutexRelease(&mutex);
  printf("First Task: Successfully released mutex!\n");

  rtosTaskExit();
}

void task2(void* arg) {

  rtosDelay(500);
  printf("Second Task: Attempting to release mutex\n");
  rtosStatus_t stat = rtosMutexRelease(&mutex);

  switch (stat) {
    case RTOS_OK:
      printf("Second Task: Successfully released mutex\n");
      break;
    case RTOS_ERROR_RESOURCE:
      printf("Second Task: Could not release mutex since not the owner\n");
      break;
  }
  rtosTaskExit();
}

int main(void) {
  printf("\n\n\n\n\n");

  rtosInitialize();
  rtosTaskNew(task1, NULL, RTOS_PRIORITY_NORMAL, &tcb1);  
  rtosTaskNew(task2, NULL, RTOS_PRIORITY_NORMAL, &tcb2);

  // If priority inherit disabled, high-prio task should fail to acquire and deadlock
  // If priority inherit enabled, low-prio task will be elevated and avoid deadlock
  // rtosMutexAttr_t attributes = {"", 0};
  rtosMutexNew(NULL, &mutex);

  rtosBegin();
}

#endif
