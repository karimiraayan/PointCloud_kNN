// ########################################################
// # VPRO knn Distance finder algorythm                   #
// # Mohmmadali Karimi, IMS, Uni Hannover, 2022           #
// ########################################################
#ifndef GET_DISTANCE_KARIMI_H
#define GET_DISTANCE_KARIMI_H
#include "get_distance_config.h"
#include "global_knn.h"
#include <stdint.h>
#include "../../../isa_intrinsic_lib/isa_intrinsic_aux_lib.h"
#include "../../../isa_intrinsic_lib/core_class_wrapper.h"
class get_distance{
public:
    static void get_distance_and_save_it();
private:
    static void get_N_prim();
    static void load_N_to_lm(int cluster, int unit, int mm_offset, int lm_offset);
    static void calculate_distance(int lm_offset);
    static void load_neighbours(int cluster,int unit,int input_addr,int loop_y_element, int loop_x_element,int stride_extera ,
                                int x_addr_offset , int y_addr_offset,int p_top,int p_right  ,int p_bottom,int p_left);
    static void find_neighbours(int cluster,int unit,int x, int y,int label2D_addr);
    static int get_sgm_offset(int cluster, int unit);
    static void load_data_C_U(int lm_offset, int cycle);
    static void save_data_C_U(int lm_offset, int cycle);
};


#endif
