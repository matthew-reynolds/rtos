/**
 * Context switch header file
 * @author Andrew Morton, 2018
 */
#ifndef __RTOS_CONTEXT_H
#define __RTOS_CONTEXT_H

#include <stdint.h>

uint32_t rtosStoreContext(void);
void     rtosRestoreContext(uint32_t sp);
void     popR4(void);
void     pushR4(void);
void     popR4R6(void);
void     pushR4R6(void);
void     popR4R10(void);
void     pushR4R10(void);

#endif  // __RTOS_CONTEXT_H
