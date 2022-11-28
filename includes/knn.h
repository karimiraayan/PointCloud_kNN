// ########################################################
// # VPRO knn algorythm                                   #
// # Mohmmadali Karimi, IMS, Uni Hannover, 2022           #
// ########################################################
#ifndef SORTING_DATA_KARIMI_H
#define SORTING_DATA_KARIMI_H
#include <stdint.h>
#include "global_knn.h"
#include "../../../isa_intrinsic_lib/isa_intrinsic_aux_lib.h"
#include "../../../isa_intrinsic_lib/core_class_wrapper.h"
class knn
{
public:
        static void get_the_valued_vote();
private:
        static void load_data_to_lm(int cluster, int unit, int data_addr, int lm_offset);
        static void find_n_MIN(int lm_offset);     
        static void compute_mins(int lane_id);
        static void load_data_C_U(int lm_offset, int cycle);
        static void save_data_C_U(int lm_offset, int cycle);

};


#endif
