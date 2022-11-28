//
// Created by gesper on 01.10.19.
//

#include <cmath>
#include <iostream>

#include "Cluster.h"
#include "VectorLaneLS.h"
#include "VectorUnit.h"
#include "../../simulator/helper/typeConversion.h"
#include "../../simulator/helper/debugHelper.h"
#include "../../simulator/simCore.h"


VectorLaneLS::VectorLaneLS(VectorUnit *vector_unit, int register_file_size, int pipeline_depth, int vector_lane_count) :
    VectorLane(log2(int(LS)), register_file_size, pipeline_depth, vector_lane_count)
{

    this->vector_unit = vector_unit;

    //printf_info("SIM: \t\tInstanziated new L/S Vector-Lane (lane: %i)\n", vector_lane_id);

    accu = -1;

    // interpretated as 24-bit segments
    rf_data = nullptr;
    rf_flag[0] = nullptr;
    rf_flag[1] = nullptr;
};

void VectorLaneLS::tick() {
    VectorLane::tick();
}
/**
 * clock tick to process lanes command/pipeline
 */
void VectorLaneLS::tick_pipeline(int from, int until) {
    for (int stage = from; stage <= until; stage++) {
        // Addressing Units (Stage 0..3) for address calc
        // Local Memory Address Computation, base address (stage 3),
        // Chaining Write Enable (stage 4)
        // ALU Input Operands (stage 4) e.g. chain, imm
        // ALU Input Operands (stage 5) (assign to alu) e.g., src1_rdata
        // Local Memory Interface (stage 5) e.g. is_load/store, addr, wdata
        // ALU (stages 5..8)
        // Condition Check (stage 6) e.g., z_v, n_v
        // Data Write-Back Buffer (stage 8) e.g., rf_write_data <= lm_data/alu_result  chain_data_o <= alu_result
        // Chaining Output (stage 9)
        //        chain_sync_o <= enable(4) and (not enable(5)) and cmd_reg_pipe(4)(cmd_is_chain_c); -- 3 cycles latency between sync and actual data!!
        // Register File (read: stage 3..5; write: stage 9)

        PipelineDate &pipe = pipeline[stage];
        if (pipe.cmd->type == CommandVPRO::NONE)
            continue;

        if (pipe.cmd->type != CommandVPRO::NONE && !pipe.cmd->isLS()) {
            printf_warning("LS Pipeline Tick got a NON-LS Cmd!");
        }

        if (stage == 0) { // calc addresses
                // result address in rf for regular instructions
                // for L/S
                if (pipe.cmd->type == CommandVPRO::STORE || pipe.cmd->type == CommandVPRO::STORE_SHIFT_LEFT || pipe.cmd->type == CommandVPRO::STORE_SHIFT_RIGHT)
                {
                    pipe.lm_addr = pipe.cmd->src2.imm + pipe.cmd->dst.offset + pipe.cmd->dst.alpha * pipe.cmd->x + pipe.cmd->dst.beta * pipe.cmd->y;
                }
                if (pipe.cmd->type == CommandVPRO::LOAD || pipe.cmd->type == CommandVPRO::LOADS || pipe.cmd->type == CommandVPRO::LOADB || pipe.cmd->type == CommandVPRO::LOADBS || pipe.cmd->type == CommandVPRO::LOADS_SHIFT_LEFT || pipe.cmd->type == CommandVPRO::LOADS_SHIFT_RIGHT )
                {
                    pipe.lm_addr = pipe.cmd->src2.imm + pipe.cmd->src1.offset + pipe.cmd->src1.alpha * pipe.cmd->x + pipe.cmd->src1.beta * pipe.cmd->y;
                }
                if (pipe.cmd->type == CommandVPRO::STORE_SHIFT_LEFT || pipe.cmd->type == CommandVPRO::STORE_SHIFT_RIGHT)
                {
                    if(pipe.cmd->src2.sel != SRC_SEL_IMM) printf_error("Shift in Store; only by immediate!\n");
                    pipe.opb = get_operand(pipe.cmd->src2, pipe.cmd->x, pipe.cmd->y, pipe.cmd->x_end);
                    pipe.opb = *__24to32signed(pipe.opb);
                }
                if(pipe.cmd->type == CommandVPRO::LOAD_REVERSE || pipe.cmd->type == CommandVPRO::STORE_REVERSE)
                {
//                    printf("d.imm%i -> 2imm%i + 1offs%i + 1a%i * x%i + 1b%i * y%i \n", pipe.cmd->dst.imm, pipe.cmd->src2.imm, pipe.cmd->src1.offset, pipe.cmd->src1.alpha, pipe.cmd->x, pipe.cmd->src1.beta, pipe.cmd->y);
//                    getchar();
                    if(pipe.cmd->dst.imm == 0b00) pipe.lm_addr = pipe.cmd->src2.imm + pipe.cmd->src1.offset + pipe.cmd->src1.alpha * pipe.cmd->x + pipe.cmd->src1.beta * pipe.cmd->y;
                    if(pipe.cmd->dst.imm == 0b01) pipe.lm_addr = pipe.cmd->src2.imm + pipe.cmd->src1.offset + pipe.cmd->src1.alpha * pipe.cmd->x - pipe.cmd->src1.beta * pipe.cmd->y;
                    if(pipe.cmd->dst.imm == 0b10) pipe.lm_addr = pipe.cmd->src2.imm + pipe.cmd->src1.offset - pipe.cmd->src1.alpha * pipe.cmd->x + pipe.cmd->src1.beta * pipe.cmd->y;
                    if(pipe.cmd->dst.imm == 0b11) pipe.lm_addr = pipe.cmd->src2.imm + pipe.cmd->src1.offset - pipe.cmd->src1.alpha * pipe.cmd->x - pipe.cmd->src1.beta * pipe.cmd->y;
                }

//                if (debug & DEBUG_INSTRUCTIONS) {
//                    if (pipe.cmd->type == CommandVPRO::NONE) {
//                        printf("New Instr.: \tUnit(%2i) Lane(%2i): NONE \n", vector_unit->vector_unit_id,
//                               vector_lane_id);
//                    } else {
//                        printf("New Instr.: \tUnit(%2i) Lane(%2i):\t", vector_unit->vector_unit_id, vector_lane_id);
//                        print_cmd(pipe.cmd.get());
//                    }
//                }
        }
        else if (stage == 1) {}

        else if (stage == 2) {}

        else if (stage == 3) {}

        else if (stage == 4) { // read chain input, read rf <?>
//                pipe.opa = get_operand(pipe.cmd->src1, pipe.cmd->x, pipe.cmd->y, pipe.cmd->x_end);
//                pipe.opa = *__24to32signed(pipe.opa);
//                pipe.opb = get_operand(pipe.cmd->src2, pipe.cmd->x, pipe.cmd->y, pipe.cmd->x_end);
//                pipe.opb = *__24to32signed(pipe.opb);

            if (pipe.cmd->type == CommandVPRO::STORE || pipe.cmd->type == CommandVPRO::STORE_SHIFT_LEFT ||
                pipe.cmd->type == CommandVPRO::STORE_SHIFT_RIGHT || pipe.cmd->type == CommandVPRO::STORE_REVERSE) {
                pipe.pre_data = get_operand(pipe.cmd->src1, pipe.cmd->x, pipe.cmd->y, pipe.cmd->x_end);
                pipe.pre_data = *__24to32signed(pipe.pre_data);
                pipe.pre_data = *__16to24(pipe.pre_data);
//                    if (debug & DEBUG_CHAINING){
//                        printf_info("\t STORE will use data: %i\n", pipe.data);
//                    }
            }
            if (pipe.cmd->type == CommandVPRO::STORE_SHIFT_LEFT){ // MUX of different shifted datas
                pipe.pre_data = uint32_t(int32_t(pipe.pre_data) << pipe.opb);
                pipe.pre_data = *__24to32signed(pipe.pre_data);
                pipe.pre_data = *__16to24(pipe.pre_data);
            }
            if (pipe.cmd->type == CommandVPRO::STORE_SHIFT_RIGHT){
                pipe.pre_data = uint32_t(int32_t(pipe.pre_data) >> pipe.opb);
                pipe.pre_data = *__24to32signed(pipe.pre_data);
                pipe.pre_data = *__16to24(pipe.pre_data);
            }

            // LOAD or Write: SRC1: Chain from other LS
            if (pipe.cmd->src1.chain_ls){
                if (pipe.cmd->src1.chain_left)
                    pipe.pre_data = vector_unit->getLeftNeighbor()->getLSLane()->get_pipeline_chain_data();
                else if (pipe.cmd->src1.chain_right)
                    pipe.pre_data = vector_unit->getRightNeighbor()->getLSLane()->get_pipeline_chain_data();
                else
                    printf_error("Chain from LS LOAD stage 4 gets data but no direction specified!");
            }
        } else if (stage == 5) { // execute alu, write lm

            // valid possibilities
                // STORE: SRC1: Data [reg, chain (L/LS)], BaseAddr: Src2: Imm,    Dst: Addr vector
                // LOAD:  SRC1: Addr vector,              BaseAddr: Src2: Imm,    Dst unused
                // LOAD:  SRC1: chain from LS (data source).        Src2: unused, DST: unused

            if (pipe.cmd->isWriteLM()){
                // STORE: SRC1: Data, Addr: Src2(Imm)+Dst(Addr)
                pipe.data = pipe.pre_data;

                if (pipe.cmd->src2.sel != SRC_SEL_IMM) {
                    printf_error(
                            "VPRO_SIM ERROR: Invalid SRC2 source for %s! (Needs to be IMMediate! cmd->src2.sel=%d)\n",
                            pipe.cmd->get_type().toStdString().c_str(), pipe.cmd->src2.sel);
                    exit(-1);
                }

            } else {
                // LOAD: SRC1: Addr, Src2: Imm
                if (pipe.cmd->src1.sel != SRC_SEL_ADDR && pipe.cmd->src1.sel != SRC_SEL_LS) {
                    printf_error(
                            "VPRO_SIM ERROR: Invalid SRC1 source for %s operation! (Needs to be Adress or Chaining source! cmd->src1.sel=%d)\n",
                            pipe.cmd->get_type().toStdString().c_str(), pipe.cmd->src1.sel);
                    exit(-1);
                }

                if (pipe.cmd->src1.sel == SRC_SEL_ADDR){
                    switch (pipe.cmd->type) {
                        case CommandVPRO::LOAD:
                        case CommandVPRO::LOADS:
                        case CommandVPRO::LOADS_SHIFT_LEFT:
                        case CommandVPRO::LOADS_SHIFT_RIGHT:
                        case CommandVPRO::LOADB:
                        case CommandVPRO::LOADBS:
                        case CommandVPRO::LOAD_REVERSE:
                            pipe.pre_data = vector_unit->getLocalMemoryData(pipe.lm_addr);
                        default:
                            break;
                    }
                } else {
//                    pre_data already received (chain from LS)
                }
            }

            switch (pipe.cmd->type) {
                case CommandVPRO::LOAD:
                    pipe.pre_data = *__16to24(pipe.pre_data);
                    pipe.pre_data = *__24to32signed(pipe.pre_data);
                    break;
                case CommandVPRO::LOADS:
                    pipe.pre_data = *__16to24signed(pipe.pre_data);
                    pipe.pre_data = *__24to32signed(pipe.pre_data);
                    break;
                case CommandVPRO::LOADS_SHIFT_LEFT:
                    pipe.pre_data = *__16to24signed(pipe.pre_data);
                    pipe.pre_data = *__24to32signed(pipe.pre_data);
                    pipe.pre_data = int32_t(pipe.pre_data) << pipe.cmd->dst.offset;
                    pipe.pre_data = *__24to32signed(pipe.pre_data);
                    break;
                case CommandVPRO::LOADS_SHIFT_RIGHT:
                    pipe.pre_data = *__16to24signed(pipe.pre_data);
                    pipe.pre_data = *__24to32signed(pipe.pre_data);
                    pipe.pre_data = int32_t(pipe.pre_data) >> pipe.cmd->dst.offset;
                    pipe.pre_data = *__24to32signed(pipe.pre_data);
                    break;
                case CommandVPRO::LOADB:
                    pipe.pre_data = *__8to24(pipe.pre_data);
                    pipe.pre_data = *__24to32signed(pipe.pre_data);
                    break;
                case CommandVPRO::LOADBS:
                    pipe.pre_data = *__8to24signed(pipe.pre_data);
                    pipe.pre_data = *__24to32signed(pipe.pre_data);
                    break;
                case CommandVPRO::LOAD_REVERSE:
                    pipe.pre_data = *__8to24signed(pipe.pre_data);
                    pipe.pre_data = *__24to32signed(pipe.pre_data);
                    break;

                case CommandVPRO::STORE:
                    vector_unit->writeLocalMemoryData(pipe.lm_addr, pipe.data);
                    pipe.pre_data = pipe.data;
                    break;
                case CommandVPRO::STORE_SHIFT_LEFT:
                    printf_error("VPRO_SIM ERROR: STORE_SHIFT_LEFT instruction not implemented \n");
                    break;
                case CommandVPRO::STORE_SHIFT_RIGHT:
                    printf_error("VPRO_SIM ERROR: STORE_SHIFT_RIGHT instruction not implemented \n");
                    break;
                case CommandVPRO::STORE_REVERSE:
                    printf_error("VPRO_SIM ERROR: STORE_REVERSE instruction not implemented \n");
                    break;
                default:
                    break;
            }
            pipe.flag[0] = __is_zero(pipe.pre_data);
            pipe.flag[1] = __is_negative(pipe.pre_data);
//            for LS chain data source:
//                    pipe.data = pipe.pre_data;
//                      take chained data as own result
//                    TODO: this is done in stage end-1, could happen earlier here to save some cycles on chain LS
//                     to LS if following command get stalled due to load delay from this Load
        }
    } // for stages
}

void VectorLaneLS::execute_cmd(PipelineDate &pipe) {
    // convert to 24 bit (fill 1 if signed...)
    pipe.data = *__32to24(pipe.data);
    pipe.data = *__24to32signed(pipe.data);

    printf_error("In L/S Lane, execute_cmd() should not be called ever! [But is!]\n");
}

bool VectorLaneLS::get_z_flag(addr_field_t src, uint32_t x, uint32_t y, uint32_t x_end) {
    printf_error("get_z_flag on L/S LANE called! No RF exist!");
    return false;
}

bool VectorLaneLS::get_n_flag(addr_field_t src, uint32_t x, uint32_t y, uint32_t x_end) {
    printf_error("get_n_flag on L/S LANE called! No RF exist!");
    return false;
}

bool VectorLaneLS::get_rf_flag(int, int) {
    printf_error("get_rf_flag on L/S LANE called! No RF exist!");
    return false;
}

uint8_t *VectorLaneLS::getregister() {
    printf_error("getregister on L/S LANE called! No RF exist!");
    return nullptr;
}

bool *VectorLaneLS::getzeroflag() {
    printf_error("getzeroflag on L/S LANE called! No RF exist!");
    return nullptr;
}

bool *VectorLaneLS::getnegativeflag() {
    printf_error("getnegativeflag on L/S LANE called! No RF exist!");
    return nullptr;
}

void VectorLaneLS::dumpRegisterFile(std::string prefix) {
    printf_error("dumpRegisterFile on L/S LANE called! No RF exist!");
}

uint32_t VectorLaneLS::get_rf_data(int addr, int size) {
    printf_error("get_rf_data on L/S LANE called! No RF exist!");
    return 0;
}

void VectorLaneLS::set_rf_data(int addr, uint32_t data, int size) {
    printf_error("set_rf_data on L/S LANE called! No RF exist!");
}
