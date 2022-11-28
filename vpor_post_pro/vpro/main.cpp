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
#define NUM_VECTORLANES  2 // per unit

#ifndef CNN_YOLO_LITE_DEFINES_H
#define CNN_YOLO_LITE_DEFINES_H


constexpr int reference_datatype = 32;
constexpr bool reference_datatype_float = false;



#endif //CNN_YOLO_LITE_DEFINES_H



//----------------------------------------------------------------------------------
//----------------------------------Main--------------------------------------------
//----------------------------------------------------------------------------------
int main(int argc, char *argv[]) {
    // initialize simulator environment
    sim_init(main, argc, argv, MAIN_MEMORY_SIZE, LM_SIZE, RF_SIZE, NUM_CLUSTERS, NUM_VU_PER_CLUSTER, NUM_VECTORLANES);
    sim_printf("Clusters: %i, units: %i, lanes: %i", NUM_CLUSTERS, NUM_VU_PER_CLUSTER, NUM_VECTORLANES);
    // execute each on ALL vector units
    vpro_set_idmask(0xFFFFFFFF);

   // const int16_t kernel_outline[9] = {-1, -1, -1, -1, 8, -1, -1, -1, -1};
   // const int16_t kernel_blur[9] = {1, 2, 1, 2, 4, 2, 1, 2, 1};
   // const int16_t kernel_sharpen[9] = {0, -1, 0, -1, 5, -1, 0, -1, 0};
   // const int16_t kernel_laplace[9] = {0, 1, 0, 1, -4, 1, 0, 1, 0};
   // const int16_t kernel_sobel_x[9] = {-1, 0, 1, -2, 0, 2, -1, 0, 1};
   // const int16_t kernel_sobel_y[9] = {-1, -2, -1, 0, 0, 0, 1, 2, 1};
  // const int16_t G[25] = {0.99951871 ,0.99784303 ,0.99644375 ,0.99784303 ,0.99951871 ,0.99784303
 //,0.99033311 ,0.984062 ,0.99033311 ,0.99784303 ,0.99644375 ,0.984062
// 0.97372268 ,0.984062 ,0.99644375 ,0.99784303 ,0.99033311 0.984062
// 0.99033311 ,0.99784303 ,0.99951871 ,0.99784303 ,0.99644375 ,0.99784303
// ,0.99951871};



 
    
    // ************************** YOUR CODE **************************** //
    //dma_ext2D_to_loc1D(0,0x0,0x0,217,8,8);
   // dma_wait_to_finish(0xFFFF);
   // dma_loc1D_to_ext2D(0,0x30D40,0x0,217,8,8);
   // dma_wait_to_finish(0xFFFF);
    //dma_ext1D_to_loc1D(0,(uint64_t)((intptr_t)(kernel_laplace)),0x40,9);
      //  dma_wait_to_finish(0xFFFF);
        // kernel load
         /*    __builtin_vpro_instruction_word(LS, NONBLOCKING, IS_CHAIN, FUNC_LOADS, NO_FLAG_UPDATE,
            DST_ADDR(0, 0, 0), SRC1_ADDR(64, 1, 3), SRC2_IMM(0), 2, 2);
            __builtin_vpro_instruction_word(L0, NONBLOCKING, NO_CHAIN, FUNC_ADD, NO_FLAG_UPDATE,
                DST_ADDR(64, 1, 3), SRC1_LS, SRC2_IMM(0), 2, 2);
            
            __builtin_vpro_instruction_word(LS, NONBLOCKING, IS_CHAIN, FUNC_LOAD, NO_FLAG_UPDATE,
            DST_ADDR(0, 0, 0), SRC1_ADDR(0, 1, 8), SRC2_IMM(0), 7, 7);
            __builtin_vpro_instruction_word(L0, NONBLOCKING, NO_CHAIN, FUNC_ADD, NO_FLAG_UPDATE,
            DST_ADDR(0, 1, 8), SRC1_LS, SRC2_IMM(0), 7, 7);
            //loop for the 6*6 pixel
            int x,y,z;
            z=0;
            for (x=0; x<6;x++)
        {
               for (y=0; y<6;y++)
                {
        
                __builtin_vpro_instruction_word(L0, NONBLOCKING, NO_CHAIN, FUNC_MACL_PRE, NO_FLAG_UPDATE,
                    DST_ADDR((76+x+y+z), 0, 0), SRC1_ADDR(64, 1, 3), SRC2_ADDR((y+z),1,8), 2, 2);
            
                }
         z=z+5;
        }
        
         __builtin_vpro_instruction_word(L0, NONBLOCKING, IS_CHAIN, FUNC_ADD, NO_FLAG_UPDATE,
         DST_ADDR(100, 1, 6), SRC1_ADDR(76, 1, 6), SRC2_IMM(0), 5, 5);
         
         __builtin_vpro_instruction_word(LS, NONBLOCKING, IS_CHAIN, FUNC_STORE, NO_FLAG_UPDATE,
         DST_ADDR(76, 1, 6), SRC1_CHAINING(0), SRC2_IMM(0), 5, 5);
         */

         // starting : reding data
        dma_set_pad_widths(2,2,0,0);
        dma_set_pad_value(0)


    dma_ext2D_to_loc1D(0,0x0,0x0,1015,10,10);
    dma_wait_to_finish(0xFFFF);
        
 __builtin_vpro_instruction_word(LS, NONBLOCKING, IS_CHAIN, FUNC_LOADS, NO_FLAG_UPDATE,
            DST_ADDR(0, 0, 0), SRC1_ADDR(0, 1, 10), SRC2_IMM(0), 9, 9);
            __builtin_vpro_instruction_word(L0, NONBLOCKING, NO_CHAIN, FUNC_ADD, NO_FLAG_UPDATE,
                DST_ADDR(0, 1, 10), SRC1_LS, SRC2_IMM(0), 9, 9);       

dma_wait_to_finish(0xFFFF);
        
   // dma_wait_to_finish(0xFFFF);
        
        
         
         

         
       
         

         // comment             


           

        
    

            vpro_wait_busy(0xFFFF, 0xFFFF);
            //dma_loc1D_to_ext2D(0,0x30D40,0x0,217,8,8);
            //dma_wait_to_finish(0xFFFF);

    

 

        





    /************ ISA *********/
    // *** VPRO VectorUnits/Lanes functions *** //
    /*
    void __builtin_vpro_instruction_word(uint32_t id, uint32_t blocking, uint32_t is_chain, 
                                         uint32_t func, uint32_t flag_update, uint32_t dst,
                                         uint32_t src1, uint32_t src2, 
                                         uint32_t x_end, uint32_t y_end);

    void vpro_wait_busy(uint32_t cluster, uint32_t unit_mask);
    */

    // *** DMA functions *** //
    /*
    // *** Extern -> Local *** //
    void dma_ext1D_to_loc1D(uint32_t cluster, uint64_t ext_base, uint32_t loc_base, 
                            uint32_t word_count);
    void dma_ext2D_to_loc1D(uint32_t cluster, uint64_t ext_base, uint32_t loc_base,
                            uint32_t x_stride, uint32_t x_size, uint32_t y_size);

    // *** Local -> Extern *** //
    void dma_loc1D_to_ext1D(uint32_t cluster, uint64_t ext_base, uint32_t loc_base,
                            uint32_t word_count);
    void dma_loc1D_to_ext2D(uint32_t cluster, uint64_t ext_base, uint32_t loc_base,
                            uint32_t x_stride, uint32_t x_size, uint32_t y_size);

    // *** Others *** //
    void dma_wait_to_finish(uint32_t cluster);
    */

    sim_stop();
    return 0; // return to crt0.asm and loop forever
}

