/**
 * main.c
 */
#include <stdbool.h>
#include <stdlib.h>

#include "../rtos/rtos.h"

void task1(void* arg) {
  uint32_t task_id = (uint32_t) arg;

  while (true) {
    unsigned i = 1;
    while (i < 1000)
      i++;
  }
}

int main(void) {
  rtosInitialize();
  rtosTaskControlBlock_t* tcb1;
  rtosTaskControlBlock_t* tcb2;
  rtosTaskNew(&tcb1, task1, (void*) 1, RTOS_PRIORITY_NORMAL);
  rtosTaskNew(&tcb2, task1, (void*) 2, RTOS_PRIORITY_NORMAL);
  rtosBegin();
}
