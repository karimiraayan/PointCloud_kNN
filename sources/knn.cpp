#include "../includes/knn.h"
#include "knn_config.h"

void knn::load_data_to_lm(int cluster, int unit, int data_addr, int lm_offset)
{
    dma_ext1D_to_loc1D(cluster,data_addr,lm_offset+LM_BASE_VU(unit),LOAD_SIZE_LM*NUM_OF_NEIGHBOURS);
}
//-------------------------------------------------------------
void knn::compute_mins(int lane_id)
{
for (int x=0;x<NUM_NEAREST_NEIGHBOURS;x++)//to find the 5 smallest numbers
{
    for (int y=0;y<LOAD_SIZE_RF;y++)//MIN function doesn't work fine without the loop
    { 
        //find the min
        __builtin_vpro_instruction_word(lane_id, NONBLOCKING, NO_CHAIN, FUNC_MIN_VECTOR, NO_FLAG_UPDATE,
                                        DST_ADDR(NUM_OF_NEIGHBOURS*LOAD_SIZE_RF+y, 0, 1),
                                        SRC1_ADDR(NUM_OF_NEIGHBOURS*y, 1, NUM_OF_NEIGHBOURS),
                                        SRC2_ADDR(0,0,0),
                                        NUM_OF_NEIGHBOURS-1,
                                        0);
    }                                    
    //make min zero and update flag
    __builtin_vpro_instruction_word(lane_id, NONBLOCKING, NO_CHAIN, FUNC_SUB, FLAG_UPDATE,
                                    DST_ADDR(0, 1, NUM_OF_NEIGHBOURS),
                                    SRC1_ADDR(NUM_OF_NEIGHBOURS*LOAD_SIZE_RF,0,1),
                                    SRC2_ADDR(0, 1, NUM_OF_NEIGHBOURS),
                                    NUM_OF_NEIGHBOURS-1,
                                    LOAD_SIZE_RF-1);
    //bring the numbers back                                
    __builtin_vpro_instruction_word(lane_id, NONBLOCKING, NO_CHAIN, FUNC_ADD, NO_FLAG_UPDATE,
                                    DST_ADDR(0, 1, NUM_OF_NEIGHBOURS),
                                    SRC1_ADDR(0, 1, NUM_OF_NEIGHBOURS),
                                    SRC2_ADDR(NUM_OF_NEIGHBOURS*LOAD_SIZE_RF,0,1),
                                    NUM_OF_NEIGHBOURS-1,
                                    LOAD_SIZE_RF-1);
    //change mins to new max
    __builtin_vpro_instruction_word(lane_id, NONBLOCKING, NO_CHAIN, FUNC_MV_ZE, NO_FLAG_UPDATE,
                                    DST_ADDR(0, 1, NUM_OF_NEIGHBOURS),
                                    SRC1_ADDR(0, 1, NUM_OF_NEIGHBOURS),
                                    SRC2_IMM(OUT_RANGER),
                                    NUM_OF_NEIGHBOURS-1,
                                    LOAD_SIZE_RF-1);   
}
}
//-------------------------------------------------------------
void knn::find_n_MIN(int lm_offset)// offset is for buffering
{
    //----------------------------------------------------------------------------//
    //-- the idea is here to make all the min_numbers the biggest same number    //
    //-- and at the end cut all the smaller number away                         //
    //-- we put the cut of area just one number below the min so it            //
    //-- it still doesn't in min algorythim but it's big enoungh to           //
    //-- not to have negative effect on our alghorythm                       //
    //----------------------------------------------------------------------//

for (int z=0; z<(LOAD_SIZE_LM/(LOAD_SIZE_RF*NUM_VECTORLANES));z++)// go through the lm
{
    //--------------lane0-----------------//load and apply CUTOFF and update flag 
    __builtin_vpro_instruction_word(LS, NONBLOCKING, IS_CHAIN, FUNC_LOAD, NO_FLAG_UPDATE,
                                    DST_ADDR(0, 0, 0),
                                    SRC1_ADDR(0, 1, NUM_OF_NEIGHBOURS),
                                    SRC2_IMM(2*z*LOAD_SIZE_RF*NUM_OF_NEIGHBOURS+lm_offset),
                                    NUM_OF_NEIGHBOURS-1,
                                    LOAD_SIZE_RF-1 );
    __builtin_vpro_instruction_word(L0, NONBLOCKING, NO_CHAIN, FUNC_SUB, FLAG_UPDATE,
                                    DST_ADDR(0, 1, NUM_OF_NEIGHBOURS),
                                    SRC1_IMM(CUTOFF),
                                    SRC2_LS,
                                    NUM_OF_NEIGHBOURS-1,
                                    LOAD_SIZE_RF-1 );
    //--------------lane0-----------------//CUTOFF the not needed data and put it to some big number
    __builtin_vpro_instruction_word(L0, NONBLOCKING, NO_CHAIN, FUNC_ADD, NO_FLAG_UPDATE,
                                    DST_ADDR(0, 1, NUM_OF_NEIGHBOURS),
                                    SRC1_ADDR(0, 1, NUM_OF_NEIGHBOURS),
                                    SRC2_IMM(CUTOFF),
                                    NUM_OF_NEIGHBOURS-1,
                                    LOAD_SIZE_RF-1 );
    __builtin_vpro_instruction_word(L0, NONBLOCKING, NO_CHAIN, FUNC_MV_PL, NO_FLAG_UPDATE,
                                    DST_ADDR(0, 1, NUM_OF_NEIGHBOURS),
                                    SRC1_ADDR(0, 1, NUM_OF_NEIGHBOURS),
                                    SRC2_IMM(OUT_RANGER-1),
                                    NUM_OF_NEIGHBOURS-1,
                                    LOAD_SIZE_RF-1 ); 
    //--------------lane1-----------------//load and apply CUTOFF and update flag
    __builtin_vpro_instruction_word(LS, NONBLOCKING, IS_CHAIN, FUNC_LOAD, NO_FLAG_UPDATE,
                                    DST_ADDR(0, 0, 0),
                                    SRC1_ADDR(0, 1, NUM_OF_NEIGHBOURS),
                                    SRC2_IMM((2*z+1)*LOAD_SIZE_RF*NUM_OF_NEIGHBOURS+lm_offset),
                                    NUM_OF_NEIGHBOURS-1,
                                    LOAD_SIZE_RF-1 );
    __builtin_vpro_instruction_word(L1, NONBLOCKING, NO_CHAIN, FUNC_SUB, FLAG_UPDATE,
                                    DST_ADDR(0, 1, NUM_OF_NEIGHBOURS),
                                    SRC1_IMM(CUTOFF),
                                    SRC2_LS,
                                    NUM_OF_NEIGHBOURS-1,
                                    LOAD_SIZE_RF-1 );   
    //--------------lane1-----------------//CUTOFF the not needed data and put it to some big number
    __builtin_vpro_instruction_word(L1, NONBLOCKING, NO_CHAIN, FUNC_ADD, NO_FLAG_UPDATE,
                                    DST_ADDR(0, 1, NUM_OF_NEIGHBOURS),
                                    SRC1_ADDR(0, 1, NUM_OF_NEIGHBOURS),
                                    SRC2_IMM(CUTOFF),
                                    NUM_OF_NEIGHBOURS-1,
                                    LOAD_SIZE_RF-1 );
    __builtin_vpro_instruction_word(L1, NONBLOCKING, NO_CHAIN, FUNC_MV_PL, NO_FLAG_UPDATE,
                                    DST_ADDR(0, 1, NUM_OF_NEIGHBOURS),
                                    SRC1_ADDR(0, 1, NUM_OF_NEIGHBOURS),
                                    SRC2_IMM(OUT_RANGER-1),
                                    NUM_OF_NEIGHBOURS-1,
                                    LOAD_SIZE_RF-1 );                        
    //----------------------------------------
    compute_mins(L0_1);
    //----------------------------------------
    __builtin_vpro_instruction_word(L0, NONBLOCKING, NO_CHAIN, FUNC_SUB, FLAG_UPDATE,//to make all the mins count it
                                    DST_ADDR(0, 1, NUM_OF_NEIGHBOURS),
                                    SRC1_IMM(OUT_RANGER-1),
                                    SRC2_ADDR(0, 1, NUM_OF_NEIGHBOURS),
                                    NUM_OF_NEIGHBOURS-1,
                                    LOAD_SIZE_RF-1 ); 
    __builtin_vpro_instruction_word(L0, NONBLOCKING, NO_CHAIN, FUNC_MV_MI, FLAG_UPDATE,
                                    DST_ADDR(0, 1, NUM_OF_NEIGHBOURS),
                                    SRC1_ADDR(0, 1, NUM_OF_NEIGHBOURS),
                                    SRC2_IMM(0),
                                    NUM_OF_NEIGHBOURS-1,
                                    LOAD_SIZE_RF-1);
    //--------------lane0-----------------//save to lm 
    __builtin_vpro_instruction_word(L0, NONBLOCKING, IS_CHAIN, FUNC_ADD, NO_FLAG_UPDATE,
                                    DST_ADDR(0, 1, NUM_OF_NEIGHBOURS),
                                    SRC1_ADDR(0, 1, NUM_OF_NEIGHBOURS),
                                    SRC2_IMM(0),
                                    NUM_OF_NEIGHBOURS-1,
                                    LOAD_SIZE_RF-1);   
    __builtin_vpro_instruction_word(LS, NONBLOCKING, NO_CHAIN, FUNC_STORE, NO_FLAG_UPDATE,
                                    DST_ADDR(0, 1, NUM_OF_NEIGHBOURS),
                                    SRC1_CHAINING(0),
                                    SRC2_IMM(2*z*LOAD_SIZE_RF*NUM_OF_NEIGHBOURS+lm_offset),
                                    NUM_OF_NEIGHBOURS-1,
                                    LOAD_SIZE_RF-1);
    //-------------------------------------------------
        __builtin_vpro_instruction_word(L1, NONBLOCKING, NO_CHAIN, FUNC_SUB, FLAG_UPDATE,//to make all the mins count it
                                    DST_ADDR(0, 1, NUM_OF_NEIGHBOURS),
                                    SRC1_IMM(OUT_RANGER-1),
                                    SRC2_ADDR(0, 1, NUM_OF_NEIGHBOURS),
                                    NUM_OF_NEIGHBOURS-1,
                                    LOAD_SIZE_RF-1 ); 
    __builtin_vpro_instruction_word(L1, NONBLOCKING, NO_CHAIN, FUNC_MV_MI, FLAG_UPDATE,
                                    DST_ADDR(0, 1, NUM_OF_NEIGHBOURS),
                                    SRC1_ADDR(0, 1, NUM_OF_NEIGHBOURS),
                                    SRC2_IMM(0),
                                    NUM_OF_NEIGHBOURS-1,
                                    LOAD_SIZE_RF-1);
    //--------------lane1-----------------//save to lm  
    __builtin_vpro_instruction_word(L1, NONBLOCKING, IS_CHAIN, FUNC_ADD, NO_FLAG_UPDATE,
                                    DST_ADDR(0, 1, NUM_OF_NEIGHBOURS),
                                    SRC1_ADDR(0, 1, NUM_OF_NEIGHBOURS),
                                    SRC2_IMM(0),
                                    NUM_OF_NEIGHBOURS-1,
                                    LOAD_SIZE_RF-1);  
    __builtin_vpro_instruction_word(LS, NONBLOCKING, NO_CHAIN, FUNC_STORE, NO_FLAG_UPDATE,
                                    DST_ADDR(0, 1, NUM_OF_NEIGHBOURS),
                                    SRC1_CHAINING(1),
                                    SRC2_IMM((2*z+1)*LOAD_SIZE_RF*NUM_OF_NEIGHBOURS+lm_offset),
                                    NUM_OF_NEIGHBOURS-1,
                                    LOAD_SIZE_RF-1);  
}
}
//------------------------------------------------------------
void knn::load_data_C_U(int lm_offset, int cycle)
{
int sgm_num=cycle*SGM_BATCH_SIZE;
for (int unit=0;unit<NUM_VU_PER_CLUSTER;unit++)//load data 
{    
    for (int cluster=0;cluster<NUM_CLUSTERS;cluster++)
    {
        int addr_offset=BYTES_PER_DATA*(sgm_num*NUM_OF_NEIGHBOURS);
        load_data_to_lm(cluster,unit,DISTANCE_ADDR+addr_offset,lm_offset);
        sgm_num=sgm_num+LOAD_SIZE_LM;                   
    } 
}
}
//------------------------------------------------------------
void knn::save_data_C_U(int lm_offset, int cycle)
{
int sgm_num=cycle*SGM_BATCH_SIZE;
for (int unit=0;unit<NUM_VU_PER_CLUSTER;unit++)//store
{    
    for (int cluster=0;cluster<NUM_CLUSTERS;cluster++)
    {
        dma_loc1D_to_ext1D( cluster,
                    KNN_SORTED_ADDR+BYTES_PER_DATA*NUM_OF_NEIGHBOURS*sgm_num,
                    lm_offset+LM_BASE_VU(unit),
                    NUM_OF_NEIGHBOURS*LOAD_SIZE_LM);
        sgm_num=sgm_num+LOAD_SIZE_LM;
    }
}
}       
//-----------------------------------------------------------------------get the valued vote
void knn::get_the_valued_vote()
{
    int buffering=0;
    int cycle = 0;
    //--------------------------peeling
    load_data_C_U(buffering*BUFFERING_OFFSET, cycle);
    dma_wait_to_finish(0xfffff);
    find_n_MIN(buffering*BUFFERING_OFFSET);
    vpro_wait_busy(0xffff,0xffff);
    cycle++;
    load_data_C_U((!buffering)*BUFFERING_OFFSET, cycle);
    //----------------------------------
    int num_cycles = TOTAL_NUM_OF_POINTS/(NUM_CLUSTERS*NUM_VU_PER_CLUSTER*LOAD_SIZE_LM);
    for (;cycle < num_cycles+1; cycle++)// iterating more to ensure all data are computed
    {
        save_data_C_U(buffering*BUFFERING_OFFSET, cycle - 1);
        load_data_C_U(buffering*BUFFERING_OFFSET, cycle + 1); 
        dma_wait_to_finish(0xfffff);
        buffering=!buffering;
        find_n_MIN(buffering*BUFFERING_OFFSET);
        vpro_wait_busy(0xffff,0xffff);
    }
    save_data_C_U(buffering*BUFFERING_OFFSET, cycle - 1); // last peeling  
    dma_wait_to_finish(0xffff);   
}


