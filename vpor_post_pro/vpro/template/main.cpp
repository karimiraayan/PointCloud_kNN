#include <stdint.h>

// Intrinsic auxiliary library
#include "../../isa_intrinsic_lib/isa_intrinsic_aux_lib.h"
#include "../../isa_intrinsic_lib/core_class_wrapper.h"

#ifndef NUM_CLUSTERS
#define NUM_CLUSTERS  1 // number of clusters
#endif
#ifndef NUM_VU_PER_CLUSTER
#define NUM_VU_PER_CLUSTER  1 // vector units per cluster
#endif
#define NUM_VECTORLANES  2 // per unit

#include "defines.h"
#include "application.h"

void initialize_environment(int argc, char *argv[]);

int close_environment();

//----------------------------------------------------------------------------------
//----------------------------------Main--------------------------------------------
//----------------------------------------------------------------------------------
int main(int argc, char *argv[]) {
    initialize_environment(argc, argv);
    aux_flush_dcache();
    aux_clr_dcache();
    aux_clr_icache();

    // execute application.cpp
    run();

    return close_environment();
}

void initialize_environment(int argc, char *argv[]) {
    sim_init(main, argc, argv, 1024 * 1024 * 512, 8192, 1024, NUM_CLUSTERS, NUM_VU_PER_CLUSTER, NUM_VECTORLANES, 10);

    HW.LANES = NUM_VECTORLANES;
    HW.CLUSTERS = NUM_CLUSTERS;
    HW.UNITS = NUM_VU_PER_CLUSTER;
    HW.MM_SIZE = 1024 * 1024 * 512;
    HW.LM_SIZE = 8192;
    HW.RF_SIZE = 1024;

    sim_printf("VPRO Template Programm");
    sim_printf("Clusters: %i, units: %i, lanes: %i", NUM_CLUSTERS, NUM_VU_PER_CLUSTER, NUM_VECTORLANES);

    SIGNAL_RUNNING = 1;
    SIGNAL_START = 0;
    SIGNAL_RETURN = 0;

//    // reset result array in mm to 0
//    for (int i = 0; i < NUM_TEST_ENTRIES; ++i){
//        result_array_zeros[i] = 0;
//        result_array_dead[i] = 0xdead;
//    }
    vpro_set_idmask(0xFFFFFFFF); // broadcast to all

    // 1. send address of result array
    aux_print_debugfifo(uint32_t(intptr_t(result_array)));
    // 2. send address of input 1 array
    aux_print_debugfifo(uint32_t(intptr_t(test_array_1)));
    // 3. send address of input 2 array
    aux_print_debugfifo(uint32_t(intptr_t(test_array_2)));
    // 4. send address of start flag
    aux_print_debugfifo(uint32_t(intptr_t(&SIGNAL_START)));
    // 5. send address of running flag
    aux_print_debugfifo(uint32_t(intptr_t(&SIGNAL_RUNNING)));
    // 6. send address of return flag
    aux_print_debugfifo(uint32_t(intptr_t(&SIGNAL_RETURN)));
}

int close_environment() {
    sim_stop();
    return 0;
}