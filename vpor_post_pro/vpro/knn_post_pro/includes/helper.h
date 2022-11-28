
#include <stdint.h>
#include <inttypes.h>
#include "core_class_wrapper.h"
#include "isa_intrinsic_aux_lib.h"

#ifndef helper
#define helper

void printProgress(double percent, int size);

//assumes little endian
void printBits(size_t const size, void const * const ptr);
#endif
