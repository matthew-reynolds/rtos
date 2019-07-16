/**
 * Standard return status codes (analagous to errno)
 * @author Matt Reynolds
 * @author Dawson Hemphill
 */
#ifndef __RTOS_STATUS_H
#define __RTOS_STATUS_H

#define rtosWaitForever 0xFFFFFFFFU

typedef enum {
  RTOS_OK,
  RTOS_ERROR,
  RTOS_ERROR_TIMEOUT,
  RTOS_ERROR_RESOURCE,
  RTOS_ERROR_PARAMETER,
  RTOS_ERROR_NO_MEMORY,
  RTOS_ERROR_ISR,
} rtosStatus_t;

#endif  // __RTOS_STATUS_H
