/**
 * Standard return status codes (analagous to errno)
 * @author Matt Reynolds
 * @author Dawson Hemphill
 */
#ifndef __RTOS_STATUS_H
#define __RTOS_STATUS_H

/// A special timeout value that informs the RTOS to never timeout.
#define RTOS_WAIT_FOREVER 0xFFFFFFFFU

/// Standard return status codes
typedef enum {
  RTOS_OK,               ///< Operation completed successfully
  RTOS_ERROR,            ///< Unspecified runtime error
  RTOS_ERROR_TIMEOUT,    ///< Operation not completed within timeout period
  RTOS_ERROR_RESOURCE,   ///< Resource not available
  RTOS_ERROR_PARAMETER,  ///< Parameter error
} rtosStatus_t;

#endif  // __RTOS_STATUS_H
