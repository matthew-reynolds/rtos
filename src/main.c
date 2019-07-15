/**
 * main.c
 */
#include <stdbool.h>
#include <stdlib.h>

#include "../rtos/rtos.h"

void task1(void* arg) {
  uint32_t task_id = (uint32_t) arg;

  while (true) {
    rtosDelay(task_id * 100);
  }
}

int main(void) {
  rtosInitialize();
  rtosTaskHandle_t tcb1;
  rtosTaskHandle_t tcb2;
  rtosTaskNew(task1, (void*) 1, RTOS_PRIORITY_NORMAL, &tcb1);
  rtosTaskNew(task1, (void*) 2, RTOS_PRIORITY_NORMAL, &tcb2);
  rtosBegin();
}
