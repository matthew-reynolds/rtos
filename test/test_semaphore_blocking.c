/**
 * test_mutex_prioinherit.c
 *
 * Test mutex priority inheritance
 */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../rtos/rtos.h"

rtosMutex_t printMutex;
uint32_t itr = 0;



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
			printf("Tasks Have been synced\n");
			rtosMutexRelease(&printMutex);
		}
	} rtosMutexRelease(&b->mutex);
	rtosSemaphoreAcquire(&b->turnstile2, RTOS_WAIT_FOREVER);
	rtosSemaphoreRelease(&b->turnstile2);
}

barrier_t b;
void task(void *args) {
	while(true) {
		rtosMutexAcquire(&printMutex, RTOS_WAIT_FOREVER);
		printf("%s\n", (char const *)args);
		rtosMutexRelease(&printMutex);

		rtosDelay(rtosGetSysTickFreq()/(rand()%3 + 1));
		sync(&b);
		rtosMutexAcquire(&printMutex, RTOS_WAIT_FOREVER);
		printf("Finished Sync");
		itr++;
		rtosMutexRelease(&printMutex);
	}
}

void test_semaphore_blocking(void) {
	srand(100);
	rtosInitialize();
	rtosMutexNew(NULL, &printMutex);
	init(&b, 3);
	rtosTaskNew(task, NULL, RTOS_PRIORITY_NORMAL, NULL);
	rtosTaskNew(task, NULL, RTOS_PRIORITY_NORMAL, NULL);
	rtosTaskNew(task, NULL, RTOS_PRIORITY_NORMAL, NULL);
	rtosBegin();
	for( ; ; ) ;
}
