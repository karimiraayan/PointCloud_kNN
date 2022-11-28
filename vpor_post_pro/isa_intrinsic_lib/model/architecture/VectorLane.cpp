//
// Created by gesper on 06.03.19.
//

#include <iostream>
#include <bitset>
#include <stdlib.h>
#include "VectorLane.h"
#include "Cluster.h"
#include "VectorUnit.h"
#include "stats/SimStat.h"
#include "../../simulator/helper/typeConversion.h"
#include "../../simulator/helper/debugHelper.h"
#include "../../simulator/simCore.h"
#include "../vpro/common/vpro_globals.h"

VectorLane::VectorLane(int id, int register_file_size, int pipeline_depth, int vector_lane_count) :
        register_file_size(register_file_size), pipeline_depth(pipeline_depth), vector_lane_count(vector_lane_count),
        vector_lane_id(id),
        left_lane(this), right_lane(this), vector_unit(nullptr), ls_lane(this),
        clock_cycle(0),
        blocking(false), blocking_nxt(false),
        src_lane_stall(false), src_lane_stall_nxt(false),
        dst_lane_stall(false), dst_lane_stall_nxt(false),
        lane_chaining(false), lane_chaining_nxt(false),
        dst_lane_ready(false),
        dst_lanes(0b0),
        dst_units(0b0),
        chain_data_nxt(0), chain_data(0),
        //chain_DSTisRdy(false), chain_DSTisRdy_nxt(false),
        flag(new bool[2]), flag_nxt(new bool[2]()), rf_data(nullptr), rf_flag{nullptr, nullptr},
        stat(LaneStat()), updated(false),
        pipelineALUDepth(3),
        accu(0),
        consecutive_DST_stall_counter(0),
        consecutive_SRC_stall_counter(0) {
    current_cmd = std::make_shared<CommandVPRO>();
    new_cmd = std::make_shared<CommandVPRO>();
    noneCmd = std::make_shared<CommandVPRO>();
    pipeline = QVector<PipelineDate>(5 + CommandVPRO::MAX_ALU_DEPTH);
}

VectorLane::VectorLane(int id, VectorUnit *vector_unit, int register_file_size, int pipeline_depth,
                       int vector_lane_count) :
        VectorLane(id, register_file_size, pipeline_depth, vector_lane_count) {
    //printf("SIM: \t\tInstanziated new Vector-Lane (lane: %i)\n", vector_lane_id);

    this->vector_unit = vector_unit;

    // interpretated as 24-bit segments
    rf_data = new uint8_t[register_file_size * 3]();
    rf_flag[0] = new bool[register_file_size]();
    rf_flag[1] = new bool[register_file_size]();
};


/**
 * on accessing data from the register file. checks adress
 * @param addr rf-address
 * @param size [optional] = 3 for 24-bit
 * @return uint8_t[3] data
 */
uint32_t VectorLane::get_rf_data(int addr, int size) {
    // check if this accesses a valid register file
    if (addr * 3 + size > 3 * register_file_size) {
        printf_error("[Get] Access Register File out of range! (lane: %i, addr: %d, access size: %d)\n", vector_lane_id,
                     addr,
                     size);
        return 0;
    }
    if (size != 3) {
        printf_error("RF Read for size != 2 [NOT IMPLEMENTED!]\n");
    }

    return (*((uint32_t *) &rf_data[addr * 3])) & 0x00ffffffu;
}

/**
 * on setting data from the register file. checks adress
 * @param addr rf-address
 * @param data uint8_t[3] data
 * @param size [optional] = 3 for 24-bit
 */
void VectorLane::set_rf_data(int addr, uint32_t data, int size) {
    // check if this accesses a valid register file
    if (addr * 3 + size > 3 * register_file_size) {
        printf_error("[Set] Access Register File out of range! (lane: %i, addr: %d, access size: %d)\n", vector_lane_id,
                     addr,
                     size);
        return;
    }

    for (int i = 0; i < size; i++) {
        rf_data[addr * 3 + i] = uint8_t(data >> (i * 8));
    }
}

/**
 * checks if the current command is processed completely
 * @return bool
 */
bool VectorLane::isBusy() {
    bool pipeline_busy = false;
    for (int i = 0; i < pipeline_depth; i++) {
        pipeline_busy |= (pipeline[i].cmd->type != CommandVPRO::NONE);
    }

    return isBlocking() || pipeline_busy;
}

std::shared_ptr<CommandVPRO> VectorLane::getCmd() {
    return current_cmd;
}

bool VectorLane::isBlocking() {
    return blocking;
}

void VectorLane::resetAccu() {
    accu = 0;
}

void VectorLane::resetMinMax(uint32_t value) {
    minmax_value = value;
}


void VectorLane::check_stall_conditions(){
    constexpr int pipeline_src_read_stage = 3; // in next tick this will be 4 (RD stage
    src_lane_stall_nxt = false;
    dst_lane_stall_nxt = false;
    bool read_src1 = false;
    bool read_src2 = false;

    if (debug & DEBUG_LANE_STALLS){
        printf(ORANGE);
        printf("C%02iU%02i", vector_unit->cluster->cluster_id, vector_unit->vector_unit_id);
        printf((isLSLane()) ? "LS: " : "L%i: ", vector_lane_id);
    }

    addr_field_t src1 = pipeline[pipeline_src_read_stage].cmd->src1;
    if (debug & DEBUG_LANE_STALLS) printf("SRC1: ");
    if (is_src_chaining(src1)){
        if ((!get_lane_from_src(src1)->is_lane_chaining()) ||
            ((src1.delayed_chain) && !get_lane_from_src(src1)->is_dst_lane_ready())) {
            src_lane_stall_nxt = true;
            if (debug & DEBUG_LANE_STALLS) printf("IS STALL ");
        }
        else {
            read_src1 = true;
            if (debug & DEBUG_LANE_STALLS) printf("NO STALL ");
        }
    }
    else if (debug & DEBUG_LANE_STALLS) printf("NO CHAIN ");

    addr_field_t src2 = pipeline[pipeline_src_read_stage].cmd->src2;
    if (debug & DEBUG_LANE_STALLS) printf("SRC2: ");
    if (is_src_chaining(src2)){
        if ((!get_lane_from_src(src2)->is_lane_chaining()) ||
            ((src2.delayed_chain) && !get_lane_from_src(src2)->is_dst_lane_ready())) {
            src_lane_stall_nxt = true;
            if (debug & DEBUG_LANE_STALLS) printf("IS STALL ");
        }
        else {
            read_src2 = true;
            if (debug & DEBUG_LANE_STALLS) printf("NO STALL ");
        }
    }
    else if (debug & DEBUG_LANE_STALLS) printf("NO CHAIN ");

    if (this->is_lane_chaining()){
        if (!dst_lane_ready){
            if (debug & DEBUG_LANE_STALLS) printf("DST_STALL: DST LANE READY WAS NOT SET");
            dst_lane_stall_nxt = true;
        }
        else if (debug & DEBUG_LANE_STALLS) printf("DST_LANE_READY: CHAINING DATA");
    }
    else if (debug & DEBUG_LANE_STALLS)printf("NO CHAINING OUT");

    if (debug & DEBUG_LANE_STALLS)printf("  |  ");
    if (!src_lane_stall_nxt && !dst_lane_stall_nxt){
        if (read_src1){
            if (debug & DEBUG_LANE_STALLS){
                printf(ORANGE);
                printf("C%02iU%02i", vector_unit->cluster->cluster_id, vector_unit->vector_unit_id);
                printf((isLSLane()) ? "LS: " : "L%i: ", vector_lane_id);
                printf("SET DST LANE READY IN ");
                printf("C%02iU%02i", get_lane_from_src(src1)->vector_unit->cluster->cluster_id, get_lane_from_src(src1)->vector_unit->vector_unit_id);
                printf((get_lane_from_src(src1)->isLSLane()) ? "LS: " : "L%i: ", get_lane_from_src(src1)->vector_lane_id);
            }
            get_lane_from_src(src1)->set_dst_lane_ready(0b1u << vector_lane_id, 0b1u << vector_unit->vector_unit_id);
        }
        if (read_src2){
            if (debug & DEBUG_LANE_STALLS){
                printf(ORANGE);
                printf("C%02iU%02i", vector_unit->cluster->cluster_id, vector_unit->vector_unit_id);
                printf((isLSLane()) ? "LS: " : "L%i: ", vector_lane_id);
                printf("SET DST LANE READY IN ");
                printf("C%02iU%02i", get_lane_from_src(src2)->vector_unit->cluster->cluster_id, get_lane_from_src(src2)->vector_unit->vector_unit_id);
                printf((get_lane_from_src(src2)->isLSLane()) ? "LS: " : "L%i: ", get_lane_from_src(src2)->vector_lane_id);
            }
            get_lane_from_src(src2)->set_dst_lane_ready(0b1u << vector_lane_id, 0b1u << vector_unit->vector_unit_id);
        }
    }

    if (debug & DEBUG_LANE_STALLS) printf("\n");
}

void VectorLane::update_stall_condition(){
    src_lane_stall  = src_lane_stall_nxt;
    dst_lane_stall  = dst_lane_stall_nxt;
}


/**
 * clock tick to process lanes command/pipeline
 */
void VectorLane::tick() {
    //*********************************************
    // Fetch Command
    //*********************************************

    if (current_cmd->is_done()) { // Get a new command if the "old" is done
        if (new_cmd->is_done()) {
            new_cmd = vector_unit->getNextCommandForLane(vector_lane_id);
            //checkCmdForLane(new_cmd);
//                printf_info("Got a New Command for Lane %i cause old one is DONE!\n", vector_lane_id);
//                new_cmd->print();
        } // if there is already a new cmd fetched...
        if (new_cmd->pipelineALUDepth < pipelineALUDepth) { // check whether to delay the start?
            if (debug & DEBUG_PIPELINELENGTH_CHANGES) {
                printf_info(
                        "Got a New Command (buffer) for Lane %i cause old one is DONE! [WAIT cause pipeline Length has changed!]\n",
                        vector_lane_id);
            }
            pipelineALUDepth--; // maybe next tick
            current_cmd = std::make_shared<CommandVPRO>();
            current_cmd->id_mask = 1u << uint(vector_lane_id);
        } else {
            pipelineALUDepth = new_cmd->pipelineALUDepth;
            current_cmd = new_cmd;
//                printf_info("Got a New Command (buffer) for Lane %i cause old one is DONE! [pipeline Length changed to %i!]\n", vector_lane_id, pipelineALUDepth);
        }
    }


    //*********************************************
    // Update Pipeline
    //*********************************************
    blocking_nxt = current_cmd->blocking;

    // any pipeline stage -3 as data is read in stage 4
    for (int stage = 0; stage <= 5 + pipelineALUDepth - 1 - (chain_target_stage + 1); stage++) {
        blocking_nxt |= pipeline[stage].cmd->blocking;
    }

    if (src_lane_stall)
        stat.incrementSrcStall();
    if (dst_lane_stall)
        stat.incrementDstStall();

    //*********************************************
    // Execute Pipeline
    //*********************************************
    if (!src_lane_stall && !dst_lane_stall) { // run all
        vector_unit->cluster->core->getSimStats().newCommandTick(current_cmd.get());
        stat.addExecutedCmdTick(current_cmd.get());
        processPipeline(current_cmd);
        tick_pipeline(0, 5 + pipelineALUDepth + 1);  // execute ALL pipeline stages
        //*********************************************
        // process X and Y
        //*********************************************
        current_cmd->x++;
        if (current_cmd->x > current_cmd->x_end) {
            current_cmd->x = 0;
            current_cmd->y++;
            if (current_cmd->y > current_cmd->y_end) {
                stat.addExecutedCmdQueue(current_cmd.get());
                current_cmd->done = true;
            }
        }
    } else if (src_lane_stall && !dst_lane_stall) { // run from src if SRC wait
        stat.addExecutedCmdTick(noneCmd.get());
        int from = chain_target_stage + 1; // 3+1=4 the "fifth" pipeline stage
        // process later half of pipeline that is not affected by missing input data of stalling source lane
        for (int i = pipeline_depth; i > from; i--) { // process half Pipeline
            pipeline[i] = pipeline[i - 1]; // shift each element and append new empty
        }
        PipelineDate pipe;
        pipe.cmd = std::make_shared<CommandVPRO>();
        // fill the empty pipeline stage with a none cmd
        pipeline[from] = pipe;
        tick_pipeline(from, 5 + pipelineALUDepth + 1);  // execute later pipeline stages
    } else if (src_lane_stall && dst_lane_stall) { // run nothing if both stall
        stat.addExecutedCmdTick(noneCmd.get());
    } else if (!src_lane_stall && dst_lane_stall) { // run nothing if DST stall
        stat.addExecutedCmdTick(noneCmd.get());
    }

    //*********************************************
    // Update Registers
    //*********************************************

    pipeline[5 + pipelineALUDepth].data = pipeline[5 + pipelineALUDepth].pre_data;
    chain_data_nxt = pipeline[5 + pipelineALUDepth].pre_data;
    lane_chaining_nxt = pipeline[5 + pipelineALUDepth].cmd->is_chain;
    flag_nxt = pipeline[5 + pipelineALUDepth].flag;

    if (!dst_lane_stall) {    // run later part! (all)
        pipeline[5 + pipelineALUDepth + 1].cmd->blocking = false;
    }

    clock_cycle++;
}

void VectorLane::update() {
    if (debug & DEBUG_TICK)
        printf(" > Tick Lane %i\n", vector_lane_id);

    if (dst_lane_stall) consecutive_DST_stall_counter++;
    else consecutive_DST_stall_counter = 0;
    if (src_lane_stall) consecutive_SRC_stall_counter++;
    else consecutive_SRC_stall_counter = 0;

    src_lane_stall = src_lane_stall_nxt;
    dst_lane_stall = dst_lane_stall_nxt;
    blocking = blocking_nxt;
    flag[0] = flag_nxt[0];
    flag[1] = flag_nxt[1];
    chain_data = chain_data_nxt;
    lane_chaining = lane_chaining_nxt;

    //*********************************************
    // Debug Print messages
    //*********************************************
    if (debug & DEBUG_INSTRUCTIONS) {
        printf("[Pipeline 0] (new?) Processed Instr.: \tUnit(%2i) Lane(%2i):", vector_unit->vector_unit_id,
               vector_lane_id);
        if (pipeline[0].cmd->type == CommandVPRO::NONE) {
            printf("\tCMD: NONE \n");
        } else {
            printf("\tCMD: ");
            print_cmd(pipeline[0].cmd.get());
        }
    }
    if (debug & DEBUG_PIPELINE) {
        if (this->vector_lane_id == 0 && vector_unit->vector_unit_id == 0 && vector_unit->cluster->cluster_id == 0)
        print_pipeline();
    }
    if (debug & DEBUG_PIPELINE_9) {
        printf(BLUE);
        printf("\t\t\tResult processed from pipeline[9]: %i\n", pipeline[9].data);
        if (!__is_zero(pipeline[9].data))
            dumpRegisterFile("\t\t");
        printf(RESET_COLOR);
    }
    if ((this->vector_lane_id == 0 || this->vector_lane_id == 2) && vector_unit->vector_unit_id == 0 && vector_unit->cluster->cluster_id == 0) // (vector_unit->vector_unit_id==0 || vector_unit->vector_unit_id==1)
    if (debug & DEBUG_INSTRUCTION_DATA) {   // PRINT operations DATA and Addresses
//            pipe.cmd->print();
        auto pipe = pipeline[pipelineALUDepth + 5];   //???
        if (this->isLSLane()) {
//            auto pipe = pipeline[5];
            if (pipe.cmd->type != CommandVPRO::NONE) {
                printf("\e[48;5;232m");
                printf(BLUE);
                printf("C%2i U%2i LS", vector_unit->cluster->cluster_id, vector_unit->vector_unit_id);
                printf("\t");
                pipe.cmd->print_type();
                int cmd_vector_pos = pipe.cmd->y * (pipe.cmd->x_end + 1) + pipe.cmd->x;
                int cmd_vector_length = (pipe.cmd->x_end + 1) * (pipe.cmd->y_end + 1) - 1;
                printf("%2i/%2i", cmd_vector_pos, cmd_vector_length);
                if(is_src_chaining(pipe.cmd->src1)) printf((get_lane_from_src(pipe.cmd->src1)->isLSLane()) ? "\t CH[  LS]=" : "\t CH[  L%i]=",
                                                           get_lane_from_src(pipe.cmd->src1)->vector_lane_id);
                else if(is_src_chaining(pipe.cmd->src2)) printf((get_lane_from_src(pipe.cmd->src2)->isLSLane()) ? "\t CH[  LS]=" : "\t CH[  L%i]=",
                                                                get_lane_from_src(pipe.cmd->src2)->vector_lane_id);
                else printf_info("\t LM[%4i]=", pipe.lm_addr);
                printf("\033[0m");
                if (pipe.data != 0)
                    printf(INVERTED);
                printf_info("%13i (0x%04x)", int(pipe.data), pipe.data);

                printf(RESET_COLOR"\n");
            }
        } else {
            if (pipe.cmd->type != CommandVPRO::NONE) {
                if(vector_lane_id == 0) printf("\e[48;5;235m");
                if(vector_lane_id == 1) printf("\e[48;5;238m");
                printf(BLUE);
                printf("C%2i U%2i L%i", vector_unit->cluster->cluster_id, vector_unit->vector_unit_id, vector_lane_id);
                printf("\t");
                pipe.cmd->print_type();
                int cmd_vector_pos = pipe.cmd->y * (pipe.cmd->x_end + 1) + pipe.cmd->x;
                int cmd_vector_length = (pipe.cmd->x_end + 1) * (pipe.cmd->y_end + 1) - 1;
                printf("%2i/%2i", cmd_vector_pos, cmd_vector_length);

                if(is_src_chaining(pipe.cmd->src1)) printf((get_lane_from_src(pipe.cmd->src1)->isLSLane()) ? "\t CH[  LS]=" : "\t CH[  L%i]=",
                                                           get_lane_from_src(pipe.cmd->src1)->vector_lane_id);
                else printf_info("\tOPA[%4i]=", (pipe.cmd->src1.offset + pipe.cmd->src1.alpha * pipe.cmd->x + pipe.cmd->src1.beta * pipe.cmd->y));
                if(vector_lane_id == 0) printf("\e[48;5;235m");
                if(vector_lane_id == 1) printf("\e[48;5;238m");
                printf_info("%9i (0x%08x)\t", int(pipe.opa), pipe.opa);
                if(vector_lane_id == 0) printf("\e[48;5;235m");
                if(vector_lane_id == 1) printf("\e[48;5;238m");

                if(is_src_chaining(pipe.cmd->src1)) printf((get_lane_from_src(pipe.cmd->src1)->isLSLane()) ? " CH[  LS]=" : " CH[  L%i]=",
                                                           get_lane_from_src(pipe.cmd->src1)->vector_lane_id);
                else printf_info("OPB[%4i]=", (pipe.cmd->src2.offset + pipe.cmd->src2.alpha * pipe.cmd->x + pipe.cmd->src2.beta * pipe.cmd->y));
                if(vector_lane_id == 0) printf("\e[48;5;235m");
                if(vector_lane_id == 1) printf("\e[48;5;238m");
                printf_info("%9i (0x%08x)\t", int(pipe.opb), pipe.opb);
                if(vector_lane_id == 0) printf("\e[48;5;235m");
                if(vector_lane_id == 1) printf("\e[48;5;238m");
                printf_info("Result=%9i, Data=%9i (0x%06x)", int(pipe.pre_data), int(pipe.data), pipe.data);
                printf("\033[0m");
                if (pipe.move)
                    printf(INVERTED);
                printf_info("\t=>RF%i[%4i] (move: %s)", vector_lane_id, pipe.rf_addr, (pipe.move ? "true" : "false"));
                //            dumpRegisterFile("\t");
                printf(RESET_COLOR"\n");
            }
        }
    }
    if (debug & DEBUG_LANE_STALLS && false) {
        if (src_lane_stall || dst_lane_stall) {
            printf("Lane %2i Stall @Clock: %li -> %li: ", vector_lane_id, clock_cycle - 1, clock_cycle);
            if (src_lane_stall)
                printf_info("Chain-SRC[not rdy] ");
            if (dst_lane_stall)
                printf_info("Chain-DST[not read]");
            printf("\n");
            for (auto lane : vector_unit->getLanes()) {
                lane->print_pipeline(" >> ");
            }
            printf_warning("\n[press Enter to continue...]\n");
            getchar();
        }
    }
    // 30 due to possible mips overheads, etc. maybe another chain is active before, this can have maximal length of x_end*y_end
    constexpr int MAX_STALL_CYCLES_IN_ROW = 30 + std::min(MAX_X_END * MAX_Y_END, 256);
    if (consecutive_DST_stall_counter >= MAX_STALL_CYCLES_IN_ROW || consecutive_SRC_stall_counter >= MAX_STALL_CYCLES_IN_ROW) {
        printf_info( "\n\tToo Many Stall Cycles (> %i consecutive ones)!! \n", MAX_STALL_CYCLES_IN_ROW);
        printf_info("\tCheck your CODE!\n\t-> Isolate this command and check if chaining works (correct src/dst). \n");
        printf_info("\tIf it works and still error, this could be caused by scheduled old cmds (e.g. previous commands still chaining due wrong vector length)\n\n");
        if (isLSLane())
            printf_info("This is Lane[LS]\n");
        else
            printf_info("This is Lane[%i]\n", vector_lane_id);

        if (consecutive_DST_stall_counter >= MAX_STALL_CYCLES_IN_ROW) {
            printf_info("Current Pipeline: \n");
            print_pipeline();
            printf_info("Chaining Command: \n");
            pipeline[5 + pipelineALUDepth].cmd->print();
            printf("\n");
            printf_error("Destination does not get read and causes stall! (Command %s in Pipeline Stage %i)\n",
                         pipeline[5 + pipelineALUDepth].cmd->get_type().toStdString().c_str(), 5 + pipelineALUDepth);
            getchar();
        }
        if (consecutive_SRC_stall_counter >= MAX_STALL_CYCLES_IN_ROW) {
            printf_info("Current Pipeline: \n\t");
            print_pipeline();
            printf_info("Chaining Command: \n\t");
            pipeline[chain_target_stage].cmd->print();
            printf("\n");
            printf_error("Stall is caused by no Data feeded on Source Read! (Command %s in Pipeline Stage %i)\n",
                         pipeline[chain_target_stage].cmd->get_type().toStdString().c_str(), chain_target_stage);
            getchar();
        }
    }
    if (checkForWARConflict){ // analyse pipeline for WAR conflict
        const int i = chain_target_stage;
        if (pipeline[i].cmd->type != CommandVPRO::NONE){
            QMap<int, QString> writtenRFs;
            for (int j = chain_target_stage+1; j <= 5 + pipelineALUDepth; ++j) {
                if (pipeline[j].cmd->isWriteRF())
                    writtenRFs[pipeline[j].cmd->dst.offset + pipeline[j].cmd->dst.alpha * pipeline[j].cmd->x + pipeline[j].cmd->dst.beta * pipeline[j].cmd->y] = pipeline[j].cmd->get_string();
            }
            if (pipeline[i].cmd->src1.sel == SRC_SEL_ADDR){
                int rf_addr1 = pipeline[i].cmd->src1.offset + pipeline[i].cmd->src1.alpha * pipeline[i].cmd->x + pipeline[i].cmd->src1.beta * pipeline[i].cmd->y;
                if (writtenRFs.count(rf_addr1) > 0){
                    printf_warning("Write to RF after Read (WAR conflict!?)\n");
                    printf_warning("Previous instruction \n\t(%s) \nwill write into RF[%i] after \n\t%s SRC1 Reads it, but is scheduled before!\n",
                                   writtenRFs[rf_addr1].toStdString().c_str(), rf_addr1, pipeline[i].cmd->get_string().toStdString().c_str());
                }
            }
            if (pipeline[i].cmd->src2.sel == SRC_SEL_ADDR){
                int rf_addr2 = pipeline[i].cmd->src2.offset + pipeline[i].cmd->src2.alpha * pipeline[i].cmd->x + pipeline[i].cmd->src2.beta * pipeline[i].cmd->y;
                if (writtenRFs.count(rf_addr2) > 0){
                    printf_warning("Write to RF after Read (WAR conflict!?)\n");
                    printf_warning("Previous instruction \n\t(%s) \nwill write into RF[%i] after \n\t%s SRC2 Reads it, but is scheduled before!\n",
                                   writtenRFs[rf_addr2].toStdString().c_str(), rf_addr2, pipeline[i].cmd->get_string().toStdString().c_str());
                }
            }
        }
    }

    // reset register
    chain_data_nxt = 0xca12dead;    // chaindead / no data in chain
    blocking_nxt = false;
    lane_chaining_nxt = false;
    dst_lane_ready = false;
    dst_lanes = 0b0;
    dst_units = 0b0;
    src_lane_stall_nxt = false;
    dst_lane_stall_nxt = false;
}

void VectorLane::tick_pipeline(int from, int until) {
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

        if (stage == 0) { // calc addresses

            // result address in rf for regular instructions
            pipe.rf_addr = pipe.cmd->dst.offset + pipe.cmd->dst.alpha * pipe.cmd->x + pipe.cmd->dst.beta * pipe.cmd->y;

        } else if (stage == 1) {

        } else if (stage == 2) {

        } else if (stage == 3) {

        } else if (stage == 4) { // read chain input, read rf <?>
            pipe.opa = get_operand(pipe.cmd->src1, pipe.cmd->x, pipe.cmd->y, pipe.cmd->x_end);
            pipe.opa = *__24to32signed(pipe.opa);
            pipe.opb = get_operand(pipe.cmd->src2, pipe.cmd->x, pipe.cmd->y, pipe.cmd->x_end);

            // only 18 bit for SRC2 if mac operation
            if (pipe.cmd->type == CommandVPRO::MULL || pipe.cmd->type == CommandVPRO::MULH ||
                pipe.cmd->type == CommandVPRO::MACL || pipe.cmd->type == CommandVPRO::MACH ||
                pipe.cmd->type == CommandVPRO::MACL_PRE || pipe.cmd->type == CommandVPRO::MACH_PRE ||
                pipe.cmd->type == CommandVPRO::MULL_NEG || pipe.cmd->type == CommandVPRO::MULH_NEG ||
                pipe.cmd->type == CommandVPRO::MULL_POS || pipe.cmd->type == CommandVPRO::MULH_POS) {
                auto tmp = *__24to32signed(pipe.opb);
                pipe.opb = *__18to32signed(pipe.opb);
                if (tmp != pipe.opb){
                    pipe.cmd->print();
                    printf_warning("MUL* only takes 18-bit for second operand (SRC2)/OPB. Loaded Data got cut... %i (soll) != %i (18-bit cut)\n", tmp, pipe.opb);
                }

            } else {
                pipe.opb = *__24to32signed(pipe.opb);
            }

            // update move flag to print correctly in execute stage // differs from HW!
            switch (pipe.cmd->type) {
                case CommandVPRO::MV_ZE:
                    pipe.move = get_z_flag(pipe.cmd->src1, pipe.cmd->x, pipe.cmd->y, pipe.cmd->x_end); // z ==1
                    break;
                case CommandVPRO::MV_NZ:
                    pipe.move = !(get_z_flag(pipe.cmd->src1, pipe.cmd->x, pipe.cmd->y, pipe.cmd->x_end)); // z == 0
                    break;
                case CommandVPRO::MV_MI:
                case CommandVPRO::MULL_NEG:
                case CommandVPRO::MULH_NEG:
                case CommandVPRO::SHIFT_AR_NEG:
                    pipe.move = get_n_flag(pipe.cmd->src1, pipe.cmd->x, pipe.cmd->y, pipe.cmd->x_end); // n == 1
                    break;
                case CommandVPRO::MV_PL:
                case CommandVPRO::MULL_POS:
                case CommandVPRO::MULH_POS:
                case CommandVPRO::SHIFT_AR_POS:
                    pipe.move = !(get_n_flag(pipe.cmd->src1, pipe.cmd->x, pipe.cmd->y, pipe.cmd->x_end)); // n == 0
                    break;
                default:
                    pipe.move = true;
                    break;
            }

        } else if (stage == 5) { // execute alu, write lm

            if (pipe.cmd->x == 0 && pipe.cmd->y == 0 &&
                (pipe.cmd->type == CommandVPRO::MACL_PRE || pipe.cmd->type == CommandVPRO::MACH_PRE)) {
                resetAccu(); // accu = 0
            }
            if (pipe.cmd->x == 0 && pipe.cmd->y == 0 &&
                (pipe.cmd->type == CommandVPRO::MIN_VECTOR || pipe.cmd->type == CommandVPRO::MAX_VECTOR)) {
                minmax_value = pipe.opa;
                minmax_index = pipe.cmd->src1.alpha * pipe.cmd->x + pipe.cmd->src1.beta * pipe.cmd->y;
            }

            execute_cmd(pipe);
            if (pipe.cmd->type == CommandVPRO::MULL_NEG || pipe.cmd->type == CommandVPRO::MULH_NEG || pipe.cmd->type == CommandVPRO::SHIFT_AR_NEG ||
                pipe.cmd->type == CommandVPRO::MULL_POS || pipe.cmd->type == CommandVPRO::MULH_POS || pipe.cmd->type == CommandVPRO::SHIFT_AR_POS)
                if (!pipe.move){
                    pipe.pre_data = pipe.opa;
                    pipe.move = true;   // only mv_x will not write RF, others do always but conditional take Operand a if condition fails
                }

            pipe.flag[0] = __is_zero(pipe.pre_data);
            pipe.flag[1] = __is_negative(pipe.pre_data);

        } else if (stage == 6) { // check conditions
            // already done by sim in stage 5
        }

        if (stage == 5 + pipelineALUDepth + 1) {
            if (pipe.cmd->isWriteRF()) {
                if (pipe.move) {
                    set_rf_data(pipe.rf_addr, pipe.data);

                    // Update FLAGs (two steps... to allow chaining)
                    if (pipe.cmd->flag_update) {
                        rf_flag[0][pipe.rf_addr] = pipe.flag[0];
                        rf_flag[1][pipe.rf_addr] = pipe.flag[1];
                    }
                }
            }
        } // stage wb
    } // stages
}

bool VectorLane::is_src_chaining(addr_field_t src) {
    switch (src.sel) {
        case SRC_SEL_CHAIN:
        case SRC_SEL_LS:
            return true;
        default:
            return false;
    }
}

VectorLane *VectorLane::get_lane_from_src(addr_field_t src) {
    switch (src.sel) {
        case SRC_SEL_CHAIN:
            if (src.chain_left)
                return getLeftNeighbor();
            else if (src.chain_right)
                return getRightNeighbor();
            else
                return vector_unit->getLanes().at(src.chain_id - 1);
        case SRC_SEL_LS:
            if (isLSLane()) { // for chaining between LS Lanes
                if (src.chain_right)
                    return vector_unit->getRightNeighbor()->getLSLane();
                if (src.chain_left)
                    return vector_unit->getLeftNeighbor()->getLSLane();
            }
            return getLSLane();
        default:
            return this;
    }
}

/**
 * to get SRC1 and SRC2 data this function resolves the source (Immediate, Left Lane Chain data, Right ...
 * or simple address to the register file)
 * @param src contains selection and immediate/alpha/beta for adress calc
 * @param x currents commands 'iterations'
 * @param y
 * @param x_end
 * @return uint8_t[3] array with data for specified operand
 */
uint32_t VectorLane::get_operand(addr_field_t src, uint32_t x, uint32_t y, uint32_t x_end) {
    uint32_t op_data = 0;
    switch (src.sel) {
        case SRC_SEL_ADDR:
            op_data = get_rf_data(src.offset + src.alpha * x + src.beta * y);
            break;
        case SRC_SEL_IMM:
            op_data = src.imm;
            if (uint8_t(src.imm >> 24) > 0) // cannot happen. immediate is cut to 20 bit in __vpro_instr...
                printf_warning("using immediate as operand which is more than 24 bit! upper 8 bit got ignored!\n");
            op_data = *__20to32signed(op_data);
            break;
        case SRC_SEL_CHAIN:
        case SRC_SEL_LS:
            op_data = get_lane_from_src(src)->get_pipeline_chain_data();
            if (debug & DEBUG_CHAINING ||
            !get_lane_from_src(src)->is_lane_chaining() ||
            !get_lane_from_src(src)->is_dst_lane_ready()){
                printf(ORANGE "C%02iU%02i", vector_unit->cluster->cluster_id, vector_unit->vector_unit_id);
                printf((isLSLane()) ? "LS " : "L%i ", vector_lane_id);
                printf("ACCESSES C%02iU%02i", get_lane_from_src(src)->vector_unit->getCluster()->cluster_id,
                       get_lane_from_src(src)->vector_unit->vector_unit_id);
                printf((get_lane_from_src(src)->isLSLane()) ? "LS" : "L%i", get_lane_from_src(src)->vector_lane_id);

                if(debug & DEBUG_CHAINING) printf(" - DATA: %i (0x%X)\n", get_lane_from_src(src)->chain_data, get_lane_from_src(src)->chain_data);
                if(!get_lane_from_src(src)->is_lane_chaining()) printf_warning(" - NO CHAINING CMD IN OUT STAGE\n");
                if(!get_lane_from_src(src)->is_dst_lane_ready()) printf_warning(" - DST LANE NOT READY\n");
            }
            break;
        default:
            printf_error("VPRO_SIM ERROR: Invalid source selection! (src_sel=%d)\n", src.sel);
            exit(1);
    }
    return op_data;
//    return opx & 0x00FFFFFF; // 24 of 32 bit
}

/**
 * after execution the result of the command is pushed to the pipeline to be able to be accessed from another lane
 * @param newElement Pipeline_data type of result
 */
void VectorLane::processPipeline(std::shared_ptr<CommandVPRO> newElement) {
    // shift each element and append new
    for (int i = pipeline_depth; i > 0; i--) {
        pipeline[i] = pipeline[i - 1];
    }
    PipelineDate pipe;
    pipe.cmd = std::make_shared<CommandVPRO>(newElement.get());
    pipeline[0] = pipe;
}

/**
 * if operand is chained, the result is inside the pipeline of another lane.
 * this function returns the result from that lane
 * @param atClockTick to match correct pipeline level (depends on the lanes current tick)
 * @return uint8_t[3] data inside pipeline
 */
uint32_t VectorLane::get_pipeline_chain_data() {
    return chain_data;
}

void VectorLane::print_pipeline(const QString prefix){
    printf(ORANGE);
    if(vector_unit->cluster->cluster_id==0 && vector_unit->vector_unit_id==0 && vector_lane_id==0)
        printf("PRINT PIPELINE AFTER_CYCLE: %5li   DEPTH: %i \n", clock_cycle, 5 + pipelineALUDepth + 1);
    printf("C%02iU%02i", vector_unit->cluster->cluster_id, vector_unit->vector_unit_id);
    if (isLSLane()) printf("LS: ");
    else printf("L%i: ", vector_lane_id);

    int num_stages = 5 + pipelineALUDepth + 1;
    for (int stage = 0; stage < num_stages; stage++) {
        printf("|");
        if (pipeline[stage].cmd->type == CommandVPRO::NONE) {
            printf(INVERTED);
            if(src_lane_stall || dst_lane_stall) printf(RESET_COLOR "\e[30m" "\e[100m");
            printf("    ");
            pipeline[stage].cmd->print_type();
            printf(RESET_COLOR "\e[49m");
            printf(ORANGE);
        } else {
            int cmd_vector_pos = pipeline[stage].cmd->y * (pipeline[stage].cmd->x_end + 1) + pipeline[stage].cmd->x;
            if(src_lane_stall || dst_lane_stall) printf("\e[100m");
            if(cmd_vector_pos == 0) printf("\e[44m");
            printf("%3i ", cmd_vector_pos);
            pipeline[stage].cmd->print_type();
            printf(ORANGE "\e[49m");
        }
    }
    printf("|");
//    printf("\t %s", (dstRdy_pre)?"pre DST RDY":"");
    printf("  %s", (lane_chaining) ? "DATA READY" : "\e[90mDATA READY\e[93m");
//    if (lane_chaining)
    printf((dst_lane_ready) ? " CHAIN TO " : "\e[90m CHAIN TO \e[93m");
    printf((dst_lanes & 0b001) ? "L0 " : "\e[90mL0\e[93m ");
    printf((dst_lanes & 0b010) ? "L1 " : "\e[90mL1\e[93m ");
    printf((dst_lanes & 0b100) ? "LS " : "\e[90mLS\e[93m ");
    //printf("UNITS:%s", std::bitset<16> (dst_units).to_string().c_str());
    printf("IN UNITS:");
    for(int bit = (sizeof(dst_units) * 8) - 1; bit >= 0; bit--){
        if((dst_units >> bit) & 0b1) printf("1");
        else printf("\e[90m0\e[93m");
    }

//printf("%s", (isDSTchainRdy(clock_cycle)? "\tDST Rdy" : ""));
    printf("%s", (isBlocking() ? "\tBLOCKING" : ""));
    if (dst_lane_stall)
        printf("  STALL_DST");
    if (src_lane_stall)
        printf("  STALL_SRC ");
    printf("\n");
    printf(RESET_COLOR);
    if(vector_unit->cluster->cluster_id==HW.CLUSTERS-1 && vector_unit->vector_unit_id==HW.UNITS-1 && vector_lane_id==HW.LANES) printf("\n");
}

/**
 * if operand is chained, the flags is inside the pipeline of another lane.
 * this function returns the flags from that lane
 * @param atClockTick to match correct pipeline level (depends on the lanes current tick)
 * @return bool[2] flags inside pipeline
 */
bool *VectorLane::getPipelineForwardFlags() {
    if (debug & DEBUG_CHAINING) {
        printf_info("Accessing chain FLAGS (accesses on lane %i)", vector_lane_id);
        printf_info(" zero: %s, neg: %s\n", flag[0] ? "true" : "false", flag[1] ? "true" : "false");
    }

    return flag;
}

/**
 * this is the ALU
 * the execution of the command depends on function-selection
 * @param Pipeline item to be processed
 * @return
 */
void VectorLane::execute_cmd(PipelineDate &pipe) {

    uint32_t &res = pipe.res;
    pipe.pre_data = 0;
    uint64_t res_mul;
    float res_div;  //0..22|23..30|31 == Mantisse|Exponent|Vorzeichen
    uint32_t rf_addr_src1;

    switch (pipe.cmd->type) {
        case CommandVPRO::ADD:
            res = pipe.opa + pipe.opb;
            pipe.pre_data = res;
            break;
        case CommandVPRO::SUB:
            res = pipe.opb - pipe.opa;
            pipe.pre_data = res;
            break;
        case CommandVPRO::MULL_NEG:
        case CommandVPRO::MULL_POS:
        case CommandVPRO::MULL:         // (!) OPB is 18-bit long. (A + B inside uint32 with sign extension/int32)
            res_mul = (uint64_t) (((int64_t) (int32_t) pipe.opa) * ((int64_t) (int32_t) pipe.opb));
            accu = res_mul;
            res = (uint32_t) res_mul;
            pipe.pre_data = res;
            break;
        case CommandVPRO::MULH_NEG:
        case CommandVPRO::MULH_POS:
        case CommandVPRO::MULH:         // (!) OPB is 18-bit long. (A + B inside uint32 with sign extension/int32)
            res_mul = (uint64_t) (((int64_t) (int32_t) pipe.opa) * ((int64_t) (int32_t) pipe.opb));
            accu = res_mul;
            res = (uint32_t) (res_mul >> (vector_unit->getCluster()->ACCU_MUL_HIGH_BIT_SHIFT));
            pipe.pre_data = res;
            break;
        case CommandVPRO::DIVL:         // only simulator!
            res_div = ((float) pipe.opa) / ((float) pipe.opb);
            for (int i = 0; i < 16; i++) {
                res_div *= 2;
            }
            res = (uint32_t) res_div;
            res = (((1 << 16) - 1) & res);
            pipe.pre_data = *__32to24(res);
            /*
            ... = 0
            for (32)
            res_div_frac_int << 1
            if float(0.667898765) > 
            | 1
            */
            break;
        case CommandVPRO::DIVH:         // only simulator!
            res = (uint32_t) ((float) pipe.opa) / ((float) pipe.opb);
            //accu = res_div;
            //res = (uint32_t) res_div;		
            pipe.pre_data = *__32to24(res);
            break;
        case CommandVPRO::MACL:         // (!) OPB is 18-bit long. (A + B inside uint32 with sign extension/int32)
            res_mul = (uint64_t) (((int64_t) (int32_t) pipe.opa) * ((int64_t) (int32_t) pipe.opb));
            accu += (res_mul & 0xffffffffffffLL);
            res = (uint32_t) (accu);
            pipe.pre_data = res;
//            printf("\nLane %i, MACL: ACCU : %li = %li, return: %i = %i\n", vector_lane_id, (int64_t) accu,
//                   (accu & 0x800000000000LL)? (int64_t) (accu | 0xffff000000000000LL) : (int64_t) accu,
//                   res, int32_t (res));
            break;
        case CommandVPRO::MACH:         // (!) OPB is 18-bit long. (A + B inside uint32 with sign extension/int32)
            res_mul = (uint64_t) (((int64_t) (int32_t) pipe.opa) * ((int64_t) (int32_t) pipe.opb));
            accu += (res_mul & 0xffffffffffffLL);  // 48-bit
            res = (uint32_t) (accu >> (vector_unit->getCluster()->ACCU_MAC_HIGH_BIT_SHIFT));
            pipe.pre_data = res;
//            printf("\nLane %i, MACH: ACCU : %li = %li, return: %i = %i\n", vector_lane_id, (int64_t) accu,
//                   (accu & 0x800000000000LL)? (int64_t) (accu | 0xffff000000000000LL) : (int64_t) accu,
//                   res, int32_t (res));
            break;
        case CommandVPRO::MACL_PRE:         // (!) OPB is 18-bit long. (A + B inside uint32 with sign extension/int32)
            res_mul = (uint64_t) (((int64_t) (int32_t) pipe.opa) * ((int64_t) (int32_t) pipe.opb));
            accu += (res_mul & 0xffffffffffffLL);  // 48-bit
            res = (uint32_t) (accu);
//            printf("\nLane %i, MACL_PRE: ACCU : %li = %li, return: %i = %i\n", vector_lane_id, (int64_t) accu,
//                    (accu & 0x800000000000LL)? (int64_t) (accu | 0xffff000000000000LL) : (int64_t) accu,
//                    res, int32_t (res));
            pipe.pre_data = res;
            break;
        case CommandVPRO::MACH_PRE:         // (!) OPB is 18-bit long. (A + B inside uint32 with sign extension/int32)
            res_mul = (uint64_t) (((int64_t) (int32_t) pipe.opa) * ((int64_t) (int32_t) pipe.opb));
            accu += (res_mul & 0xffffffffffffLL);  // 48-bit
            res = (uint32_t) (accu >> (vector_unit->getCluster()->ACCU_MAC_HIGH_BIT_SHIFT));
//            printf("\nLane %i, MACL_PRE: ACCU : %li = %li, return: %i = %i\n", vector_lane_id, (int64_t) accu,
//                    (accu & 0x800000000000LL)? (int64_t) (accu | 0xffff000000000000LL) : (int64_t) accu,
//                   res, int32_t (res));
            pipe.pre_data = res;
            break;
        case CommandVPRO::XOR:
            res = pipe.opa ^ pipe.opb;
            pipe.pre_data = res;
            break;
        case CommandVPRO::XNOR:
            res = ~(pipe.opa ^ pipe.opb);
            pipe.pre_data = res;
            break;
        case CommandVPRO::AND:
            res = pipe.opa & pipe.opb;
            pipe.pre_data = res;
            break;
        case CommandVPRO::ANDN:
            res = pipe.opa & (~pipe.opb);
            pipe.pre_data = res;
            break;
        case CommandVPRO::NAND:
            res = ~(pipe.opa & pipe.opb);
            pipe.pre_data = res;
            break;
        case CommandVPRO::OR:
            res = pipe.opa | pipe.opb;
            pipe.pre_data = res;
            break;
        case CommandVPRO::ORN:
            res = pipe.opa | (~pipe.opb);
            pipe.pre_data = res;
            break;
        case CommandVPRO::NOR:
            res = ~(pipe.opa | pipe.opb);
            pipe.pre_data = res;
            break;
        case CommandVPRO::SHIFT_LL:
            if (vector_unit->cluster->core->SIM_ONLY_WARNING_PRINTED_ONCE_SHIFT_LL) {
                std::cout << ORANGE << "\n--------------------------------Shift_LL encounter-----------------------\n\n"
                          <<
                          "This program will only work in simulation. For VPRO emulation/HW-system you'll have to remove Shift_LL.\n"
                          <<
                          "Use MUL instead ;-) MUL with 2**x instead of << x!\n" <<
                          "This message will not appear again in this simulation run. \n" <<
                          "Do you want to continue (type in: 'y'/'n')?\n>" << RESET_COLOR;
                char ans;
                std::cin >> ans;
                if ((ans != 'Y') && (ans != 'y')) {
                    exit(EXIT_FAILURE); // will continue?  with main!?
                    std::_Exit(EXIT_FAILURE);
                    exit(1);
                }
                vector_unit->cluster->core->SIM_ONLY_WARNING_PRINTED_ONCE_SHIFT_LL = false;
            }
            res = (*__32to24(pipe.opa) << uint(pipe.opb & uint(0x1f)));
            pipe.pre_data = res;
            break;
        case CommandVPRO::SHIFT_LR:
            res = (*__32to24(pipe.opa) >> uint(pipe.opb & uint(0x1f)));
            pipe.pre_data = res;
            break;
        case CommandVPRO::SHIFT_AR:
        case CommandVPRO::SHIFT_AR_NEG:
        case CommandVPRO::SHIFT_AR_POS:
            res = (uint32_t) (((int32_t) pipe.opa) >> uint(pipe.opb & uint(0x1f)));
            pipe.pre_data = res;
            break;
        case CommandVPRO::ABS:
            res = (pipe.opa & 0x80000000) ? -pipe.opa : pipe.opa;
            pipe.pre_data = *__32to24(res);
            break;
        case CommandVPRO::MIN:
            res = ((int32_t) pipe.opa < (int32_t) pipe.opb) ? pipe.opa : pipe.opb;
            pipe.pre_data = res;
            break;
        case CommandVPRO::MIN_VECTOR:
            // DST(komplex-addr / 1 register) SRC1(vector - komplex addr) SRC2(imm; @bit0: index?)
            // SRC2(1, -, -) == return index
            // SRC2(0, -, -) == return value
            if ((int32_t) pipe.opa < (int32_t) minmax_value) {
                minmax_value = pipe.opa;
                rf_addr_src1 = pipe.cmd->src2.alpha * pipe.cmd->x + pipe.cmd->src2.beta * pipe.cmd->y;
                minmax_index = rf_addr_src1;
            }
            if ((pipe.cmd->src2.offset & 0b1u) == 1u)
                res = minmax_index;
            else
                res = minmax_value;
            pipe.pre_data = res;
            break;
        case CommandVPRO::MAX:
            res = ((int32_t) pipe.opa > (int32_t) pipe.opb) ? pipe.opa : pipe.opb;
            pipe.pre_data = res;
            break;
        case CommandVPRO::MAX_VECTOR:
            // DST(komplex-addr / 1 register) SRC1(vector - komplex addr) SRC2(imm; @bit0: index?)
            // SRC2(1, -, -) == return index
            // SRC2(0, -, -) == return value
            if ((int32_t) pipe.opa > (int32_t) minmax_value) {
                minmax_value = pipe.opa;
                rf_addr_src1 = pipe.cmd->src1.alpha * pipe.cmd->x + pipe.cmd->src1.beta * pipe.cmd->y;
                minmax_index = rf_addr_src1;
            }
            if ((pipe.cmd->src2.offset & 0b1u) == 1u)
                res = minmax_index;
            else
                res = minmax_value;
            pipe.pre_data = res;
            break;
        case CommandVPRO::BIT_REVERSAL:
            res = 0;
            if (pipe.opb == 0)  // default if 0 bits should be reversed
                pipe.opb = 24;
            for (uint i = 0u; i < pipe.opb; i++)
                res |= ((pipe.opa >> i) & 0b1u) << (pipe.opb - 1 - i);
            pipe.pre_data = res;
            break;
        case CommandVPRO::MV_ZE:
        case CommandVPRO::MV_NZ:
        case CommandVPRO::MV_MI:
        case CommandVPRO::MV_PL:
            pipe.pre_data = *__32to24(pipe.opb);
            break;
        case CommandVPRO::NONE:
        case CommandVPRO::WAIT_BUSY:
        case CommandVPRO::PIPELINE_WAIT:
            break;
        case CommandVPRO::LOAD:
        case CommandVPRO::LOADS:
        case CommandVPRO::LOADB:
        case CommandVPRO::LOADBS:
        case CommandVPRO::STORE:
            printf_error(
                    "Memory instruction in Lanes ALU detected! <should not happen because Pipeline handles this earlier>!\n");
            break;
        case CommandVPRO::LOOP_START:
        case CommandVPRO::LOOP_END:
        case CommandVPRO::LOOP_MASK:
            printf_error(
                    "LOOP instruction in Lane detected! <should not happen because simulator handles this earlier; in the vector unit>!\n");
            break;
        default:
            printf_error("Lane Command Execution: Error in Command TYPE: ");
            print_cmd(pipe.cmd.get());
            printf("\n");
            break;
    }

    // convert to 24 bit (fill 1 if signed...)
    pipe.pre_data = *__32to24(pipe.pre_data);
    pipe.pre_data = *__24to32signed(pipe.pre_data);
}


bool VectorLane::get_rf_flag(int addr, int select) {
    if (addr > register_file_size) {
        printf_error("RF Flag Address access out of range; (addr: %i > %i)[Select; %i]", addr, register_file_size,
                     select);
        return false;
    }
    if (select > 1) {
        printf_error("RF Flag Select access out of range; (addr: %i > %i)[Select; %i]", addr, register_file_size,
                     select);
        return false;
    }
    return rf_flag[select][addr];
}


// ***********************************************************************
// Read Z-flag operand according to source selection
// ***********************************************************************
bool VectorLane::get_z_flag(addr_field_t src, uint32_t x, uint32_t y, uint32_t x_end) {
    bool z_flag = false;

    switch (src.sel) {
        case SRC_SEL_ADDR:
        case SRC_SEL_IMM:
            z_flag = get_rf_flag(src.offset + src.alpha * x + src.beta * y, 0);
            break;
        case SRC_SEL_CHAIN:
            if (src.chain_left)
                z_flag = getLeftNeighbor()->getPipelineForwardFlags()[0]; //get_rf_flag(x + y * (x_end + 1), 0);
            else if (src.chain_right)
                z_flag = getRightNeighbor()->getPipelineForwardFlags()[0]; //get_rf_flag(x + y * (x_end + 1), 0);
            else
                z_flag = vector_unit->getLanes().at(src.chain_id - 1)->getPipelineForwardFlags()[0];
            break;
        case SRC_SEL_LS:
            z_flag = getLSLane()->getPipelineForwardFlags()[0];
            break;
        default:
            printf_error("VPRO_SIM ERROR: Invalid source selection get_z_flag! (src_sel=%d)\n", src.sel);
            break;
    }
    return z_flag;
}


// ***********************************************************************
// Read N-flag operand according to source selection
// ***********************************************************************
bool VectorLane::get_n_flag(addr_field_t src, uint32_t x, uint32_t y, uint32_t x_end) {
    bool n_flag = false;

    switch (src.sel) {
        case SRC_SEL_ADDR:
        case SRC_SEL_IMM:
            n_flag = get_rf_flag(src.offset + src.alpha * x + src.beta * y, 1);
            break;
        case SRC_SEL_CHAIN:
            if (src.chain_left)
                n_flag = getLeftNeighbor()->getPipelineForwardFlags()[1]; //get_rf_flag(x + y * (x_end + 1), 1);
            else if (src.chain_right)
                n_flag = getRightNeighbor()->getPipelineForwardFlags()[1]; //get_rf_flag(x + y * (x_end + 1), 1);
            else
                n_flag = vector_unit->getLanes().at(src.chain_id - 1)->getPipelineForwardFlags()[1];
            break;
        case SRC_SEL_LS:
            n_flag = getLSLane()->getPipelineForwardFlags()[1];
            break;
        default:
            printf_error("VPRO_SIM ERROR: Invalid source selection get_n_flag! (src_sel=%d)\n", src.sel);
            break;
    }
    return n_flag;
}


/**
 * Sim function to dump content of this lanes whole register file
 */
void VectorLane::dumpRegisterFile(std::string prefix) {
    printf("%s", prefix.c_str());
    printf("DUMP Register File (Vector Lane %i):\n", vector_lane_id);
    printf(LGREEN);
    int printLast = 0;
    for (int r = 0; r < register_file_size * 3; r += 16 * 3) { // all elements in 16 element blocks
        bool hasData = false;
        for (int s = 0; s <= 15 * 3; s += 3) { // each of the 16 blocks
            auto data = ((uint32_t) (rf_data[r + s]) + (uint32_t) (rf_data[r + s + 1] << 8) +
                         (uint32_t) (rf_data[r + s + 2] << 16));
            hasData |= (data != 0);
        }
        if (hasData) {
            printLast = std::min(printLast + 1, 1);
            printf(LGREEN);
            printf("%s", prefix.c_str());
            printf("$%03x:  ", (r / 3) & (~(16 - 1)));
            for (int s = 0; s <= 15 * 3; s += 3) { // each of the 16 blocks
                auto data = ((uint32_t) (rf_data[r + s]) + (uint32_t) (rf_data[r + s + 1] << 8) +
                             (uint32_t) (rf_data[r + s + 2] << 16));
                if ((uint32_t(data >> 23u) & 1) == 1)
                    data |= 0xFF000000;

                if (data == 0) {
                    printf(LIGHT);
                    //printf("0x%06x", (uint32_t) data);
                    printf("%3i", (int32_t) data);
                    printf(NORMAL_);
                } else {
                    printf(LGREEN);
                    //printf("0x%06x", (uint32_t) data);
                    printf("%3i", (int32_t) data);
                }
                if (debug & DEBUG_DUMP_FLAGS) {
                    auto neg = rf_flag[1][(r + s) / 3];
                    auto zero = rf_flag[0][(r + s) / 3];
                    printf("[%s%s]", neg ? "-" : "+", zero ? "z" : "d");
                }
                printf(" ");
            }
            printf("\n");
        } else {
            if (printLast == 0) {
                printf("%s", prefix.c_str());
                printf("...\n");
            }
            printLast = std::max(printLast - 1, -1);
        }
    }
    printf(RESET_COLOR);
}
