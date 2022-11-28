#include <stdint.h>
#include <math.h>

// Intrinsic auxiliary library
#include "../../isa_intrinsic_lib/isa_intrinsic_aux_lib.h"
#include "../../isa_intrinsic_lib/core_class_wrapper.h"

#define MAIN_MEMORY_SIZE   1024*1024*1024 // in bytes -> 512 MB // [0 ~ 1073741824]
#define RF_SIZE  1024 // in entries, FIXED! each 24-bit
#define LM_SIZE  8192 // number of entries, each 16-bit wide

#ifndef NUM_CLUSTERS  
#define NUM_CLUSTERS  1 // number of clusters
#endif
#ifndef NUM_VU_PER_CLUSTER
#define NUM_VU_PER_CLUSTER  1 // vector units per cluster
#endif
#include <stdio.h>
#include "get_distance.h"
#include "voting.h"
#include "knn.h"
//----------------------------------------------------------------------------------
//----------------------------------Main--------------------------------------------
//----------------------------------------------------------------------------------
int main(int argc, char *argv[]) {
    // initialize simulator environment
    sim_init(main, argc, argv, MAIN_MEMORY_SIZE, LM_SIZE, RF_SIZE, NUM_CLUSTERS, NUM_VU_PER_CLUSTER, NUM_VECTORLANES);
    sim_printf("Clusters: %i, units: %i, lanes: %i", NUM_CLUSTERS, NUM_VU_PER_CLUSTER, NUM_VECTORLANES);
    // execute each on ALL vector units
    vpro_set_idmask(0xFFFF);   
    // ************************** YOUR CODE **************************** //
    get_distance::get_distance_and_save_it();
    knn::get_the_valued_vote();
    voting::find_the_most_votet_label(L_CON_ADDR);
    sim_stop();
    return 0;
}