//
// Created by gesper on 3/3/21.
//
#include "application.h"
#include "../../isa_intrinsic_lib/isa_intrinsic_aux_lib.h"
#include "../../isa_intrinsic_lib/core_class_wrapper.h"

void run() {
    while (SIGNAL_RUNNING != 0) {
#ifdef SIMULATION
        int32_t *reference_result;
#endif
        int32_t select = 0;
        while (SIGNAL_START == 0) {         // wait for a valid test start signal from host app
            if (SIGNAL_RUNNING != 1) {      // exit if running is deactivated
                aux_print_debugfifo(SIGNAL_DONE | 0xdead);
                return;
            }
#ifdef SIMULATION
            SIGNAL_START = 1; // no host application, get input / control here // TODO: fake host select
#endif
        }
        select = SIGNAL_START;  // read set value
        SIGNAL_RETURN = 0;
        SIGNAL_START = 0;       // reset selection value

#ifdef SIMULATION
        std::random_device rd;     // only used once to initialise (seed) engine
        std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
        std::uniform_int_distribution<int> uni(0, 0xffff); // guaranteed unbiased
        for (int i = 0; i < NUM_TEST_ENTRIES; i++) {
            test_array_1[i] = uint16_t(uni(rng));
            test_array_2[i] = uint16_t(uni(rng));
        }
        reference_result = new int32_t[NUM_TEST_ENTRIES](0);    // TODO: create reference?!
        sim_printf("Selection: %i\n", select);
#else
        aux_print_debugfifo(uint32_t(select));  // print test back to host
#endif


        /**
         *  Execute whatever (maybe selected by host)
         */
        for (int i = 0; i < NUM_TEST_ENTRIES; i++) {
            result_array[i] = test_array_1[i] + test_array_2[i];
        }


#ifdef SIMULATION
        /**
         * Check result correctnes (ISS)
         */
#else
        SIGNAL_RETURN = 1;
#endif
    }
}