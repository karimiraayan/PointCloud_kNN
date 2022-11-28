//
// Created by gesper on 24.07.19.
//

// ########################################################
// # VPRO instruction & system simulation library         #
// # Sven Gesper, IMS, Uni Hannover, 2019                 #
// ########################################################


#include "core_class_wrapper.h"
#include "model/commands/CommandDMA.h"
#include "model/commands/CommandVPRO.h"
#include "simulator/helper/debugHelper.h"
#include <core_dma_broadcast.hpp>

SimCore * core_ = new SimCore();



void __load_shift_left(uint32_t id, uint32_t lm_immediate, uint32_t offset, uint32_t alpha, uint32_t beta, uint32_t shift_factor, uint32_t x_end, uint32_t y_end){

    auto command = std::make_shared<CommandVPRO>();

    command->dst.offset = shift_factor & 0x3ffu;
    if (shift_factor != command->dst.offset){
        printf_error("__load_shift_left: OVERFLOW on shift_factor!\n");
    }

    command->src1.sel = SRC_SEL_ADDR;
    command->src1.beta = beta & 0x1fu;
    if (beta != command->src1.beta){
        printf_error("__load_shift_left: OVERFLOW on beta!\n");
    }
    command->src1.alpha = alpha & 0x1fu;
    if (alpha != command->src1.alpha){
        printf_error("__load_shift_left: OVERFLOW on alpha!\n");
    }
    command->src1.offset = offset & 0x3ffu;
    if (offset != command->src1.offset){
        printf_error("__load_shift_left: OVERFLOW on offset!\n");
    }

    command->src2.sel = SRC_SEL_IMM;
    command->src2.imm = lm_immediate & 0xfffffu;
    if (lm_immediate != command->src2.imm){
        printf_error("__load_shift_left: OVERFLOW on lm_immediate!\n");
    }

    command->is_chain = true;
    command->x_end = x_end;
    command->y_end = y_end;
    command->id_mask = id;
    command->fu_sel = CLASS_MEM;
    command->type = CommandVPRO::LOADS_SHIFT_LEFT;

    core_->run_vpro_instruction(command);
}

void __load_shift_right(uint32_t id, uint32_t lm_immediate, uint32_t offset, uint32_t alpha, uint32_t beta, uint32_t shift_factor, uint32_t x_end, uint32_t y_end){

    auto command = std::make_shared<CommandVPRO>();

    command->dst.offset = shift_factor & 0x3ffu;
    if (shift_factor != command->dst.offset){
        printf_error("__load_shift_right: OVERFLOW on shift_factor!\n");
    }

    command->src1.sel = SRC_SEL_ADDR;
    command->src1.beta = beta & 0x1fu;
    if (beta != command->src1.beta){
        printf_error("__load_shift_right: OVERFLOW on beta!\n");
    }
    command->src1.alpha = alpha & 0x1fu;
    if (alpha != command->src1.alpha){
        printf_error("__load_shift_right: OVERFLOW on alpha!\n");
    }
    command->src1.offset = offset & 0x3ffu;
    if (offset != command->src1.offset){
        printf_error("__load_shift_right: OVERFLOW on offset!\n");
    }

    command->src2.sel = SRC_SEL_IMM;
    command->src2.imm = lm_immediate & 0xfffffu;
    if (lm_immediate != command->src2.imm){
        printf_error("__load_shift_right: OVERFLOW on lm_immediate!\n");
    }

    command->is_chain = true;
    command->x_end = x_end;
    command->y_end = y_end;
    command->id_mask = id;
    command->fu_sel = CLASS_MEM;
    command->type = CommandVPRO::LOADS_SHIFT_RIGHT;


    core_->run_vpro_instruction(command);
}

void __load_reverse(uint32_t lm_immediate, uint32_t offset, int32_t alpha, int32_t beta, uint32_t x_end, uint32_t y_end){

    auto command = std::make_shared<CommandVPRO>();

    if(alpha < 0 && beta < 0) command->dst.imm = 0b11;
    else if(alpha < 0 && beta >= 0) command->dst.imm = 0b10;
    else if(alpha >= 0 && beta >= 0) command->dst.imm = 0b00;
    else  command->dst.imm = 0b01;


    command->src1.sel = SRC_SEL_ADDR;
    command->src1.beta = abs(beta) & 0x1fu;
    if (abs(beta) != command->src1.beta){
        printf_error("__load_reverse: OVERFLOW on beta!\n");
    }
    command->src1.alpha = abs(alpha) & 0x1fu;
    if (abs(alpha) != command->src1.alpha){
        printf_error("__load_reverse: OVERFLOW on alpha!\n");
    }
    command->src1.offset = offset & 0x3ffu;
    if (offset != command->src1.offset){
        printf_error("__load_reverse: OVERFLOW on offset!\n");
    }

    command->src2.sel = SRC_SEL_IMM;
    command->src2.imm = lm_immediate & 0xfffffu;
    if (lm_immediate != command->src2.imm){
        printf_error("__load_reverse: OVERFLOW on lm_immediate!\n");
    }

    command->is_chain = true;
    command->x_end = x_end;
    command->y_end = y_end;
    command->id_mask = LS;
    command->fu_sel = CLASS_MEM;
    command->type = CommandVPRO::LOAD_REVERSE;

    core_->run_vpro_instruction(command);
}


//todo this function does not work yet
//in load_reverse the sign of alpha and beta is encoded in the destination immediate, as it is never used
void __store_reverse(uint32_t id, uint32_t lm_immediate, uint32_t offset, int32_t alpha, int32_t beta, uint32_t x_end, uint32_t y_end, bool delayed_chain){

    auto command = std::make_shared<CommandVPRO>();

    command->dst.sel = SRC_SEL_ADDR; // must be ADDR!
    command->dst.alpha = alpha & 0x1fu;
    command->dst.beta = beta & 0x1fu;
    command->dst.offset = offset & 0x3ffu;
    if (beta != command->dst.beta){
        printf_error("__store_reverse: OVERFLOW on beta!\n");
    }
    if (alpha != command->dst.alpha){
        printf_error("__store_reverse: OVERFLOW on alpha!\n");
    }
    if (offset != command->dst.offset){
        printf_error("__store_reverse: OVERFLOW on offset!\n");
    }

    if(alpha < 0 && beta < 0) command->src1.imm = 0b11;
    else if(alpha < 0 && beta >= 0) command->src1.imm = 0b10;
    else if(alpha >= 0 && beta >= 0) command->src1.imm = 0b00;
    else  command->src1.imm = 0b01;
    command->src1.sel = SRC_SEL_CHAIN;
    command->src1.delayed_chain = delayed_chain;

    command->src2.sel = SRC_SEL_IMM;
    command->src2.imm = lm_immediate & 0xfffffu;
    if (lm_immediate != command->src2.imm){
        printf_error("__store_reverse: OVERFLOW on lm_immediate!\n");
    }

    command->is_chain = true;
    command->x_end = x_end;
    command->y_end = y_end;
    command->id_mask = LS;
    command->fu_sel = CLASS_MEM;
    command->type = CommandVPRO::STORE_REVERSE;

    core_->run_vpro_instruction(command);
}

void __find_max_vector(LANE id, uint32_t dst, uint32_t src1_sel, uint32_t src1, uint32_t x_end, uint32_t y_end,
                       bool find_index, bool chain, bool blocking, bool flag_update)
{
    auto command = std::make_shared<CommandVPRO>();

    command->dst.sel = SRC_SEL_ADDR; // must be ADDR!
    command->dst.imm = dst & 0xfffff;
    command->dst.beta = dst & 0x1f;
    command->dst.alpha = (dst >> 5) & 0x1f;
    command->dst.offset = (dst >> 10) & 0x3ff;

    command->src1.sel = (uint8_t) src1_sel;
    command->src1.imm = src1 & 0xfffff;
    command->src1.beta = src1 & 0x1f;
    command->src1.alpha = (src1 >> 5) & 0x1f;
    command->src1.offset = (src1 >> 10) & 0x3ff;
    if (src1_sel == SRC_SEL_CHAIN){
        command->src1.chain_id = 1 + (src1 & 0x7fff); // TODO chain id + 1 is ugly! use of src1_sel ? -> e.g. in Lanes forward,...
        command->src1.chain_left = ((src1 & 0x8000) == 0x8000);
        command->src1.chain_right = ((src1 & 0x8001) == 0x8001);
    } else if (src1_sel == SRC_SEL_LS){
        command->src1.chain_ls = true;
    }
    command->src1.delayed_chain = (command->src1.chain_left || command->src1.chain_right || command->src1.chain_ls) && (src1 & 0x0002) == 0x0002;

    command->src2.alpha = 1;
    command->src2.beta = x_end + 1;
    if(find_index) command->src2.offset = 1;
    else command->src2.offset = 0;

    command->is_chain = chain;
    command->x_end = x_end;
    command->y_end = y_end;
    command->id_mask = id;
    command->func = 0b1110;
    command->fu_sel = CLASS_SPECIAL;
    command->type = CommandVPRO::MAX_VECTOR;
    command->blocking = blocking;
    command->flag_update = flag_update;

    core_->run_vpro_instruction(command);
}

void __find_min_vector(LANE id, uint32_t dst, uint32_t src1_sel, uint32_t src1, uint32_t x_end, uint32_t y_end,
        bool find_index, bool chain, bool blocking, bool flag_update)
{
    auto command = std::make_shared<CommandVPRO>();

    command->dst.sel = SRC_SEL_ADDR; // must be ADDR!
    command->dst.imm = dst & 0xfffff;
    command->dst.beta = dst & 0x1f;
    command->dst.alpha = (dst >> 5) & 0x1f;
    command->dst.offset = (dst >> 10) & 0x3ff;

    command->src1.sel = (uint8_t) src1_sel;
    command->src1.imm = src1 & 0xfffff;
    command->src1.beta = src1 & 0x1f;
    command->src1.alpha = (src1 >> 6) & 0x1f;
    command->src1.offset = (src1 >> 12) & 0x3ff;
    if (src1_sel == SRC_SEL_CHAIN){
    command->src1.chain_id = 1 + (src1 & 0x7fff); // TODO chain id + 1 is ugly! use of src1_sel ? -> e.g. in Lanes forward,...
    command->src1.chain_left = ((src1 & 0x8000) == 0x8000);
    command->src1.chain_right = ((src1 & 0x8001) == 0x8001);
    } else if (src1_sel == SRC_SEL_LS){
    command->src1.chain_ls = true;
    }
    command->src1.delayed_chain = (command->src1.chain_left || command->src1.chain_right || command->src1.chain_ls) && (src1 & 0x0002) == 0x0002;

    command->src2.alpha = 1;
    command->src2.beta = x_end + 1;
    if(find_index) command->src2.offset = 1;
    else command->src2.offset = 0;

    command->is_chain = chain;
    command->x_end = x_end;
    command->y_end = y_end;
    command->id_mask = id;
    command->func = 0b1101;
    command->fu_sel = CLASS_SPECIAL;
    command->type = CommandVPRO::MIN_VECTOR;
    command->blocking = blocking;
    command->flag_update = flag_update;

    core_->run_vpro_instruction(command);
}


// *** VPRO VectorUnits/Lanes *** //
void __builtin_vpro_instruction_word(uint32_t id, uint32_t blocking, uint32_t is_chain, uint32_t fu_sel, uint32_t func, uint32_t flag_update, uint32_t dst, uint32_t src1_sel, uint32_t src1, uint32_t src2_sel, uint32_t src2, uint32_t x_end, uint32_t y_end){
    core_->__builtin_vpro_instruction_word(id, blocking, is_chain, fu_sel, func, flag_update, dst, src1_sel, src1, src2_sel, src2, x_end, y_end);
}
void __vpro(uint32_t id, uint32_t blocking, uint32_t is_chain, uint32_t fu_sel, uint32_t func, uint32_t flag_update, uint32_t dst, uint32_t src1_sel, uint32_t src1, uint32_t src2_sel, uint32_t src2, uint32_t x_end, uint32_t y_end){
    core_->__builtin_vpro_instruction_word(id, blocking, is_chain, fu_sel, func, flag_update, dst, src1_sel, src1, src2_sel, src2, x_end, y_end);
}
void vpro_wait_busy(uint32_t cluster_mask, uint32_t unit_mask, int cycle){
    printf("cycle:%i vpro_wait_busy cluster_mask:%u unit_mask: unit_mask:%u \n", cycle, cluster_mask, unit_mask);
    vpro_wait_busy(cluster_mask, unit_mask);
}
void vpro_wait_busy(uint32_t cluster_mask, uint32_t unit_mask){
    core_->vpro_wait_busy(cluster_mask, unit_mask);
}
void vpro_set_idmask(uint32_t idmask){
    core_->vpro_set_idmask(idmask);
}
void vpro_set_unit_mask(uint32_t idmask){
    core_->vpro_set_unit_mask(idmask);
}
uint32_t vpro_get_unit_mask(){
    return core_->get_unit_mask();
}

void vpro_set_cluster_mask(uint32_t idmask){
    core_->vpro_set_cluster_mask(idmask);
}
void vpro_pipeline_wait(){
    core_->vpro_pipeline_wait();
}


void vpro_loop_start(uint32_t start, uint32_t end, uint32_t increment_step){
    core_->vpro_loop_start(start, end, increment_step);
}
void vpro_loop_end(){
    core_->vpro_loop_end();
}
void vpro_loop_mask(uint32_t src1_mask, uint32_t src2_mask, uint32_t dst_mask){
    core_->vpro_loop_mask(src1_mask, src2_mask, dst_mask);
}
void vpro_mac_h_bit_shift(uint32_t shift){
    core_->vpro_mac_h_bit_shift(shift);
}
void vpro_mul_h_bit_shift(uint32_t shift){
    core_->vpro_mul_h_bit_shift(shift);
}

// *** VPRO DMA *** //
void dma_ext1D_to_loc1D(uint32_t cluster, uint64_t ext_base, uint32_t loc_base, uint32_t word_count, uint32_t x_stride, uint32_t x_size, uint32_t y_size, bool is_broadcast_no_checking){
    core_->dma_ext1D_to_loc1D(cluster, ext_base, loc_base, word_count, x_stride, x_size, y_size, is_broadcast_no_checking);
}
void dma_ext2D_to_loc1D(uint32_t cluster, uint64_t ext_base, uint32_t loc_base, int32_t x_stride, uint32_t x_size, uint32_t y_size, bool pad_flags[4], bool is_broadcast_no_checking){
    core_->dma_ext2D_to_loc1D(cluster, ext_base, loc_base, x_stride, x_size, y_size, pad_flags, is_broadcast_no_checking);
}
void dma_loc1D_to_ext1D(uint32_t cluster, uint64_t ext_base, uint32_t loc_base, uint32_t word_count){
    core_->dma_loc1D_to_ext1D(cluster, ext_base, loc_base, word_count);
}
void dma_loc1D_to_ext2D(uint32_t cluster, uint64_t ext_base, uint32_t loc_base, int32_t x_stride, uint32_t x_size, uint32_t y_size){
    core_->dma_loc1D_to_ext2D(cluster, ext_base, loc_base, x_stride, x_size, y_size);
}
void dma_wait_to_finish(uint32_t cluster_mask, int cycle){
    printf("cycle:%i dma_wait_to_finish %u \n", cycle, cluster_mask);
    dma_wait_to_finish(cluster_mask);
}
void dma_wait_to_finish(uint32_t cluster_mask){
    core_->dma_wait_to_finish(cluster_mask);
}
void dma_set_pad_widths(uint32_t top, uint32_t right, uint32_t bottom, uint32_t left){
    core_->dma_set_pad_widths(top, right, bottom, left);
}

void dma_set_pad_value(int16_t value){
    core_->dma_set_pad_value(value);
}

// *** Main Memory *** //
int  bin_file_send(uint32_t addr, int num_bytes, char const* file_name){
    return core_->bin_file_send(addr, num_bytes, file_name);
}
int  bin_file_dump(uint32_t addr, int num_bytes, char const* file_name){
    return core_->bin_file_dump(addr, num_bytes, file_name);
}
uint8_t* bin_file_return(uint32_t addr, int num_bytes){
    return core_->bin_file_return(addr, num_bytes);
}
uint16_t *get_main_memory(uint32_t addr, int num_elements_16bit){
    return core_->get_main_memory(addr, num_elements_16bit);
}
uint16_t *get_mm_element(uint32_t addr){
    return core_->get_main_memory(addr, 1);
}
uint8_t* bin_rf_return(uint32_t cluster, uint32_t unit, uint32_t lane){
    return core_->bin_rf_return(cluster, unit, lane);
}
uint8_t* bin_lm_return(uint32_t cluster, uint32_t unit){
    return core_->bin_lm_return(cluster, unit);
}
void aux_memset(uint32_t base_addr, uint8_t value, uint32_t num_bytes){
    core_->aux_memset(base_addr, value, num_bytes);
}

// *** System *** //
void aux_print_debugfifo(uint32_t data){
    core_->aux_print_debugfifo(data);
}
void aux_dev_null(uint32_t data){
    return core_->aux_dev_null(data);
}
void aux_flush_dcache(){
    core_->aux_flush_dcache();
}
void aux_wait_cycles(int cycles){

}

void aux_clr_dcache(){}
void aux_clr_icache(){}

void aux_clr_sys_time(){
    core_->aux_clr_sys_time();
}
void aux_clr_cycle_cnt(){
    core_->aux_clr_cycle_cnt();
}
uint32_t aux_get_cycle_cnt(){
    return core_->aux_get_cycle_cnt();
}
void aux_send_system_runtime(){
    core_->aux_send_system_runtime();
}
uint32_t aux_get_sys_time_lo(){
    return core_->aux_get_sys_time_lo();
}
uint32_t aux_get_sys_time_hi(){
    return core_->aux_get_sys_time_hi();
}

// *** Simulator debugging funtions ***
void sim_dump_local_memory(uint32_t cluster, uint32_t unit){
    core_->sim_dump_local_memory(cluster, unit);
}
void sim_dump_queue(uint32_t cluster, uint32_t unit){
    core_->sim_dump_queue(cluster, unit);
}
void sim_dump_register_file(uint32_t cluster, uint32_t unit, uint32_t lane){
    core_->sim_dump_register_file(cluster, unit, lane);
}

void sim_wait_step(bool finish, const char* msg){
    core_->sim_wait_step(finish, msg);
}

// *** General simulator environment control ***
int sim_init(int (*main_fkt)(int, char**), int argc, char* argv[]){
    return core_->sim_init(main_fkt, argc, argv);
}
int sim_init(int (*main_fkt)(int, char**), int &argc, char* argv[], uint32_t main_memory_size, uint32_t local_memory_size, uint32_t register_file_size, int cluster_count, int vector_unit_count, int vector_lane_count, int pipeline_depth){
    return core_->sim_init(main_fkt, argc, argv, main_memory_size, local_memory_size, register_file_size, cluster_count, vector_unit_count, vector_lane_count, pipeline_depth);
}

void sim_stop(){
    core_->sim_stop();
}

void sim_printf(const char *format){
    core_->sim_printf(format);
}
