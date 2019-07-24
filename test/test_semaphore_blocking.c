/**
 * test_mutex_prioinherit.c
 *
 * Test mutex priority inheritance
 */
#if TEST_SEMAPHORE
 
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../rtos/rtos.h"

rtosMutex_t printMutex;
rtosTaskHandle_t task1;
rtosTaskHandle_t task2;
rtosTaskHandle_t task3;

typedef struct {
	rtosMutex_t mutex;
	rtosSemaphore_t turnstile1, turnstile2;
	uint32_t count, n;
} barrier_t;

void init(barrier_t *b, uint32_t n) {
	//osMutexAttr_t attr = { .attr_bits = osMutexPrioInherit | osMutexRobust };
	//b->mutex = osMutexNew(&attr);
	rtosMutexNew(NULL, &b->mutex);
  rtosSemaphoreNew(n, 0, NULL, &b->turnstile1);
  rtosSemaphoreNew(n, 1, NULL, &b->turnstile2);
	b->count = 0;
	b->n = n;
}

void sync(barrier_t * b) {
	rtosMutexAcquire(&printMutex, RTOS_WAIT_FOREVER);
	printf("SYNC \n");
	rtosMutexRelease(&printMutex);
	rtosMutexAcquire(&b->mutex, RTOS_WAIT_FOREVER); {
		b->count++;
		if(b->count == b->n) {
			rtosSemaphoreAcquire(&b->turnstile2, RTOS_WAIT_FOREVER);
			rtosSemaphoreRelease(&b->turnstile1);
		}
	} rtosMutexRelease(&b->mutex);

	rtosSemaphoreAcquire(&b->turnstile1, RTOS_WAIT_FOREVER);
	rtosSemaphoreRelease(&b->turnstile1);

	rtosMutexAcquire(&b->mutex, RTOS_WAIT_FOREVER); {
		b->count--;
		if(b->count == 0) {
			rtosSemaphoreAcquire(&b->turnstile1, RTOS_WAIT_FOREVER);
			rtosSemaphoreRelease(&b->turnstile2);

			rtosMutexAcquire(&printMutex, RTOS_WAIT_FOREVER);
			printf("Tasks have synced\n");
			rtosMutexRelease(&printMutex);
		}
	} rtosMutexRelease(&b->mutex);
	rtosSemaphoreAcquire(&b->turnstile2, RTOS_WAIT_FOREVER);
	rtosSemaphoreRelease(&b->turnstile2);
}

barrier_t b;
void task(void *args) {
	while(true) {
		rtosDelay(rtosGetSysTickFreq()/(rand()%3 + 1));
		sync(&b);
		rtosMutexAcquire(&printMutex, RTOS_WAIT_FOREVER);
		printf("Finished Sync \n");
		rtosMutexRelease(&printMutex);
	}
}

int main(void) {
	srand(100);
	rtosInitialize();
	rtosMutexNew(NULL, &printMutex);
	init(&b, 3);
	rtosTaskNew(task, NULL, RTOS_PRIORITY_NORMAL, &task1);
	rtosTaskNew(task, NULL, RTOS_PRIORITY_NORMAL, &task2);
	rtosTaskNew(task, NULL, RTOS_PRIORITY_NORMAL, &task3);
	rtosBegin();
	for( ; ; ) ;
}

#endif
