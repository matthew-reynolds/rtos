/**
 * Main RTOS include file
 * @author Matt Reynolds
 * @author Dawson Hemphill
 */
#ifndef __RTOS_RTOS_H
#define __RTOS_RTOS_H

#include <stdint.h>

#include "globals.h"
#include "mutex.h"
#include "scheduler.h"
#include "semaphore.h"
#include "task.h"

uint32_t rtosGetSysTickCount(void);
uint32_t rtosGetSysTickFreq(void);
void     rtosSetSysTickFreq(uint32_t freq);

void rtosInitialize(void);
void rtosBegin(void);

#endif  // __RTOS_RTOS_H
