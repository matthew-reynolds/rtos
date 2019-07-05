# rtos

MTE 241 - Final project RTOS

## Simulation

Not all peripheral devices exist in the simulator. Therefore, in order to run the code in simulation, some modifications must be made to the environment and code.
 - Undef `CLOCK_SETUP` (or define to 0) in `system_LCP17xx.c`
 - Undef `__RTGT_UART`
