// ########################################################
// # VPRO instruction & system simulation library         #
// # Sven Gesper, IMS, Uni Hannover, 2019                 #
// ########################################################
// # Base class which defines VPROs supported instr       #
// # -> is C++                                            #
// # -> evaluated for simulation only                     #
// ########################################################

#ifndef core_interface_class
#define core_interface_class

//#include <string>

// include aux library
#include "../isa_intrinsic_aux_lib.h"


// Interface to define all functions. implementation (sim/... ) is done inside SimCore / VProCore
// all functions are pure virual void (requires an implementation)

static bool default_flags[4] = {0,0,0,0};

class Core{
public:
    // *** VPRO VectorUnits/Lanes *** //
    virtual void __builtin_vpro_instruction_word(uint32_t id, uint32_t blocking, uint32_t is_chain, uint32_t fu_sel, uint32_t func, uint32_t flag_update, uint32_t dst, uint32_t src1_sel, uint32_t src1, uint32_t src2_sel, uint32_t src2, uint32_t x_end, uint32_t y_end) = 0;
    virtual void vpro_wait_busy(uint32_t cluster, uint32_t unit_mask) = 0;
    virtual void vpro_set_idmask(uint32_t idmask) = 0;
    virtual void vpro_set_unit_mask(uint32_t idmask) = 0;
    virtual void vpro_set_cluster_mask(uint32_t idmask) = 0;
    virtual void vpro_pipeline_wait() = 0;

    virtual void vpro_loop_start(uint32_t start, uint32_t end, uint32_t increment_step) = 0;
    virtual void vpro_loop_end() = 0;
    virtual void vpro_loop_mask(uint32_t src1_mask, uint32_t src2_mask, uint32_t dst_mask) = 0;

    virtual void vpro_mac_h_bit_shift(uint32_t shift) = 0;
    virtual void vpro_mul_h_bit_shift(uint32_t shift) = 0;

    // *** VPRO DMA *** //
    virtual void dma_ext1D_to_loc1D(uint32_t cluster, uint64_t ext_base, uint32_t loc_base, uint32_t word_count, uint32_t x_stride = 0, uint32_t x_size = 0, uint32_t y_size = 0, bool is_broadcast_no_checking = false) = 0;
    virtual void dma_ext2D_to_loc1D(uint32_t cluster, uint64_t ext_base, uint32_t loc_base, int32_t x_stride, uint32_t x_size, uint32_t y_size, bool pad_flags[4] = default_flags, bool is_broadcast_no_checking = false) = 0;
    virtual void dma_loc1D_to_ext1D(uint32_t cluster, uint64_t ext_base, uint32_t loc_base, uint32_t word_count) = 0;
    virtual void dma_loc1D_to_ext2D(uint32_t cluster, uint64_t ext_base, uint32_t loc_base, int32_t x_stride, uint32_t x_size, uint32_t y_size) = 0;
    virtual void dma_wait_to_finish(uint32_t cluster) = 0;

    virtual void dma_set_pad_widths(uint32_t top, uint32_t right, uint32_t bottom, uint32_t left) = 0;
    virtual void dma_set_pad_value(int16_t value) = 0;

    // *** Main Memory *** //
    virtual int  bin_file_send(uint32_t addr, int num_bytes, char const* file_name) = 0;
    virtual int  bin_file_dump(uint32_t addr, int num_bytes, char const* file_name) = 0;
    virtual uint8_t* bin_file_return(uint32_t addr, int num_bytes) = 0;
    virtual uint8_t* bin_lm_return(uint32_t cluster, uint32_t unit) = 0;
    virtual uint8_t* bin_rf_return(uint32_t cluster, uint32_t unit, uint32_t lane) = 0;
    virtual void aux_memset(uint32_t base_addr, uint8_t value, uint32_t num_bytes) = 0;


    // *** System *** //
    virtual void aux_print_debugfifo(uint32_t data) = 0;
    //virtual uint32_t aux_dev_null(uint32_t data) = 0;
    virtual void aux_dev_null(uint32_t data) = 0;
    virtual void aux_flush_dcache() = 0;
    virtual void aux_clr_sys_time() = 0;
    virtual void aux_clr_cycle_cnt() = 0;
    virtual uint32_t aux_get_cycle_cnt() = 0;
    virtual void aux_send_system_runtime() = 0;
    virtual uint32_t aux_get_sys_time_lo() = 0;
    virtual uint32_t aux_get_sys_time_hi() = 0;



    // *** Simulator debugging funtions ***
    virtual void sim_dump_local_memory(uint32_t cluster, uint32_t unit) = 0;
    virtual void sim_dump_register_file(uint32_t cluster, uint32_t unit, uint32_t lane) = 0;
    virtual void sim_wait_step( bool finish, const char* msg) = 0;

    void sim_printf(...){};
//    template<typename... Args> void sim_printf(const char *format, Args... args);

//    virtual int  sim_printf(const char *format, ...) = 0;
//    virtual int printf_warning(const char *format, ...) = 0;
//    virtual int printf_error(const char *format, ...) = 0;


    // *** General simulator environment control ***
    virtual int sim_init(int (*main_fkt)(int, char**), int &argc, char* argv[]) = 0;
    virtual int sim_init(int (*main_fkt)(int, char**), int &argc, char* argv[], uint32_t main_memory_size, uint32_t local_memory_size, uint32_t register_file_size, int cluster_count, int vector_unit_count, int vector_lane_count, int pipeline_depth) = 0;
    virtual void sim_stop() = 0;

};

#endif
