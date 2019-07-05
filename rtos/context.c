/**
 * Context switch implementation
 * @author Andrew Morton, 2018
 */
#include "context.h"

__asm uint32_t rtosStoreContext(void) {
  // clang-format off
  MRS   R0,PSP        // Move PSP coprocessor register to R0
  STMFD R0!,{R4-R11}  // Push the R4 to R11 registers onto the processor stack (PSP)
  BX    LR            // Return the value of the stack pointer after pushing R4-R11
  // clang-format on
}

__asm void rtosRestoreContext(uint32_t sp) {
  // clang-format off
  LDMFD R0!,{R4-R11} // Pop the R4-R11 registers off of the stack at the specified stack pointer
  MSR   PSP,R0       // Move the value of the stack pointer after popping R4-R11 into the processor stack (PSP)
  BX    LR           // Return
  // clang-format on
}
