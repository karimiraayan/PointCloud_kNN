//
// Created by gesper on 3/3/21.
//

#include "defines.h"

/**
 * Test Data Variables
 */
alignas(16) volatile int16_t test_array_1[NUM_TEST_ENTRIES] __attribute__ ((section (".vpro"))); // __attribute__((align_value(128)));
alignas(16) volatile int16_t test_array_2[NUM_TEST_ENTRIES] __attribute__ ((section (".vpro"))); // __attribute__((align_value(128)));
alignas(16) volatile int16_t result_array[NUM_TEST_ENTRIES] __attribute__ ((section (".vpro"))); // __attribute__((align_value(128)));

//alignas(16) volatile int16_t result_array_zeros[NUM_TEST_ENTRIES] __attribute__ ((section (".vpro"))); // __attribute__((align_value(128)));
//alignas(16) volatile int16_t result_array_dead[NUM_TEST_ENTRIES] __attribute__ ((section (".vpro"))); // __attribute__((align_value(128)));
//alignas(16) volatile int16_t result_array_large[1024*1024] __attribute__ ((section (".vpro"))); // __attribute__((align_value(128)));

alignas(16) volatile uint64_t SIGNAL_RUNNING __attribute__ ((section (".host")));
alignas(16) volatile uint64_t SIGNAL_START __attribute__ ((section (".host")));
alignas(16) volatile uint64_t SIGNAL_RETURN __attribute__ ((section (".host")));