#include "../includes/voting.h"
#include "voting_config.h"

//find the 25 neighoubors
void voting::load_neighbours(int cluster,int unit,int input_addr,int loop_y_element, int loop_x_element,int stride_extera ,
                                    int x_addr_offset , int y_addr_offset,int p_top,int p_right  ,int p_bottom,int p_left)
{
bool pad_flags[4];
pad_flags[CommandDMA::PAD::TOP] = true;
pad_flags[CommandDMA::PAD::RIGHT] = true;
pad_flags[CommandDMA::PAD::BOTTOM] = true;
pad_flags[CommandDMA::PAD::LEFT] = true;
dma_set_pad_widths(p_top,p_right,p_bottom,p_left);
dma_set_pad_value(0);
int addr_offset=BYTES_PER_DATA*(loop_y_element*WIDTH+loop_x_element-x_addr_offset-y_addr_offset*WIDTH);
dma_ext2D_to_loc1D( cluster,
                    input_addr+addr_offset,
                    LM_BASE_VU(unit),
                    WIDTH+stride_extera-SEARCH_SIZE+1,
                    SEARCH_SIZE,SEARCH_SIZE,pad_flags);
//padding needs dma
dma_wait_to_finish(0xFFFF);

}

//------------------------------------------------------------
void voting::save_data_C_U(int lm_offset, int cycle)
{
int sgm_num=cycle*SGM_BATCH_SIZE;
for (int unit=0;unit<NUM_VU_PER_CLUSTER;unit++)
{    
    for (int cluster=0;cluster<NUM_CLUSTERS;cluster++)
    {
        int mm_addr=L_CON_ADDR+BYTES_PER_DATA*sgm_num;
        dma_loc1D_to_ext1D( cluster,
                            mm_addr,
                            LM_BASE_VU(unit)+lm_offset,
                            LOAD_SIZE_LM);
        sgm_num=sgm_num+LOAD_SIZE_LM;  
    }
}
}
//------------------------------------------------------------
void voting::find_neighbours(int cluster,int unit,int x, int y,int label2D_addr)
{
    if((x==0)&(y==0))//left upper corner 
        load_neighbours(cluster, unit, label2D_addr,y, x,2 ,0 , 0, 2, 0, 0 , 2);
    else if((x==1)&(y==0))//left upper corner 
        load_neighbours(cluster, unit, label2D_addr,y, x,1 ,1 , 0, 2, 0, 0 , 1);   
    else if ((x==0)&(y==1))//left upper corner 
        load_neighbours(cluster, unit, label2D_addr,y, x,2 ,0 , 1, 1, 0, 0 , 2);                          
    else if ((x==1)&(y==1))//left upper corner 
        load_neighbours(cluster, unit, label2D_addr,y, x,1 , 1, 1, 1, 0, 0 , 1); 
    if((x==WIDTH-1)&(y==0))//right upper corner
        load_neighbours(cluster, unit, label2D_addr,y, x,2 , 2, 0, 2, 2, 0 , 0);
    else if((x==WIDTH-2)&(y==0))//right upper corner
        load_neighbours(cluster, unit, label2D_addr,y, x,1 , 2, 0, 2, 1, 0 , 0);
    else if ((x==WIDTH-1)&(y==1))//right upper corner
        load_neighbours(cluster, unit, label2D_addr, y, x, 2, 2, 1, 1, 2, 0 , 0);                      
    else if ((x==WIDTH-2)&(y==1))//right upper corner
        load_neighbours(cluster, unit, label2D_addr, y, x, 1, 2, 1, 1, 1, 0 , 0);                      
    else if((x==WIDTH-1)&(y==HEIGHT-1))//right down corner
        load_neighbours(cluster, unit, label2D_addr, y, x, 2, 2, 2, 0, 2, 2 , 0);
    else if ((x==WIDTH-1)&(y==HEIGHT-2))//right down corner
        load_neighbours(cluster, unit, label2D_addr, y, x, 2, 2, 2, 0, 2, 1 , 0);                       
    else if((x==WIDTH-2)&(y==HEIGHT-2))//right down corner
        load_neighbours(cluster, unit, label2D_addr, y, x, 1, 2, 2, 0, 1, 1 , 0);
    else if ((x==WIDTH-2)&(y==HEIGHT-1))//right down corner
        load_neighbours(cluster, unit, label2D_addr, y, x, 1, 2, 2, 0, 1, 2 , 0);                      
    else if((x==0)&(y==HEIGHT-1))//down left cornenr
        load_neighbours(cluster, unit, label2D_addr, y, x, 2, 0, 2, 0, 2, 2 , 0);
    else if ((x==0)&(y==HEIGHT-2))//down left cornenr
        load_neighbours(cluster, unit, label2D_addr, y, x, 2, 0, 2, 0, 0, 1 , 2);
    else if((x==1)&(y==HEIGHT-2))//down left cornenr
        load_neighbours(cluster, unit, label2D_addr, y, x, 1, 1, 2, 0, 0, 1 , 1);
    else if ((x==1)&(y==HEIGHT-1))//down left cornenr
        load_neighbours(cluster, unit, label2D_addr, y, x, 1, 1, 2, 0, 0, 2 , 1);                                  
    else if((x>1)&(x<(WIDTH-2))&(y==0))//middle upper
        load_neighbours(cluster, unit, label2D_addr, y, x, 0, 2, 0, 2, 0, 0 , 0);
    else if((x>1)&(x<(WIDTH-2))&(y==1))//middle upper
        load_neighbours(cluster, unit, label2D_addr, y, x, 0, 2, 1, 1, 0, 0 , 0);
    else if((x>1)&(x<(WIDTH-2))&(y==HEIGHT-1))//middle down
        load_neighbours(cluster, unit, label2D_addr, y, x, 0, 2, 2, 0, 0, 2, 0);
    else if((x>1)&(x<(WIDTH-2))&(y==(HEIGHT-2)))//middle down
        load_neighbours(cluster, unit, label2D_addr, y, x, 0, 2, 2, 0, 0, 1, 0); 
    else if((y>1)&(y<(HEIGHT-2))&(x==0))//middle left
        load_neighbours(cluster, unit, label2D_addr, y, x, 2, 0, 2, 0, 0, 0, 2);
    else if((y>1)&(y<(HEIGHT-2))&(x==1))//middle left
        load_neighbours(cluster, unit, label2D_addr, y, x, 1, 1, 2, 0, 0, 0, 1);
    else if((y>1)&(y<(HEIGHT-2))&(x==WIDTH-1))//middle right
        load_neighbours(cluster, unit, label2D_addr, y, x, 2, 2, 2, 0, 2, 0, 0);
    else if((y>1)&(y<(HEIGHT-2))&(x==WIDTH-2))//middle right
        load_neighbours(cluster, unit, label2D_addr, y, x, 1, 2, 2, 0, 1, 0, 0);                                                                                                                 
    else if((y>1)&(y<(HEIGHT-2))&(x>1)&(x<(WIDTH-2)))
    {
        // load neighbours without padding
        dma_ext2D_to_loc1D(cluster,label2D_addr+BYTES_PER_DATA*(y*WIDTH+x-2*WIDTH-2),
                            0x0+LM_BASE_VU(unit),WIDTH-SEARCH_SIZE+1,SEARCH_SIZE,SEARCH_SIZE);   
    }  
}

//---------------------------------------------------------------------------------
void voting::get_L_prim()
{
int sgm_num=0;
    for (int x=0;x<((WIDTH*HEIGHT/(NUM_CLUSTERS*NUM_VU_PER_CLUSTER))+1);x++)
    {
        for (int unit=0;unit<NUM_VU_PER_CLUSTER;unit++)// load data to lm
        {    
            for (int cluster=0;cluster<NUM_CLUSTERS;cluster++)
            {               
                int x= (sgm_num%WIDTH);// locater the coordinate of the pixel W
                int y= int(sgm_num/WIDTH);// locater the coordinate of the pixel H
                int mm_addr=L_PRIM_ADDR+BYTES_PER_DATA*NUM_OF_NEIGHBOURS*(sgm_num);

                find_neighbours(cluster, unit,x,y,LABELS2D_ADDR); 
                dma_loc1D_to_ext1D( cluster,
                                    mm_addr,
                                    LM_BASE_VU(unit),
                                    NUM_OF_NEIGHBOURS); 
                sgm_num++;  
            }
        }      
    }        
}
//-------------------------------------------------------------------------------------------------------------------------------------
void voting::load_L_to_lm(int cluster, int unit, int mm_offset, int lm_offset)// extending the N_prim to bigger point cloud number of points
{   
    //using ofset let us decide to chooce a portion of data
    for (int x=mm_offset ; x<LOAD_SIZE_LM+mm_offset ;x++)
    {
        int mm_addr=((*get_main_memory(POINTS_2D_MAPPING_ADDR+BYTES_PER_DATA*x, 1))*NUM_OF_NEIGHBOURS)*BYTES_PER_DATA+L_PRIM_ADDR;
        int lm_addr=LM_BASE_VU(unit)+(x-mm_offset)*NUM_OF_NEIGHBOURS+lm_offset;
        dma_ext1D_to_loc1D( cluster,
                            mm_addr,
                            lm_addr,
                            NUM_OF_NEIGHBOURS);
    }
}
//----------------------------------------------------------------
// here to parts are loaded 
// first knn sorted list
// second L list
void voting::load_data_C_U(int lm_offset, int cycle)
{
int sgm_num=cycle*SGM_BATCH_SIZE;
for (int unit=0;unit<NUM_VU_PER_CLUSTER;unit++)//load data 
{    
    for (int cluster=0;cluster<NUM_CLUSTERS;cluster++)
    {
        load_L_to_lm(cluster, unit, sgm_num, lm_offset);
        sgm_num=sgm_num+LOAD_SIZE_LM;    
              
    }            
}
sgm_num=cycle*SGM_BATCH_SIZE;
for (int unit=0;unit<NUM_VU_PER_CLUSTER;unit++)//load data 
{    
    for (int cluster=0;cluster<NUM_CLUSTERS;cluster++)
    {
        int mm_addr=KNN_SORTED_ADDR+BYTES_PER_DATA*NUM_OF_NEIGHBOURS*sgm_num;
        int lm_addr=LM_BASE_VU(unit)+LOAD_SIZE_LM*NUM_OF_NEIGHBOURS+lm_offset;
        //load knn to the second part of memmory
        dma_ext1D_to_loc1D( cluster,
                            mm_addr,
                            lm_addr,
                            NUM_OF_NEIGHBOURS*LOAD_SIZE_LM); 
        sgm_num=sgm_num+LOAD_SIZE_LM;                   
    }
}
}
//-------------------------------------------------
void voting::load_data_to_rf(int lm_addr, int offset_rf, int lane_id,int flag,int blocking )
{
    __builtin_vpro_instruction_word(LS, NONBLOCKING, IS_CHAIN, FUNC_LOAD, NO_FLAG_UPDATE,
                                    DST_ADDR(0, 0, 0),
                                    SRC1_ADDR(0, 1, NUM_OF_NEIGHBOURS),
                                    SRC2_IMM(lm_addr),
                                    NUM_OF_NEIGHBOURS-1,
                                    LOAD_SIZE_RF-1 );
    __builtin_vpro_instruction_word(lane_id, blocking, NO_CHAIN, FUNC_ADD, flag,
                                    DST_ADDR(offset_rf, 1, NUM_OF_NEIGHBOURS),
                                    SRC1_LS,
                                    SRC2_IMM(0),
                                    NUM_OF_NEIGHBOURS-1,
                                    LOAD_SIZE_RF-1);
}
//---------------------------------------------
//here we merge the both knn and labels together to find the right labels
void voting::compute_knn_labels(int lane_id)
{                         
    __builtin_vpro_instruction_word(lane_id, NONBLOCKING, NO_CHAIN, FUNC_ADD, NO_FLAG_UPDATE,//add one to count yeros in too
                                    DST_ADDR(0, 1, NUM_OF_NEIGHBOURS),
                                    SRC1_ADDR(0, 1, NUM_OF_NEIGHBOURS),
                                    SRC2_IMM(1),
                                    NUM_OF_NEIGHBOURS-1,
                                    LOAD_SIZE_RF-1);   
    __builtin_vpro_instruction_word(lane_id, NONBLOCKING, NO_CHAIN, FUNC_MV_ZE, NO_FLAG_UPDATE,//move zeros
                                    DST_ADDR(0, 1, NUM_OF_NEIGHBOURS),
                                    SRC1_ADDR(NUM_OF_NEIGHBOURS*LOAD_SIZE_RF, 1, NUM_OF_NEIGHBOURS),
                                    SRC2_IMM(0),
                                    NUM_OF_NEIGHBOURS-1,
                                    LOAD_SIZE_RF-1);
}
//-------------------------------------------------------
void voting::save_to_lm(int lm_addr ,int lane_id,int lane_id_wothout_L)
{
    __builtin_vpro_instruction_word(lane_id, NONBLOCKING, IS_CHAIN, FUNC_ADD, NO_FLAG_UPDATE,
                                    DST_ADDR(0, 1, NUM_OF_NEIGHBOURS),
                                    SRC1_ADDR(0, 1, NUM_OF_NEIGHBOURS),
                                    SRC2_IMM(0),
                                    LOAD_SIZE_RF-1,
                                    1-1);
    __builtin_vpro_instruction_word(LS, NONBLOCKING, NO_CHAIN, FUNC_STORE, NO_FLAG_UPDATE,
                                    DST_ADDR(0, 1, NUM_OF_NEIGHBOURS),
                                    SRC1_CHAINING(lane_id_wothout_L),
                                    SRC2_IMM(lm_addr),
                                    LOAD_SIZE_RF-1,
                                    1-1);
}    

//---------------------------------------

void voting::calculate_the_votes(int lane_id)
{     
for (int x=1; x<NUM_CLASSES+1;x++)//we iterat here with the step of one to find the same numbers for each class
{   
    __builtin_vpro_instruction_word(lane_id, NONBLOCKING, NO_CHAIN, FUNC_SUB, FLAG_UPDATE,
                                    DST_ADDR(0, 1, NUM_OF_NEIGHBOURS), 
                                    SRC1_IMM(1),
                                    SRC2_ADDR(0, 1, NUM_OF_NEIGHBOURS) , NUM_OF_NEIGHBOURS-1, LOAD_SIZE_RF-1);
    __builtin_vpro_instruction_word(lane_id, NONBLOCKING, NO_CHAIN, FUNC_ADD, NO_FLAG_UPDATE,// to clean the memory
                                    DST_ADDR(NUM_OF_NEIGHBOURS*LOAD_SIZE_RF, 1, NUM_OF_NEIGHBOURS),
                                    SRC1_IMM(0), SRC2_IMM(0), 
                                    NUM_OF_NEIGHBOURS-1, LOAD_SIZE_RF-1);
    __builtin_vpro_instruction_word(lane_id, NONBLOCKING, NO_CHAIN, FUNC_MV_ZE, NO_FLAG_UPDATE,
                                    DST_ADDR(NUM_OF_NEIGHBOURS*LOAD_SIZE_RF, 1, NUM_OF_NEIGHBOURS),
                                    SRC1_ADDR(0, 1, NUM_OF_NEIGHBOURS), SRC2_IMM(1), 
                                    NUM_OF_NEIGHBOURS-1, LOAD_SIZE_RF-1);
    for (int z=0;z<LOAD_SIZE_RF; z++) //everytime we should reset the mac and go through the new vector and find the sum of the voted class
    {
        __builtin_vpro_instruction_word(lane_id, NONBLOCKING, NO_CHAIN, FUNC_MACL_PRE, NO_FLAG_UPDATE,
                                        DST_ADDR((2*NUM_OF_NEIGHBOURS)*LOAD_SIZE_RF+x-1+NUM_CLASSES*z, 0, 0),
                                        SRC1_ADDR(NUM_OF_NEIGHBOURS*LOAD_SIZE_RF+NUM_OF_NEIGHBOURS*z, 1, 0), SRC2_IMM(1),
                                        NUM_OF_NEIGHBOURS-1, 0);
    }
}
for (int z=0;z<LOAD_SIZE_RF; z++)
{
    __builtin_vpro_instruction_word(lane_id, NONBLOCKING, NO_CHAIN, FUNC_MAX_VECTOR, NO_FLAG_UPDATE,
                                    DST_ADDR(z, 0, 1),
                                    SRC1_ADDR((2*NUM_OF_NEIGHBOURS)*LOAD_SIZE_RF+NUM_CLASSES*z, 1, NUM_CLASSES),
                                    SRC2_ADDR(1, 0, 0),
                                    NUM_CLASSES-1, 1-1); 
}
}
//---------------------------------------------------------------------------------------
void voting::compute_the_votes(int lm_offset)
{
for (int x=0; x<(LOAD_SIZE_LM/(LOAD_SIZE_RF*2)); x++)//ohne converter
    {
        
    //------------------lane0-----------//load data to rf
        load_data_to_rf((2*x)*LOAD_SIZE_RF*NUM_OF_NEIGHBOURS+NUM_OF_NEIGHBOURS*LOAD_SIZE_LM+lm_offset, LOAD_SIZE_RF*NUM_OF_NEIGHBOURS, L0,FLAG_UPDATE, NONBLOCKING);//load knn
        load_data_to_rf((2*x)*LOAD_SIZE_RF*NUM_OF_NEIGHBOURS+lm_offset, 0, L0,NO_FLAG_UPDATE,BLOCKING);//load labels
    //------------------lane1-----------//load data to rf
        load_data_to_rf((2*x+1)*LOAD_SIZE_RF*NUM_OF_NEIGHBOURS+NUM_OF_NEIGHBOURS*LOAD_SIZE_LM+lm_offset, LOAD_SIZE_RF*NUM_OF_NEIGHBOURS, L1, FLAG_UPDATE,NONBLOCKING);
        load_data_to_rf((2*x+1)*LOAD_SIZE_RF*NUM_OF_NEIGHBOURS+lm_offset, 0, L1,NO_FLAG_UPDATE,NONBLOCKING);
    //-------------------------------------
        compute_knn_labels(L0_1);
        calculate_the_votes(L0_1);
        save_to_lm(2*x*LOAD_SIZE_RF+lm_offset,L0,0);//save even lists
        save_to_lm((2*x+1)*LOAD_SIZE_RF+lm_offset, L1, 1);//save odd lists                                            
    }
}
//----------------------------------------------------------------------------------------
void voting::find_the_most_votet_label(int output_addr)
{
    get_L_prim();
    int buffering=0;
    int cycle = 0;
    //--------------------------peeling
    load_data_C_U(buffering*BUFFERING_OFFSET, cycle);
    dma_wait_to_finish(0xFFFF);
    compute_the_votes(buffering*BUFFERING_OFFSET);
    cycle++;
    load_data_C_U((!buffering)*BUFFERING_OFFSET, cycle);
    //----------------------------------
    int num_cycles = TOTAL_NUM_OF_POINTS/(NUM_CLUSTERS*NUM_VU_PER_CLUSTER*LOAD_SIZE_LM);
    for (;cycle <num_cycles+1; cycle++)// the 2 and 1 offset is there to make sure that every number is saved at last
    {
        save_data_C_U(buffering*BUFFERING_OFFSET, cycle - 1);
        load_data_C_U(buffering*BUFFERING_OFFSET, cycle + 1);
        dma_wait_to_finish(0xffff); 
        buffering=!buffering;
        compute_the_votes(buffering*BUFFERING_OFFSET);
        vpro_wait_busy(0xffff,0xffff);
    } 
    save_data_C_U(buffering*BUFFERING_OFFSET, cycle - 1);// last peeling  
    dma_wait_to_finish(0xffff);   
}