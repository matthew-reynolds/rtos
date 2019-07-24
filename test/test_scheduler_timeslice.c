/**
 * test_scheduler_timeslice.c
 *
 * Test scheduler
 */
#if TEST_SCHEDULER_TIMESLICE

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../rtos/rtos.h"

#define NUM_COUNTERS 4

uint32_t buffers[NUM_COUNTERS];

void task_count(void* arg) {
  const uint32_t task_id = (uint32_t) arg;

  while (true) {
    buffers[task_id] = rtosGetSysTickCount();
  }
}


void task_display(void* arg) {
  uint32_t task_id = (uint32_t) arg;

  uint32_t       last_time = rtosGetSysTickCount();
  const uint32_t step      = 100 * task_id;

  while (true) {
    rtosDelayUntil(last_time + step);
    last_time += step;

    for (uint32_t task_id = 0; task_id < NUM_COUNTERS; task_id++) {
      printf("%d:%d\t", task_id, buffers[task_id]);
    }
    printf("\n");
  }
}

int main(void) {
  printf("\n\n\n\n\n");

  rtosInitialize();

  for (uint32_t task_id = 0; task_id < NUM_COUNTERS; task_id++) {
    rtosTaskNew(task_count, (void*) task_id, RTOS_PRIORITY_NORMAL, NULL);
  }
  rtosTaskNew(task_display, (void*) NUM_COUNTERS, RTOS_PRIORITY_HIGH, NULL);

  rtosBegin();
}

#endif
