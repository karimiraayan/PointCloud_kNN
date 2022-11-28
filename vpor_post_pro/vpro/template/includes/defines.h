//
// Created by gesper on 10.09.19.
//

#ifndef CNN_YOLO_LITE_DEFINES_H
#define CNN_YOLO_LITE_DEFINES_H

#include "inttypes.h"

constexpr int NUM_TEST_ENTRIES = 64;

constexpr int32_t SIGNAL_DONE = 0xdead0000;


/**
 * Test Data Variables
 */
extern volatile int16_t test_array_1[NUM_TEST_ENTRIES];
extern volatile int16_t test_array_2[NUM_TEST_ENTRIES];
extern volatile int16_t result_array[NUM_TEST_ENTRIES];

// Start/Finish variables
extern volatile uint64_t SIGNAL_RUNNING;
extern volatile uint64_t SIGNAL_START;
extern volatile uint64_t SIGNAL_RETURN;


#endif //CNN_YOLO_LITE_DEFINES_H
