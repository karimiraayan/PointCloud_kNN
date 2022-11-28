// ########################################################
// # VPRO instruction & system simulation library         #
// # Sven Gesper, IMS, Uni Hannover, 2019                 #
// ########################################################

#ifndef core_class_wrapper
#define core_class_wrapper

#include "../vpro/common/vpro_globals.h"
#include <stdlib.h>

#if (IS_SIMULATION == 1) || defined(CORE_USE_CPP)
    #include "model/core.h"
#endif

#if (IS_SIMULATION == 1)
    #include "simulator/simCore.h"
    extern SimCore * core_;
#else
    // -----------------------------------------------------------------------
    // General hardware configuration
    // -----------------------------------------------------------------------
    //#define NUM_CLUSTERS        4 // number of clusters
    //#define NUM_VU_PER_CLUSTER  8 // vector units per cluster
    //#define LM_SIZE             8192 // #entries in each local memory
    //#define USE_HW_LOOPING      0 // use hardware looping when set
    // -----------------------------------------------------------------------

    #ifdef CORE_USE_CPP
        #include "vproCore.h"
        Core * core_ = new Core();
    #else
        // include aux library
        #include "isa_intrinsic_aux_lib.h"
        #include "../vpro/common/vpro_mips.h" // VPRO.MIPS address map
        #include "../vpro/common/mips_aux.h" // MIPS CPU helper functions
        #include "../vpro/common/vpro_aux.h" // VPRO helper functions
    #endif

    class CommandDMA {
        public:
            enum PAD{
                TOP = 0,
                RIGHT = 1,
                BOTTOM = 2,
                LEFT = 3
            };
    };

#endif

#if (IS_SIMULATION == 1) || defined(CORE_USE_CPP)

    #include <string>
    #include <cstdarg>
    #include <vector>
    #include "simulator/helper/debugHelper.h"

    // *** VPRO VectorUnits/Lanes *** //
    void __load_shift_left(uint32_t id, uint32_t lm_immediate, uint32_t offset, uint32_t alpha, uint32_t beta, uint32_t shift_factor, uint32_t x_end, uint32_t y_end);
    void __load_shift_right(uint32_t id, uint32_t lm_immediate, uint32_t offset, uint32_t alpha, uint32_t beta, uint32_t shift_factor, uint32_t x_end, uint32_t y_end);
    void __load_reverse(uint32_t lm_immediate, uint32_t offset, int32_t alpha, int32_t beta, uint32_t x_end, uint32_t y_end);

    void __store_reverse(uint32_t id, uint32_t lm_immediate, uint32_t offset, int32_t alpha, int32_t beta, uint32_t x_end, uint32_t y_end);

    void __find_min_vector(LANE id, uint32_t dst, uint32_t src1_sel, uint32_t src1, uint32_t x_end, uint32_t y_end, bool find_index = false, bool chain = false, bool blocking = false, bool flag_update = false);
    void __find_max_vector(LANE id, uint32_t dst, uint32_t src1_sel, uint32_t src1, uint32_t x_end, uint32_t y_end, bool find_index = false, bool chain = false, bool blocking = false, bool flag_update = false);



    void __builtin_vpro_instruction_word(uint32_t id, uint32_t blocking, uint32_t is_chain, uint32_t fu_sel, uint32_t func, uint32_t flag_update, uint32_t dst, uint32_t src1_sel, uint32_t src1, uint32_t src2_sel, uint32_t src2, uint32_t x_end, uint32_t y_end);
    void __vpro(uint32_t id, uint32_t blocking, uint32_t is_chain, uint32_t fu_sel, uint32_t func, uint32_t flag_update, uint32_t dst, uint32_t src1_sel, uint32_t src1, uint32_t src2_sel, uint32_t src2, uint32_t x_end, uint32_t y_end);

    void vpro_wait_busy(uint32_t cluster_mask, uint32_t unit_mask);
    void vpro_wait_busy(uint32_t cluster_mask, uint32_t unit_mask, int cycle);
    void vpro_set_idmask(uint32_t idmask);
    void vpro_set_unit_mask(uint32_t idmask);
    void vpro_set_cluster_mask(uint32_t idmask);
    void vpro_pipeline_wait();


    void vpro_loop_start(uint32_t start, uint32_t end, uint32_t increment_step);
    void vpro_loop_end();
    void vpro_loop_mask(uint32_t src1_mask, uint32_t src2_mask, uint32_t dst_mask);

    void vpro_mac_h_bit_shift(uint32_t shift);
    void vpro_mul_h_bit_shift(uint32_t shift);

    // *** VPRO DMA *** //
    void dma_ext1D_to_loc1D(uint32_t cluster, uint64_t ext_base, uint32_t loc_base, uint32_t word_count, uint32_t x_stride = 0, uint32_t x_size = 0, uint32_t y_size = 0, bool is_broadcast_no_checking = false);
    void dma_ext2D_to_loc1D(uint32_t cluster, uint64_t ext_base, uint32_t loc_base, int32_t x_stride, uint32_t x_size, uint32_t y_size, bool pad_flags[4] = default_flags, bool is_broadcast_no_checking = false);
    void dma_loc1D_to_ext1D(uint32_t cluster, uint64_t ext_base, uint32_t loc_base, uint32_t word_count);
    void dma_loc1D_to_ext2D(uint32_t cluster, uint64_t ext_base, uint32_t loc_base, int32_t x_stride, uint32_t x_size, uint32_t y_size);
    void dma_wait_to_finish(uint32_t cluster_mask, int cycle);
    void dma_ext1D_to_loc1D_broadcast(uint32_t cluster, uint64_t unit_mask, uint64_t ext_base, uint32_t loc_base, uint32_t word_count);
    void dma_ext2D_to_loc1D_broadcast(uint32_t cluster, uint64_t unit_mask, uint64_t ext_base, uint32_t loc_base, int32_t x_stride, uint32_t x_size, uint32_t y_size, bool pad_flags[4] = default_flags);

    void dma_wait_to_finish(uint32_t cluster_mask);

    void dma_set_pad_widths(uint32_t top, uint32_t right, uint32_t bottom, uint32_t left);
    void dma_set_pad_value(int16_t value);

    // *** Main Memory *** //
    int  bin_file_send(uint32_t addr, int num_bytes, char const* file_name);
    int  bin_file_dump(uint32_t addr, int num_bytes, char const* file_name);
    uint8_t* bin_file_return(uint32_t addr, int num_bytes);
    uint16_t *get_main_memory(uint32_t addr, int num_elements);
    uint16_t *get_mm_element(uint32_t addr);
    uint8_t* bin_rf_return(uint32_t cluster, uint32_t unit, uint32_t lane);
    uint8_t* bin_lm_return(uint32_t cluster, uint32_t unit);
    void aux_memset(uint32_t base_addr, uint8_t value, uint32_t num_bytes);

    // *** System *** //
    void aux_print_debugfifo(uint32_t data);
    void aux_dev_null(uint32_t data);
    void aux_flush_dcache();
    void aux_clr_dcache();
    void aux_wait_cycles(int cycles);
    void aux_clr_icache();
    void aux_clr_sys_time();
    void aux_clr_cycle_cnt();
    uint32_t aux_get_cycle_cnt();
    void aux_send_system_runtime();
    uint32_t aux_get_sys_time_lo();
    uint32_t aux_get_sys_time_hi();

    // *** Simulator debugging funtions ***
    void sim_dump_local_memory(uint32_t cluster, uint32_t unit);
    void sim_dump_queue(uint32_t cluster, uint32_t unit);
    void sim_dump_register_file(uint32_t cluster, uint32_t unit, uint32_t lane);
    void sim_finish_and_wait_step(int clusters);
    void sim_wait_step(bool finish = false, const char* msg = "");

    // *** General simulator environment control ***
    int sim_init(int (*main_fkt)(int, char**), int argc, char* argv[]);
    int sim_init(int (*main_fkt)(int, char**), int &argc, char* argv[], uint32_t main_memory_size = 1024*1024*1024, uint32_t local_memory_size = 8192, uint32_t register_file_size = 1024, int cluster_count = 1, int vector_unit_count = 1, int vector_lane_count = 2, int pipeline_depth = 10);
    void sim_stop();

    void sim_printf(const char *format);

//    template<typename... Args> void sim_printf(const char *format, Args... args);
    template<typename... Args> void sim_printf(const char *format, Args... args){
        core_->sim_printf(format, args...);
    }

#endif

// This is C++ so forward c function calls to functions in the object.






#endif
