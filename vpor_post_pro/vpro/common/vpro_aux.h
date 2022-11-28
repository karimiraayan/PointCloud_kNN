// ###############################################################
// # vpro_aux.h - VPRO system helper functions                   #
// ###############################################################

#ifndef vpro_aux_h
#define vpro_aux_h

// IO map
#include "vpro_mips.h"
#include "../../isa_intrinsic_lib/isa_intrinsic_aux_lib.h"
#include "vpro_globals.h"

static bool default_flags[4] = {0,0,0,0};

// ***************************************************************************************************************
// iDMA elementary function prototypes
// ***************************************************************************************************************
inline void __attribute__((always_inline)) dma_set_pad_widths(uint32_t top, uint32_t right, uint32_t bottom, uint32_t left) {
    IDMA_PAD_TOP_SIZE = top;
    IDMA_PAD_BOTTOM_SIZE = bottom;
    IDMA_PAD_RIGHT_SIZE = right;
    IDMA_PAD_LEFT_SIZE = left;
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
}

inline void __attribute__((always_inline)) dma_set_pad_value(uint32_t value) {
    IDMA_PAD_VALUE = value;
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
}

// ---------------------------------------------------------------------------------
// Blocks until all DMA transactions in cluster are done
// ---------------------------------------------------------------------------------
inline void __attribute__((always_inline)) dma_wait_to_finish(uint32_t cluster_mask) {
    // wait for CDC of busy signal
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
    for (int cluster = 0; cluster < HW.CLUSTERS; ++cluster) {
        if (((cluster_mask >> cluster) & 0x1u) == 0)
            continue;
        volatile uint32_t *status_reg = (volatile uint32_t *) ((cluster << 8) + (uint32_t)(intptr_t)(&IDMA_STATUS_BUSY));
        while (*status_reg != 0) {
            continue;
        }
    }
}

// ---------------------------------------------------------------------------------
// Sets up the count/stride registers for an EXT 1D => LOC 1D transfer.
// non-blocking!
// ---------------------------------------------------------------------------------
inline void __attribute__((always_inline)) dma_ext1D_to_loc1D(uint32_t cluster, uint32_t ext_base, uint32_t loc_base, uint32_t word_count) {

    *((volatile uint32_t *) ((cluster << 8) + (uint32_t)(intptr_t)(&IDMA_LOC_BASE_ADDR))) = loc_base;
    *((volatile uint32_t *) ((cluster << 8) + (uint32_t)(intptr_t)(&IDMA_BLOCK_X))) = word_count; // 16-bit elements!
//  *((volatile uint32_t*)((cluster << 8) + (uint32_t)(&IDMA_BLOCK_Y)))           = 1; // 16-bit elements!
//  *((volatile uint32_t*)((cluster << 8) + (uint32_t)(&IDMA_BLOCK_STRIDE)))      = 1; // 16-bit elements!
    *((volatile uint32_t *) ((cluster << 8) + (uint32_t)(intptr_t)(&IDMA_EXT_BASE_ADDR_E2L))) = ext_base; // + trigger
}


// ---------------------------------------------------------------------------------
// Sets up the count/stride registers for an EXT 2D => LOC 1D transfer.
// non-blocking!
// ---------------------------------------------------------------------------------
inline void __attribute__((always_inline)) dma_ext2D_to_loc1D(uint32_t cluster, uint32_t ext_base, uint32_t loc_base, uint32_t x_stride, uint32_t x_size,
                        uint32_t y_size, bool pad_flags[4] = default_flags) {
//    pad_flags[dir]  -- in CommandDMA::PAD::TOP/...
//    TOP = 0,
//    RIGHT = 1,
//    BOTTOM = 2,
//    LEFT = 3
    if (pad_flags[1])
        x_size |= 0x10000000;   // right @28
    if (pad_flags[3])
        x_size |= 0x20000000;   // left @29
    if (pad_flags[2])
        x_size |= 0x40000000;   // bottom @30
    if (pad_flags[0])
        x_size |= 0x80000000;   // top @31
    *((volatile uint32_t *) ((cluster << 8) + (uint32_t)(intptr_t)(&IDMA_LOC_BASE_ADDR))) = loc_base;
    *((volatile uint32_t *) ((cluster << 8) + (uint32_t)(intptr_t)(&IDMA_BLOCK_X))) = x_size; // 16-bit elements!
    *((volatile uint32_t *) ((cluster << 8) + (uint32_t)(intptr_t)(&IDMA_BLOCK_Y))) = y_size; // 16-bit elements!
    *((volatile uint32_t *) ((cluster << 8) + (uint32_t)(intptr_t)(&IDMA_BLOCK_STRIDE))) = x_stride - 1; // 16-bit elements!
    *((volatile uint32_t *) ((cluster << 8) + (uint32_t)(intptr_t)(&IDMA_EXT_BASE_ADDR_E2L))) = ext_base; // + trigger
}

// ---------------------------------------------------------------------------------
// Sets up the count/stride registers for a LOC 1D => EXT 1D transfer.
// non-blocking!
// ---------------------------------------------------------------------------------
inline void __attribute__((always_inline)) dma_loc1D_to_ext1D(uint32_t cluster, uint32_t ext_base, uint32_t loc_base, uint32_t word_count) {
    *((volatile uint32_t *) ((cluster << 8) + (uint32_t)(intptr_t)(&IDMA_LOC_BASE_ADDR))) = loc_base;
    *((volatile uint32_t *) ((cluster << 8) + (uint32_t)(intptr_t)(&IDMA_BLOCK_X))) = word_count; // 16-bit elements!
//  *((volatile uint32_t*)((cluster << 8) + (uint32_t)(&IDMA_BLOCK_Y)))           = 1; // 16-bit elements!
//  *((volatile uint32_t*)((cluster << 8) + (uint32_t)(&IDMA_BLOCK_STRIDE)))      = 1; // 16-bit elements!
    *((volatile uint32_t *) ((cluster << 8) + (uint32_t)(intptr_t)(&IDMA_EXT_BASE_ADDR_L2E))) = ext_base; // + trigger
}

// ---------------------------------------------------------------------------------
// Sets up the count/stride registers for a LOC 1D => EXT 2D transfer.
// non-blocking!
// ---------------------------------------------------------------------------------
inline void __attribute__((always_inline)) dma_loc1D_to_ext2D(uint32_t cluster, uint32_t ext_base, uint32_t loc_base, uint32_t x_stride, uint32_t x_size,
                        uint32_t y_size) {
    *((volatile uint32_t *) ((cluster << 8) + (uint32_t)(intptr_t)(&IDMA_LOC_BASE_ADDR))) = loc_base;
    *((volatile uint32_t *) ((cluster << 8) + (uint32_t)(intptr_t)(&IDMA_BLOCK_X))) = x_size; // 16-bit elements!
    *((volatile uint32_t *) ((cluster << 8) + (uint32_t)(intptr_t)(&IDMA_BLOCK_Y))) = y_size; // 16-bit elements!
    *((volatile uint32_t *) ((cluster << 8) + (uint32_t)(intptr_t)(&IDMA_BLOCK_STRIDE))) = x_stride - 1; // 16-bit elements!
    *((volatile uint32_t *) ((cluster << 8) + (uint32_t)(intptr_t)(&IDMA_EXT_BASE_ADDR_L2E))) = ext_base; // + trigger
}

inline void __attribute__((always_inline)) dma_ext1D_to_loc1D_broadcast(uint32_t cluster, uint32_t unit_mask, uint32_t ext_base, uint32_t loc_base,
                                  uint32_t word_count) {
// use upper 19-bits of x_stride (drop 13-bits of x_stride) concat with
// use upper-4 15-bits of x_size   (drop 13-bits of x_size  ) concat with
// use upper 19-bits of y_size   (drop 13-bits of y_size  ) => result with 3*19-bit = 57-bit

//    auto hig = (unit_mask >> (15+19-13)) & 0xFFFFE000; // 19-bit
//    auto mid = (unit_mask >> (19-13)) & 0x0FFFE000;  // 15-bit
//    auto low = ((unit_mask & 0x7ffff) << 13);  // 19-bit
    *((volatile uint32_t *) ((cluster << 8) + (uint32_t)(intptr_t)(&IDMA_LOC_BASE_ADDR))) = loc_base;
    *((volatile uint32_t *) ((cluster << 8) + (uint32_t)(intptr_t)(&IDMA_BLOCK_X))) =
            (uint32_t(unit_mask >> (19 - 13)) & 0x0FFFE000) | word_count; // 16-bit elements!
    *((volatile uint32_t *) ((cluster << 8) + (uint32_t)(intptr_t)(&IDMA_BLOCK_Y))) =
            (uint32_t(unit_mask & 0x7ffff) << 13) | 1; // 16-bit elements!
    *((volatile uint32_t *) ((cluster << 8) + (uint32_t)(intptr_t)(&IDMA_BLOCK_STRIDE))) =
            (uint32_t(unit_mask >> (15 + 19 - 13)) & 0xFFFFE000) | 1; // 16-bit elements!
    *((volatile uint32_t *) ((cluster << 8) + (uint32_t)(intptr_t)(&IDMA_EXT_BASE_ADDR_E2L))) = ext_base; // + trigger
}

inline void __attribute__((always_inline)) dma_ext2D_to_loc1D_broadcast(uint32_t cluster, uint32_t unit_mask, uint32_t ext_base, uint32_t loc_base,
                                  int32_t x_stride, uint32_t x_size, uint32_t y_size, bool pad_flags[4] = default_flags) {
//    pad_flags[dir] TOP = 0,   -- in CommandDMA::PAD::-
//    RIGHT = 1,dma_ext2D_to_loc1D
//    BOTTOM = 2,
//    LEFT = 3
    if (pad_flags[1])
        x_size |= 0x10000000;   // right
    if (pad_flags[3])
        x_size |= 0x20000000;   // left
    if (pad_flags[2])
        x_size |= 0x40000000;   // bottom
    if (pad_flags[0])
        x_size |= 0x80000000;   // top

// use upper 19-bits of x_stride (drop 13-bits of x_stride) concat with
// use upper-4 15-bits of x_size   (drop 13-bits of x_size  ) concat with
// use upper 19-bits of y_size   (drop 13-bits of y_size  ) => result with 3*19-bit = 57-bit
    *((volatile uint32_t *) ((cluster << 8) + (uint32_t)(intptr_t)(&IDMA_LOC_BASE_ADDR))) = loc_base;
    *((volatile uint32_t *) ((cluster << 8) + (uint32_t)(intptr_t)(&IDMA_BLOCK_X))) =
            (uint32_t(unit_mask >> (19 - 13)) & 0x0FFFE000) | x_size; // 16-bit elements!
    *((volatile uint32_t *) ((cluster << 8) + (uint32_t)(intptr_t)(&IDMA_BLOCK_Y))) =
            (uint32_t(unit_mask & 0x7ffff) << 13) | y_size; // 16-bit elements!
    *((volatile uint32_t *) ((cluster << 8) + (uint32_t)(intptr_t)(&IDMA_BLOCK_STRIDE))) =
            (uint32_t(unit_mask >> (15 + 19 - 13)) & 0xFFFFE000) | (x_stride - 1); // 16-bit elements!
    *((volatile uint32_t *) ((cluster << 8) + (uint32_t)(intptr_t)(&IDMA_EXT_BASE_ADDR_E2L))) = ext_base; // + trigger
}



// ***************************************************************************************************************
// VPRO Status/Control Register
// ***************************************************************************************************************
inline void __attribute__((always_inline)) vpro_wait_busy(uint32_t cluster_mask, uint32_t unit_mask) {
    // wait for CDC of busy signal
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
    for (int cluster = 0; cluster < HW.CLUSTERS; ++cluster) {
        if (((cluster_mask >> cluster) & 0x1u) == 0)
            continue;
        volatile uint32_t *status_reg = (volatile uint32_t *) ((cluster << 8) + (uint32_t)(intptr_t)(&VCP_BUSY_BASE));
        while ((*status_reg & unit_mask) != 0) {
            continue;
        }
    }
}

inline void __attribute__((always_inline)) vpro_set_cluster_mask(uint32_t idmask) {
    VCP_CLUSTER_MASK = idmask;
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
    // required since IO transfer (the idmask) is sent in the processors MEM stage while
    // vector instructions are issued in the ID stage (use NOPS to make sure idmask is set
    // when the vector instruction is issued)
}

inline void __attribute__((always_inline)) vpro_set_unit_mask(uint32_t idmask) {
    VCP_UNIT_MASK = idmask;
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
    // required since IO transfer (the idmask) is sent in the processors MEM stage while
    // vector instructions are issued in the ID stage (use NOPS to make sure idmask is set
    // when the vector instruction is issued)
}

inline void __attribute__((always_inline)) vpro_pipeline_wait(void) {

    // do something that has no effect
//    __builtin_vpro_instruction_word(L0_1, NONBLOCKING, NO_CHAIN, FUNC_ADD, NO_FLAG_UPDATE, DST_ADDR(0, 0, 0),
//                                    SRC1_ADDR(0, 0, 0), SRC2_IMM(0), 10, 0);

}

inline void __attribute__((always_inline)) vpro_set_idmask(uint32_t idmask) {  // backward compatibility
    vpro_set_cluster_mask(idmask);
}

// LOOPER
inline void __attribute__((always_inline)) vpro_loop_start(uint32_t start, uint32_t end, uint32_t increment_step){
    VPRO_LOOP_START_REG_1 = (end << 16u) | increment_step;
}

inline void __attribute__((always_inline)) vpro_loop_end(){
    VPRO_LOOP_END = 1;
    asm volatile (".set noat\r\n" "nop");   // wait for looper busy command to pass through to mips
    asm volatile (".set noat\r\n" "nop");
    asm volatile (".set noat\r\n" "nop");
    asm volatile (".set noat\r\n" "nop");
    asm volatile (".set noat\r\n" "nop");
    asm volatile (".set noat\r\n" "nop");
    asm volatile (".set noat\r\n" "nop");
    asm volatile (".set noat\r\n" "nop");
//    vpro_wait_busy(0xffffffff, 0xffffffff);
}

inline void __attribute__((always_inline)) vpro_loop_mask(uint32_t src1_mask, uint32_t src2_mask, uint32_t dst_mask){
    VPRO_LOOP_MASK = (src1_mask << 20u) | (src2_mask << 10u) | dst_mask;
}

// variable VPRO Cmds
constexpr uint32_t build_IO_0(uint32_t src1sel, uint32_t src1, uint32_t xend, uint32_t blocking, uint32_t chaining){
    assert (xend <= 0x3f);
    return (src1sel << 30) | (src1 << 8) | (xend << 2) | (blocking << 1) | chaining;
}
constexpr uint32_t build_IO_1(uint32_t src2sel, uint32_t src2, uint32_t yend, uint32_t flagu){
    assert (yend <= 0x3f);
    return (src2sel << 30) | (src2 << 8) | (yend << 2) | (flagu << 1);
}
constexpr uint32_t build_IO_2(uint32_t dst, uint32_t func_sel, uint32_t func, uint32_t id){
    assert (id <= 0xf);
    return (dst << 10) | (func_sel << (4+4)) | (func << 4) | id ;
}
inline void __attribute__((always_inline)) __vpro(uint32_t id, uint32_t blocking, uint32_t is_chain,
                                                  uint32_t fu_sel, uint32_t func, uint32_t flag_update,
                                                  uint32_t dst, uint32_t src1_sel, uint32_t src1, uint32_t src2_sel, uint32_t src2,
                                                  uint32_t x_end, uint32_t y_end) {
    VPRO_CMD_REGISTER_0 = build_IO_0(src1_sel, src1, x_end, blocking, is_chain);
    VPRO_CMD_REGISTER_1 = build_IO_1(src2_sel, src2, y_end, flag_update);
    VPRO_CMD_REGISTER_2 = build_IO_2(dst, fu_sel, func, id);
//    asm volatile (".set noat\r\n" "nop");   // to handle WB (io write) stage to ID (__builtin_vpro) cycle difference
//    asm volatile (".set noat\r\n" "nop");
    // VPRO_CMD_REGISTER_2 is trigger!
//    __builtin_vpro_instruction_word(0, NONBLOCKING, NO_CHAIN, FUNC_VARIABLE_VPRO_INSTRUCTION, NO_FLAG_UPDATE, DST_ADDR(0, 0, 0), SRC1_IMM(0), SRC2_IMM(0), 0, 0);
}

inline void __attribute__((always_inline)) __load_shift_left(uint32_t id, uint32_t lm_immediate, uint32_t offset, uint32_t alpha, uint32_t beta, uint32_t shift_factor, uint32_t x_end, uint32_t y_end){
    __vpro( id, NONBLOCKING, IS_CHAIN,
          FUNC_LOAD_SHIFT_LEFT, FLAG_UPDATE,
          DST_ADDR(shift_factor, 0, 0), 
          SRC1_ADDR(offset, alpha, beta), 
          SRC2_IMM(lm_immediate),
          x_end, y_end);
}

inline void __attribute__((always_inline)) __load_shift_right(uint32_t id, uint32_t lm_immediate, uint32_t offset, uint32_t alpha, uint32_t beta, uint32_t shift_factor, uint32_t x_end, uint32_t y_end){
    __vpro( id, NONBLOCKING, IS_CHAIN,
          FUNC_LOAD_SHIFT_RIGHT, FLAG_UPDATE,
          DST_ADDR(shift_factor, 0, 0), 
          SRC1_ADDR(offset, alpha, beta), 
          SRC2_IMM(lm_immediate),
          x_end, y_end);
}

inline void __attribute__((always_inline)) vpro_mac_h_bit_shift(uint32_t shift) {
    VCP_MAC_SHIFT_REG = shift;
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
    // required since IO transfer (the idmask) is sent in the processors MEM stage while
    // vector instructions are issued in the ID stage (use NOPS to make sure idmask is set
//    // when the vector instruction is issued)
}

inline void __attribute__((always_inline)) vpro_mul_h_bit_shift(uint32_t shift) {
    VCP_MUL_SHIFT_REG = shift;
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
    // required since IO transfer (the idmask) is sent in the processors MEM stage while
    // vector instructions are issued in the ID stage (use NOPS to make sure idmask is set
    // when the vector instruction is issued)
}
    
// ***************************************************************************************************************
// Simulator dummies 
// ***************************************************************************************************************
inline void __attribute__((always_inline)) sim_finish_and_wait_step(int clusters){}
inline void __attribute__((always_inline)) sim_wait_step(void){}
inline void __attribute__((always_inline)) sim_wait_step(bool finish = false, const char* msg = ""){}

inline int __attribute__((always_inline)) sim_init(int (*main_fkt)(int, char**), int argc, char* argv[]){return 0;}
inline int __attribute__((always_inline)) sim_init(int (*main_fkt)(int, char**), int &argc, char* argv[], int main_memory_size = 1024*1024*1024, int local_memory_size = 8192, int register_file_size = 1024, int cluster_count = 1, int vector_unit_count = 1, int vector_lane_count = 2, int pipeline_depth = 10){return 0;}
inline void __attribute__((always_inline)) sim_start(void){}
inline void __attribute__((always_inline)) sim_stop(void){}

inline int __attribute__((always_inline)) sim_printf(const char *format, ...){return 0;}
//inline void __attribute__((always_inline)) sim_printf(const char *format){}
template<typename... Args> void __attribute__((always_inline)) sim_printf(const char *format, Args... args){}

inline void __attribute__((always_inline)) sim_dump_local_memory(uint32_t cluster, uint32_t unit){}
inline void __attribute__((always_inline)) sim_dump_register_file(uint32_t cluster, uint32_t unit, uint32_t lane){}
inline void __attribute__((always_inline)) sim_dump_queue(uint32_t cluster, uint32_t unit){}

#endif // vpro_aux_h

