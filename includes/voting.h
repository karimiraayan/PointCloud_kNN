// ########################################################
// # VPRO voting algorythm                                #
// # Mohmmadali Karimi, IMS, Uni Hannover, 2022           #
// ########################################################

#ifndef VOTING_KARIMI_H
#define VOTING_KARIMI_H
#include <stdint.h>
#include "global_knn.h"

#include "../../../isa_intrinsic_lib/isa_intrinsic_aux_lib.h"
#include "../../../isa_intrinsic_lib/core_class_wrapper.h"
class voting
{
public:
        static void find_the_most_votet_label(int output_addr);
private:
        static void get_L_prim();
        static void get_L(int L_prim_addr,int points_2d_mapping_addr, int L_output_addr);
        static void calculate_the_votes(int lane_id);
        static void load_neighbours(int cluster,int unit,int input_addr,int loop_y_element, int loop_x_element, int stride_extera,
                                    int x_addr_offset , int y_addr_offset,int p_top,int p_right  ,int p_bottom,int p_left);
        static void load_L_to_lm(int cluster, int unit, int mm_offset, int lm_offset);
        static void load_data_to_rf(int lm_addr, int offset_rf, int lane_id,int flag,int blocking );
        static void compute_knn_labels(int lane_id);
        static void save_to_lm(int lm_addr ,int lane_id,int lane_id_wothout_L);
        static void get_vot_one();
        static void find_neighbours(int cluster,int unit,int x, int y,int label2D_addr);
        static void load_data_C_U(int lm_offset, int cycle);
        static void save_data_C_U(int lm_offset, int cycle);
        static void compute_the_votes(int lm_offset);

};


#endif