/**
 * Context switch implementation
 * @author Andrew Morton, 2018
 */
#include "context.h"

__asm uint32_t rtosStoreContext(void) {
  // clang-format off
  MRS   R0,PSP
  STMFD R0!,{R4-R11}
  BX    LR
  // clang-format on
}

__asm void rtosRestoreContext(uint32_t sp) {
  // clang-format off
  LDMFD R0!,{R4-R11}
  MSR   PSP,R0
  BX    LR
  // clang-format on
}
