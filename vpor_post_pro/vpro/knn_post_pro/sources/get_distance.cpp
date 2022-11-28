#include "../includes/get_distance.h"

void get_distance::load_neighbours(int cluster,int unit,int input_addr,int loop_y_element, int loop_x_element,int stride_extera ,
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
dma_wait_to_finish(0xFFFF);//padding needs dma
}

//------------------------------------------------------------addressing
int get_distance::get_sgm_offset(int cluster, int unit)
{
    return cluster*NUM_CLUSTERS*unit+unit*NUM_VU_PER_CLUSTER;
}
//------------------------------------------------------------
void get_distance::load_data_C_U(int lm_offset, int cycle)
{
int sgm_num=cycle*SGM_BATCH_SIZE;
for (int unit=0;unit<NUM_VU_PER_CLUSTER;unit++)//load data 
{    
    for (int cluster=0;cluster<NUM_CLUSTERS;cluster++)
    {
        load_N_to_lm(cluster, unit, sgm_num,lm_offset);
        sgm_num=sgm_num+LOAD_SIZE_LM;                 
    }
}
}
//------------------------------------------------------------
void get_distance::save_data_C_U(int lm_offset, int cycle)
{
int sgm_num=cycle*SGM_BATCH_SIZE;
for (int unit=0;unit<NUM_VU_PER_CLUSTER;unit++)//store
{    
    for (int cluster=0;cluster<NUM_CLUSTERS;cluster++)
    {// if abfrage for last cycles
        int mm_addr=DISTANCE_ADDR+BYTES_PER_DATA*NUM_OF_NEIGHBOURS*sgm_num;
        dma_loc1D_to_ext1D( cluster,
                            mm_addr,
                            lm_offset+LM_BASE_VU(unit),
                            NUM_OF_NEIGHBOURS*LOAD_SIZE_LM);
        sgm_num=sgm_num+LOAD_SIZE_LM;  
    }
}
}
//------------------------------------------------------------
void get_distance::find_neighbours(int cluster,int unit,int x, int y,int label2D_addr)
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
void get_distance::get_N_prim()
{
int sgm_num=0;
    for (int x=0;x<((WIDTH*HEIGHT/(NUM_CLUSTERS))+1);x++)
    {   
            for (int cluster=0;cluster<NUM_CLUSTERS;cluster++)
            {
                int x= (sgm_num%WIDTH);// locater the coordinate of the pixel W
                int y= int(sgm_num/WIDTH);// locater the coordinate of the pixel H
                int mm_addr=N_PRIM_ADDR+BYTES_PER_DATA*NUM_OF_NEIGHBOURS*(sgm_num);//main memory

                find_neighbours(cluster, 0, x, y, DEAPTH2D_ADDR);
                dma_loc1D_to_ext1D( cluster,
                                    mm_addr,
                                    LM_BASE_VU(0),
                                    NUM_OF_NEIGHBOURS);
                sgm_num++;      
            }
             
    }        
}
//-------------------------------------------------------------------------------------------------------------------------------------

void get_distance::load_N_to_lm(int cluster, int unit, int mm_offset, int lm_offset)
{   
    //using ofset let us decide to chooce a portion of data
    for (int x=mm_offset ; x<LOAD_SIZE_LM+mm_offset ;x++)
    {
        int mm_addr=((*get_main_memory(POINTS_2D_MAPPING_ADDR+BYTES_PER_DATA*x, 1))*NUM_OF_NEIGHBOURS)*BYTES_PER_DATA+N_PRIM_ADDR;
        int lm_addr=LM_BASE_VU(unit)+(x-mm_offset)*NUM_OF_NEIGHBOURS+lm_offset;
        // extending the N_prim to bigger point cloud number of points
        dma_ext1D_to_loc1D( cluster,
                            mm_addr,
                            lm_addr,
                            NUM_OF_NEIGHBOURS);
        // adding the depth3d data to our N
        dma_ext1D_to_loc1D( cluster,
                            DEAPTH3D_ADDR+BYTES_PER_DATA*x,
                            lm_addr+MIDDLE_POINT,
                            1);
    }
}
//-----------------------------------------------------------------------------------------------------------------------------------
void get_distance::calculate_distance(int lm_offset)
{
    for (int y=0; y<(LOAD_SIZE_LM/(LOAD_SIZE_RF*NUM_VECTORLANES));y++)//go through each point
    { 
    //-----------------lane0--------------------//load    
        __builtin_vpro_instruction_word(LS, NONBLOCKING, IS_CHAIN, FUNC_LOAD, NO_FLAG_UPDATE,
                                        DST_ADDR(0, 0, 0),
                                        SRC1_ADDR(0, 1, NUM_OF_NEIGHBOURS),
                                        SRC2_IMM(2*y*(NUM_OF_NEIGHBOURS*LOAD_SIZE_RF)+lm_offset),//take the even addr
                                        NUM_OF_NEIGHBOURS-1,
                                        LOAD_SIZE_RF-1 );
        __builtin_vpro_instruction_word(L0, NONBLOCKING, NO_CHAIN, FUNC_ADD, NO_FLAG_UPDATE,
                                        DST_ADDR(NUM_OF_NEIGHBOURS-1, 1, NUM_OF_NEIGHBOURS),// -1 to pass all to 1024
                                        SRC1_LS,
                                        SRC2_IMM(0),
                                        NUM_OF_NEIGHBOURS-1,
                                        LOAD_SIZE_RF-1);
    //-----------------lane0--------------------//get distance to middle point    
        //find the distance between the knn points
        __builtin_vpro_instruction_word(L0, NONBLOCKING, NO_CHAIN, FUNC_SUB, NO_FLAG_UPDATE,
                                        DST_ADDR(0, 1, NUM_OF_NEIGHBOURS),// to avoide middle point be over writen
                                        SRC1_ADDR(NUM_OF_NEIGHBOURS-1, 1, NUM_OF_NEIGHBOURS),
                                        SRC2_ADDR(MIDDLE_POINT+NUM_OF_NEIGHBOURS-1,0,NUM_OF_NEIGHBOURS),
                                        NUM_OF_NEIGHBOURS-1,
                                        LOAD_SIZE_RF-1);
    //-----------------lane1--------------------//load 
        __builtin_vpro_instruction_word(LS, NONBLOCKING, IS_CHAIN, FUNC_LOAD, NO_FLAG_UPDATE,
                                        DST_ADDR(0, 0, 0),
                                        SRC1_ADDR(0, 1, NUM_OF_NEIGHBOURS),
                                        SRC2_IMM((2*y+1)*NUM_OF_NEIGHBOURS*LOAD_SIZE_RF+lm_offset),//take the odd addr
                                        NUM_OF_NEIGHBOURS-1,
                                        LOAD_SIZE_RF-1 );
        __builtin_vpro_instruction_word(L1, NONBLOCKING, NO_CHAIN, FUNC_ADD, NO_FLAG_UPDATE,
                                        DST_ADDR(NUM_OF_NEIGHBOURS-1, 1, NUM_OF_NEIGHBOURS),// -1 to pass all to 1024
                                        SRC1_LS,
                                        SRC2_IMM(0),
                                        NUM_OF_NEIGHBOURS-1,
                                        LOAD_SIZE_RF-1);   
    //-----------------lane1--------------------//get distance to middle point    
        //find the distance between the knn points
        __builtin_vpro_instruction_word(L1, NONBLOCKING, NO_CHAIN, FUNC_SUB, NO_FLAG_UPDATE,
                                        DST_ADDR(0, 1, NUM_OF_NEIGHBOURS),// to avoide middle point be over writen
                                        SRC1_ADDR(NUM_OF_NEIGHBOURS-1, 1, NUM_OF_NEIGHBOURS),
                                        SRC2_ADDR(MIDDLE_POINT+NUM_OF_NEIGHBOURS-1,0,NUM_OF_NEIGHBOURS),
                                        NUM_OF_NEIGHBOURS-1,
                                        LOAD_SIZE_RF-1);
    //-----------------lane0--------------------//save to lm
        // dictance should be positive number 
        __builtin_vpro_instruction_word(L0, NONBLOCKING, IS_CHAIN, FUNC_ABS, NO_FLAG_UPDATE,
                                        DST_ADDR(0, 1, NUM_OF_NEIGHBOURS),
                                        SRC1_ADDR(0, 1, NUM_OF_NEIGHBOURS),
                                        SRC2_IMM(0),
                                        NUM_OF_NEIGHBOURS-1,
                                        LOAD_SIZE_RF-1 ); 
        // load data back in lm
        __builtin_vpro_instruction_word(LS, NONBLOCKING, NO_CHAIN, FUNC_STORE, NO_FLAG_UPDATE,
                                        DST_ADDR(0, 1, NUM_OF_NEIGHBOURS),
                                        SRC1_CHAINING(0),
                                        SRC2_IMM(2*y*(NUM_OF_NEIGHBOURS*LOAD_SIZE_RF)+lm_offset),
                                        NUM_OF_NEIGHBOURS-1,
                                        LOAD_SIZE_RF-1);
        vpro_wait_busy(0xFFFF, 0xFFFF);
    //-----------------lane1--------------------//save to lm
        __builtin_vpro_instruction_word(L1, NONBLOCKING, IS_CHAIN, FUNC_ABS, NO_FLAG_UPDATE,
                                        DST_ADDR(0, 1, NUM_OF_NEIGHBOURS),
                                        SRC1_ADDR(0, 1, NUM_OF_NEIGHBOURS),
                                        SRC2_IMM(0),
                                        NUM_OF_NEIGHBOURS-1,
                                        LOAD_SIZE_RF-1 ); 
        __builtin_vpro_instruction_word(LS, NONBLOCKING, NO_CHAIN, FUNC_STORE, NO_FLAG_UPDATE,
                                        DST_ADDR(0, 1, NUM_OF_NEIGHBOURS),
                                        SRC1_CHAINING(1),
                                        SRC2_IMM((2*y+1)*NUM_OF_NEIGHBOURS*LOAD_SIZE_RF+lm_offset),//take the odd addr
                                        NUM_OF_NEIGHBOURS-1,
                                        LOAD_SIZE_RF-1);  
    }
}
//----------------------------------------------------------------------------------------------------------------------------
void get_distance::get_distance_and_save_it()
{
    get_N_prim();//get the data needed for N
    
    int buffering=0;
    int cycle = 0;
    //--------------------------peeling
    load_data_C_U(buffering*BUFFERING_OFFSET, cycle);
    calculate_distance(buffering*BUFFERING_OFFSET);
    cycle++;
    load_data_C_U((!buffering)*BUFFERING_OFFSET, cycle);
    //----------------------------------
    int num_cycles = TOTAL_NUM_OF_POINTS/(NUM_CLUSTERS*NUM_VU_PER_CLUSTER*LOAD_SIZE_LM);
    for (;cycle < num_cycles+1; cycle++)// the 2 and 1 offset is there to make sure that every number is saved at last
    {
        save_data_C_U(buffering*BUFFERING_OFFSET, cycle - 1);
        load_data_C_U(buffering*BUFFERING_OFFSET, cycle + 1);
        dma_wait_to_finish(0xffff);
        buffering=!buffering;
        calculate_distance(buffering*BUFFERING_OFFSET);
    }
    save_data_C_U(buffering*BUFFERING_OFFSET, cycle - 1);  // last peeling      
    dma_wait_to_finish(0xffff);   
}

