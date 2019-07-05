/**
 * Context switch header file
 * @author Andrew Morton, 2018
 */
#ifndef __RTOS_CONTEXT_H
#define __RTOS_CONTEXT_H

#include <stdint.h>

uint32_t rtosStoreContext(void);
void     rtosRestoreContext(uint32_t sp);

#endif  // __RTOS_CONTEXT_H
