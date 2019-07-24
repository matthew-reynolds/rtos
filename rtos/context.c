/**
 * Context switch implementation
 * @author Andrew Morton, 2018
 */
#include "context.h"

/**
 * Store the current execution context on the stack
 *
 * @return the final value of the stack pointer
 */
__asm uint32_t rtosStoreContext(void) {
  // clang-format off
  MRS   R0,PSP        // Move PSP coprocessor register to R0
  STMFD R0!,{R4-R11}  // Push the R4 to R11 registers onto the processor stack (PSP)
  BX    LR            // Return the value of the stack pointer after pushing R4-R11
  // clang-format on
}

/**
 * Restore the specified execution context from the stack
 *
 * @param sp the stack pointer from which to pop the context
 */
__asm void rtosRestoreContext(uint32_t sp) {
  // clang-format off
  LDMFD R0!,{R4-R11} // Pop the R4-R11 registers off of the stack at the specified stack pointer
  MSR   PSP,R0       // Move the value of the stack pointer after popping R4-R11 into the processor stack (PSP)
  BX    LR           // Return
  // clang-format on
}

__asm void popR4(void) {
  // clang-format off
  POP {R4}
  BX    LR
  // clang-format on
}

__asm void pushR4(void) {
  // clang-format off
  PUSH {R4}
  BX    LR
  // clang-format on
}

__asm void popR4R6(void) {
  // clang-format off
  POP {R4-R6}
  BX    LR
  // clang-format on
}

__asm void pushR4R6(void) {
  // clang-format off
  PUSH {R4-R6}
  BX    LR
  // clang-format on
}

__asm void popR4R10(void) {
  // clang-format off
  POP {R4-R10}
  BX    LR
  // clang-format on
}

__asm void pushR4R10(void) {
  // clang-format off
  PUSH {R4-R10}
  BX    LR
  // clang-format on
}
