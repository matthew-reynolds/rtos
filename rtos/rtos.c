/**
 * Main RTOS implementation
 * @author Matt Reynolds
 * @author Dawson Hemphill
 */

#include <stdbool.h>
#include <stdlib.h>

#include <LPC17xx.h>

#include "context.h"
#include "rtos.h"

uint32_t rtos_ticks   = 0;
uint32_t systick_freq = 1000;  // Default systick frequency = 1000Hz (1ms)

/**
 * SysTick ISR
 *
 * Increment the rtos_tick count, and invoke the scheduler.
 */
void SysTick_Handler(void) {
  popR4();

  // Increment tick count
  rtos_ticks++;

  // If the RTOS & its scheduler are running, run invoke the scheduler
  if (rtos_running_task != NULL) {
    rtosInvokeScheduler();
  }

  pushR4();
}

/**
 * SVC (Supervisor Call) ISR
 *
 * Used to start the first task.
 */
void SVC_Handler(void) {
  popR4();
  rtosRestoreContext(rtos_running_task->stack_pointer);
  pushR4();
}

/**
 * PendSV ISR
 *
 * Used to perform a context switch.
 */
void PendSV_Handler(void) {
  popR4();
  rtosPerformContextSwitch();
  pushR4();
}

/**
 * Default idle task
 *
 * Put the system into low power mode until another task is ready.
 */
void rtosIdleTask(void* arg) {
  while (true) {
    __WFE();
  }
}

/**
 * Get the current system tick count
 */
uint32_t rtosGetSysTickCount(void) {
  return rtos_ticks;
}

/**
 * Get the system tick frequency, in Hz
 */
uint32_t rtosGetSysTickFreq(void) {
  return systick_freq;
}

/**
 * Set the system tick frequency, in Hz
 */
void rtosSetSysTickFreq(uint32_t freq) {
  systick_freq = freq;
  SysTick_Config(SystemCoreClock / systick_freq);
}

/**
 * Initialize the RTOS
 */
void rtosInitialize(void) {

  // Ensure the systick frequency is set
  rtosSetSysTickFreq(systick_freq);

  // Initialize all the task control blocks
  rtosTaskInitAll();

  // Create the idle task
  rtosTaskNew(rtosIdleTask, NULL, RTOS_PRIORITY_IDLE, NULL);
}

/**
 * Start the RTOS scheduler
 */
void rtosBegin(void) {

  // Prepare the first task
  rtos_running_task = rtosGetReadyTask(rtosGetHighestReadyPriority());
  if (rtos_running_task == NULL) {
    return;
  }
  rtosPopTaskListHead(rtosGetReadyTaskQueue(rtos_running_task->priority));
  rtos_running_task->state = RTOS_TASK_RUNNING;

  // Set the PSP to the MSP
  // This is temporary (PSP will be overwritten once the task starts) and useless (The stack is not used between
  // switching to PSP and starting the task). Setting the PSP here is just good practice so the stack pointer is
  // always valid.
  __set_PSP(__get_MSP());

  // Switch thread mode stack from MSP to PSP
  CONTROL_Type ctrl;
  ctrl.w       = __get_CONTROL();
  ctrl.b.SPSEL = 1;
  __set_CONTROL(ctrl.w);

  // Reset the MSP back to the start of the stack
  __set_MSP(BASE_STACK_PTR);

  // Call SVC to start the first task
  asm("SVC 0");
}
