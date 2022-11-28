//
// Created by gesper on 06.03.19.
//
#include <QString>
#include <thread>         // std::thread

#include "VectorUnit.h"
#include "Cluster.h"
#include "../../simulator/helper/debugHelper.h"
#include "../../simulator/helper/typeConversion.h"
#include "../../simulator/simCore.h"

VectorUnit::VectorUnit(int id, Cluster *cluster, long &clock, int local_memory_size, int vector_lane_count, int register_file_size, int pipeline_depth) :
        cluster(cluster),
        local_memory_size(local_memory_size),
        vector_lane_count(vector_lane_count),
        register_file_size(register_file_size),
        pipeline_depth(pipeline_depth),
        clock(clock){
    //printf("SIM: \tInstanziated new Vector-Unit (unit: %i)\n", id);
    vector_unit_id = id;

    // LM interpretated in 24-bit segments
    local_memory = new uint8_t[local_memory_size * (LOCAL_MEMORY_DATA_WIDTH / 8)]();

    // create regular Lanes
    for (int i = 0; i < vector_lane_count; i++) {
        lanes.push_back(new VectorLane(i, this, register_file_size, pipeline_depth, vector_lane_count));
    }
    // create LS Lane
    lanes.push_back(new VectorLaneLS(this, register_file_size, pipeline_depth, vector_lane_count));
    setLSLane(static_cast<VectorLaneLS *>(lanes.back()));

    // set neighbor info to all Lanes
    lanes[0]->setNeighbors(lanes[vector_lane_count - 1], lanes[1], lanes.back());
    for (int i = 1; i < vector_lane_count - 1; i++) {
        lanes[i]->setNeighbors(lanes[i - 1], lanes[i + 1], ls_lane);
    }
    lanes[vector_lane_count - 1]->setNeighbors(lanes[vector_lane_count - 2], lanes[0], ls_lane);
    lanes.back()->setNeighbors(lanes[0], lanes[vector_lane_count - 1], ls_lane);

    // init commands
    cmd_queue = QVector<std::shared_ptr<CommandVPRO>>();
    clearCommands();
    noneCmd = std::make_shared<CommandVPRO>();

    // init Loop Registers
    loopParsing = false;
    loopPushing = false;
    loop_index = 0;
    loop_fetch_index = 0;
    for (int i = 0 ; i < MAX_LOOP_CASCADES; i++){
        loop_register[i].value = 0;
        loop_register[i].end = 0;
        loop_register[i].incr_step = 0;
        loop_register[i].is_running = false;
        loop_register[i].start = 0;
        loop_register[i].start_position = 0;
    }

    loop_instruction_index = 0;
    loop_instruction_fetch_index = 0;
    for (int i = 0 ; i < MAX_LOOP_INSTRUCTIONS; i++){
        loop_instruction_register[i].command = noneCmd;
        loop_instruction_register[i].dst_flags = 0;
        loop_instruction_register[i].src1_flags = 0;
        loop_instruction_register[i].src2_flags = 0;
    }

#ifdef THREAD_LANE
        for (auto lane : lanes){
            lane->start();
        }
#endif

    cmdQueueFetchedCmd = false;
};


bool VectorUnit::isLaneSelected(CommandVPRO *cmd, long lane_id){

//    uint32_t dst_id = cmd->id_mask >> ld(vector_lane_count);
//    uint32_t dst_id_masked = dst_id & (~cluster->unit_mask_global);
//    uint32_t unit_id_masked = vector_unit_id & (~cluster->unit_mask_global);
//    // (dst_id_masked == unit_id_masked)

    if (long(lane_id) > vector_lane_count){ // this is L/S lane check
        return cmd->isLS();
    }

    return ((uint(cmd->id_mask) >> uint(lane_id)) & 1u) == 1;

    //return (lane_id == cmd->id_mask || (cmd->id_mask & lane_id) > 0 || cmd->id_mask == 0xffffffff);
}

bool VectorUnit::isCmdQueueFull(){
    return (cmd_queue.size() > UNITS_COMMAND_QUEUE_MAX_SIZE);
}
// cmds from cluster/sim push into units queue
bool VectorUnit::sendCMD(std::shared_ptr<CommandVPRO> cmd) {

    if (loopParsing && !loopPushing) {
        // mips is pushing while loop unit is still parsing
        // only one source of new cmds for units queue is possible
        // block mips

        printf_error("Unit got command but queue push is blocked by loop unit!\n");
        printf_error("Wait for loop unit to finish!!\n");
        return false;
    }

    if (cmd->type == CommandVPRO::LOOP_START){
        if (loop_index >= MAX_LOOP_CASCADES - 1){ // already in loop with #MAX_LOOP_CASCADE
            printf_error("more LOOPs started than allowed! maximum %i cascaded Loops supported! \n", MAX_LOOP_CASCADES);
            return false;
        }
        loopParsing = true;

        if (loopPushing){
            loop_index++;
        }else{
            loopPushing = true;
        }

        loop_instruction_register[loop_instruction_index].command = std::make_shared<CommandVPRO>(cmd);
        loop_instruction_register[loop_instruction_index].src1_flags = cluster->loop_mask_src1;
        loop_instruction_register[loop_instruction_index].src2_flags = cluster->loop_mask_src2;
        loop_instruction_register[loop_instruction_index].dst_flags = cluster->loop_mask_dst;
        loop_instruction_index++;

        loop_register[loop_index].value = 0;
        loop_register[loop_index].start = cmd->x;
        loop_register[loop_index].end = cmd->x_end;
        loop_register[loop_index].incr_step = cmd->y;
        loop_register[loop_index].start_position = loop_instruction_index;
        loop_register[loop_index].is_running = false; // flag to show if completely filled

        if (debug & DEBUG_LOOPER_DETAILED){
            printf_warning("[LOOPER-PUSH] new Loop Start cmd @%i\n", loop_instruction_index-1);
        }
        // no loop start is pushed into queue
        return true;
    }

    if (cmd->type == CommandVPRO::LOOP_END){

        if (!loopPushing){ // no looping active
            printf_error("no LOOP active! LOOP_END needs active loop! \n");
            return false;
        }

        if( !loop_register[loop_index].is_running ) { // is still filling
            loop_instruction_register[loop_instruction_index].command = std::shared_ptr<CommandVPRO>(cmd);
            loop_instruction_register[loop_instruction_index].src1_flags = cluster->loop_mask_src1;
            loop_instruction_register[loop_instruction_index].src2_flags = cluster->loop_mask_src2;
            loop_instruction_register[loop_instruction_index].dst_flags = cluster->loop_mask_dst;
            loop_register[loop_index].is_running = true; // filling finished
            loop_instruction_index++;
        }

        if (loop_index == 0){
            loopPushing = false;
        } else {
            loop_index--;
        }

        if (debug & DEBUG_LOOPER_DETAILED){
            printf_warning("[LOOPER-PUSH] new Loop End cmd @%i\n", loop_instruction_index-1);
        }
        // do not push end to queue
        return true;
    }
    if (loopPushing){
        if( !loop_register[loop_index].is_running ){ // is still filling
            loop_instruction_register[loop_instruction_index].command = std::shared_ptr<CommandVPRO>(cmd);
            loop_instruction_register[loop_instruction_index].src1_flags = cluster->loop_mask_src1;
            loop_instruction_register[loop_instruction_index].src2_flags = cluster->loop_mask_src2;
            loop_instruction_register[loop_instruction_index].dst_flags = cluster->loop_mask_dst;
            loop_instruction_index++;

            if (debug & DEBUG_LOOPER_DETAILED){
                printf_warning("[LOOPER-PUSH] new Loop cmd @%i\n", loop_instruction_index-1);
            }
            // do not push to queue. this is performed by loop unit ( in tick() )
            return true;
        }
    }

    //*********************************************
    // Push to Units Queue
    //*********************************************
    if (cmd->type == CommandVPRO::NONE)
        return true;

    if (isCmdQueueFull()){
        printf_error("UNIT received CMD but Q is full! should wait (by cluster) until space available...\n");
        return false;
    }

    cmd_queue.push_back(cmd);

    return true;
}

void VectorUnit::clearCommands() {
    cmd_queue.clear();
}

bool VectorUnit::isLoopUnitBlocking(){
    return (loopParsing && !loopPushing);
}

void VectorUnit::update_loop_registers() {

    if (debug & DEBUG_TICK)
        printf("Tick Vector Unit %i\n", vector_unit_id);

    // LOOP Parsing unit
    // can add one instruction per cycle to units cmd queue
    // adds loop values to offset
    // checks loop end
    if (loopParsing) {
        if (!isCmdQueueFull() && loop_instruction_index > loop_instruction_fetch_index) {
            if (debug & DEBUG_LOOPER_DETAILED) {
                printf_warning("[LOOPER] status: fetched Loop %i, fetched instruction: %i\n", loop_fetch_index,
                               loop_instruction_fetch_index);
            }

            std::shared_ptr<CommandVPRO> loopCmd = std::make_shared<CommandVPRO>(
                    loop_instruction_register[loop_instruction_fetch_index].command.get());
            loop_instruction_fetch_index++;
            if (debug & DEBUG_LOOPER_DETAILED) {
                printf_warning("[LOOPER] instruction: \n");
                loopCmd->print();
                printf("\n");
            }
            if (loopCmd->type == CommandVPRO::LOOP_START) {
                if (loop_instruction_fetch_index - 1 > 0) // this is not the outest loop
                    loop_fetch_index++;

                // set loop indexes... ?
                loop_register[loop_fetch_index].value = 0;
                loop_register[loop_fetch_index].start = loopCmd->x;
                loop_register[loop_fetch_index].end = loopCmd->x_end;
                loop_register[loop_fetch_index].incr_step = loopCmd->y;
                loop_register[loop_fetch_index].start_position = loop_instruction_fetch_index;
                if (debug & DEBUG_LOOPER) {
                    for (int loop = 0; loop <= loop_fetch_index; loop++) {
                        printf("\t");
                    }
                    printf_warning("[LOOPER] Starting new loop! \n");
                }
            } else if (loopCmd->type == CommandVPRO::LOOP_END) {
                // process loop...
                // fill queue again
                //   1. check current loop iteration status
                if (debug & DEBUG_LOOPER) {
                    for (int loop = 0; loop <= loop_fetch_index; loop++) {
                        printf("\t");
                    }
                }
                if (loop_register[loop_fetch_index].value >= loop_register[loop_fetch_index].end) { // LOOP finished
                    if (loop_fetch_index > 0) { // another outer loop exists
                        if (debug & DEBUG_LOOPER) {
                            printf_warning("[LOOPER] ending loop [finish - outer loop will continue]! \n");
                        }
                        loop_fetch_index--; // get next instr from outer loop
                    } else {
                        if (debug & DEBUG_LOOPER) {
                            printf_warning("[LOOPER] ending loop [finish - parsing loops finished]! \n");
                        }
                        loopParsing = false;
                        loop_instruction_fetch_index = 0;
                        loop_index = 0;
                        loop_fetch_index = 0;
                        loop_instruction_index = 0;
                    }
                } else {
                    if (debug & DEBUG_LOOPER) {
                        printf_warning("[LOOPER] ending loop [continue. incremented the value]! %i => %i\n",
                                       loop_register[loop_fetch_index].value, loop_register[loop_fetch_index].value +
                                                                              loop_register[loop_fetch_index].incr_step);
                    }
                    // loop not finished yet
                    // increment value and continue with first instruction
                    loop_register[loop_fetch_index].value += loop_register[loop_fetch_index].incr_step;
                    loop_instruction_fetch_index = loop_register[loop_fetch_index].start_position;
                }
            } else { // regular instruction inside loop
                for (uint loop = 0; loop <= loop_fetch_index; loop++) {
                    int val = loop_register[loop].value;
                    if (debug & DEBUG_LOOPER_DETAILED) {
                        printf_warning("\t\tVal to add: %i\n", val);
                    }
                    if (((loop_instruction_register[loop_instruction_fetch_index - 1].src1_flags >> loop) &
                         0x00000001) == 1) // bit at loop_fetch_index == 1
                        loopCmd->src1.offset += val;
                    if (((loop_instruction_register[loop_instruction_fetch_index - 1].src2_flags >> loop) &
                         0x00000001) == 1) // bit at loop_fetch_index == 1
                        loopCmd->src2.offset += val;
                    if (((loop_instruction_register[loop_instruction_fetch_index - 1].dst_flags >> loop) &
                         0x00000001) == 1) // bit at loop_fetch_index == 1
                        loopCmd->dst.offset += val;
                }
                bool isLoopPush_saver = loopPushing;
                loopPushing = false;
                loopParsing = false;
                sendCMD(loopCmd); // send to queue [no loop filling instruction]
                loopParsing = true;
                loopPushing = isLoopPush_saver;

                if (debug & DEBUG_LOOPER_DETAILED) {
                    printf_warning("[LOOPER] Pushed command into queue:! \n");
                    loopCmd->print();
                    printf("\n");
                }
            }
        }
    }

//    printf("[Unit] Queue Length: %li \n", cmd_queue.size());

    //*********************************************
    // Lane TICK
    //*********************************************
    cmdQueueFetchedCmd = false; // enable fetch of one new command in this cycle
}

void VectorUnit::check_stall_conditions() {
    for (VectorLane *lane : lanes) {
        lane->check_stall_conditions();
    }
 //   for (VectorLane *lane : lanes) {
 //       lane->update_stall_condition();
 //   }
 /*
    if ((debug & DEBUG_LANE_STALLS) != 0){
        bool is_print = false;
            for (VectorLane *lane : lanes) {
                if (lane->is_lane_chaining()) printf(" Stall Check got DST rdy @ lane %i\n", lane->vector_lane_id);
                if (lane->is_dst_lane_ready()) printf(" Stall Check got DST is read @ lane %i\n", lane->vector_lane_id);
                if (lane->is_dst_lane_stalling()) printf(" Stall Check got Stall by dst @ lane %i\n", lane->vector_lane_id);
                if (lane->is_src_lane_stalling()) printf(" Stall Check got Stall by dst @ lane %i\n", lane->vector_lane_id);
                if (lane->is_lane_chaining() || lane->is_dst_lane_ready() || lane->is_dst_lane_stalling() || lane->is_src_lane_stalling()) is_print = true;
            }
        if (is_print) printf(" ---------------------------- Condition Check done\n");
    }
  */
}

void VectorUnit::update_stall_conditions(){
    for (VectorLane *lane : lanes) {
        lane->update_stall_condition();
    }
    if (debug & DEBUG_LANE_STALLS){
        for (VectorLane *lane : lanes) {
            printf(ORANGE);
            printf("C%02iU%02i", cluster->cluster_id, vector_unit_id);
            printf((lane->isLSLane()) ? "LS: " : "L%i: ", lane->vector_lane_id);
            printf("  %s", (lane->is_lane_chaining()) ? "lane_chaining" : "\e[90mlane_chaining\e[93m");
            printf("  %s", (lane->is_dst_lane_ready()) ? "dst_lane_ready" : "\e[90mdst_lane_ready\e[93m");
            printf("  %s", (lane->is_dst_lane_stalling()) ? "dst_lane_stalling" : "\e[90mdst_lane_stalling\e[93m");
            printf("  %s", (lane->is_src_lane_stalling()) ? "src_lane_stalling" : "\e[90msrc_lane_stalling\e[93m");
            printf("\n");
        }
    }
}

void VectorUnit::tick() {

    for (VectorLane *lane : lanes){
        lane->tick();
    }
}

void VectorUnit::update(){
    for (VectorLane *lane : lanes){
        lane->update();
    }

    //*********************************************
    // Unit statistics
    //*********************************************
    int busyLanes = 0;
    for (auto lane : lanes){
        if (lane->getCmd()->type != CommandVPRO::NONE)
            busyLanes++;
    }
    stat.tick(busyLanes);

    if (busyLanes == 0){
        for (auto c : cluster->core->getClusters()){
            if (c->isWaitingForDMA()){
                stat.dmaBusyTick();
                break;
            }
        }
        for (auto c : cluster->core->getClusters()){
            if (c->isWaitingForVPRO()){
                stat.vproBusyTick();
                break;
            }
        }
//        if (cluster->isWaitingForDMA())
//            stat.dmaBusyTick();
//        if (cluster->isWaitingForVPRO())
//            stat.vproBusyTick();
    }
}

/**
 * gets command for the lane to process. all lane command queues should be at the same state for x/y/is_done ...
 * @param id Lane
 * @return
 */
std::shared_ptr<CommandVPRO> VectorUnit::getNextCommandForLane(int id) {
    // TODO: Mutex (if THREAD_LANE)

    if(cmdQueueFetchedCmd){ // FIFO can only fetch front cmd
        return noneCmd;
    }

    if (cmd_queue.empty()){
//        if (loop_fetch_index > 0){
////            printf_error("Cmd queue is empty! but not all loops are parsed! [Looper should fill queue...]\n");
//        }
        return noneCmd;
    }

    for (auto lane : lanes){
        if (lane->isBlocking())
            return noneCmd;
    }

    auto cmd = cmd_queue.front();
    if (isLaneSelected(cmd.get(), id)) {

        // Check minimal vector length + print warning
        if (checkVectorPipelineLength && cmd->fu_sel != CLASS_OTHER && cmd->x == cmd->x_end &&
            cmd->y == cmd->y_end) {
            uint32_t vector_length = (cmd->x_end + 1) * (cmd->y_end + 1);
            if ((vector_length < pipeline_depth) && (cmd->fu_sel != CLASS_MEM) &&
                (cmd->func != OPCODE_LOOP_START &&
                 cmd->func != OPCODE_LOOP_END &&
                 cmd->func != OPCODE_LOOP_MASK)) { // ignore for LOOP instructions
                cmd->print_type();
                printf_warning("Vector length(%d) smaller than pipeline depth(%d)! User should add wait states!\n",
                               vector_length, pipeline_depth);
            }
        }

        cmd_queue.front()->id_mask &= ~(1u << uint(id));
            // unset the mask bit for this unit so this command dont get assigned to it again
        if (cmd_queue.front()->id_mask == 0) {
            // if assigned to all lanes it should be poped from queue
            cmd_queue.pop_front();
            cmdQueueFetchedCmd = true;
            cmd->id_mask |= (1u << uint(id));
            return cmd;
        } else {
            return std::make_shared<CommandVPRO>(cmd.get());
        }
    }

    return noneCmd;
}


uint32_t VectorUnit::getLocalMemoryData(int addr, int size) {
    if (addr * (LOCAL_MEMORY_DATA_WIDTH / 8) + size > local_memory_size * (LOCAL_MEMORY_DATA_WIDTH / 8)) {
        printf_error("Read from Local Memory out of Range! (addr: %d, size: %d)\n", addr, size);
        return 0;
    }
    if (size != 2){
        printf_error("LM Read for size != 2 [NOT IMPLEMENTED!]\n");
    }
    return uint32_t(*((uint16_t*)&local_memory[addr*2]));
}

void VectorUnit::writeLocalMemoryData(int addr, uint32_t data, int size) {
//    printf("Write LM [%i] DATA %i\n", addr, data);
    //int size = sizeof(data)/sizeof(uint8_t);
    if (addr + (LOCAL_MEMORY_DATA_WIDTH / 8) > local_memory_size * (LOCAL_MEMORY_DATA_WIDTH / 8)) {
        printf_error("Write to Local Memory out of Range! (addr: %d)\n", addr);
        return;
    }

    if (size > (LOCAL_MEMORY_DATA_WIDTH / 8)) {
        printf_error("Write to Local Memory more than LM DATA WIDTH (%i)\n", (LOCAL_MEMORY_DATA_WIDTH / 8));
    }

//    uint32_t  d = __24to32signed(__16to24signed(data));
    for (uint i = 0; i < size; i++) {
        local_memory[addr * (LOCAL_MEMORY_DATA_WIDTH / 8) + i] = uint8_t(data >> (i*8));
    }
}


bool VectorUnit::isBusy() {
    bool lanes_busy = loop_fetch_index > 0 || loopParsing || !cmd_queue.empty();
    for (auto lane : lanes) {
        lanes_busy = lanes_busy || lane->isBusy();
    }
    return lanes_busy;
}


// ***********************************************************************
// Dump local memory to console
// ***********************************************************************
void VectorUnit::dumpLocalMemory(const std::string prefix) {
    printf("%s", prefix.c_str());
    printf("DUMP Local Memory (Vector Unit %i):\n", vector_unit_id);
    printf(LGREEN);
    int printLast = 0;
    for (int l = 0; l < local_memory_size; l += 32) {
        bool hasData = false;
        for (int m = 0; m <= 31; m++) {
            uint32_t data = getLocalMemoryData(l + m);
            hasData |= (data != 0);
        }
        if (hasData) {
            printLast = std::min(printLast + 1, 1);
            printf(LGREEN);
            printf("%s", prefix.c_str());
            printf("$%04x:  ", l & (~(32 - 1)));
            for (int m = 0; m <= 31; m++) { // print a line with 32 entries
                if (m == 16)    // for line break...
                    printf("\n\t\t");
                uint32_t data = getLocalMemoryData(l + m);
                if (data == 0) {
                    printf(LIGHT);
                    printf("0x%04x, ", (uint32_t) data); // data & 0xffff); // in gray
                    printf(NORMAL_);
                } else {
                    printf(LGREEN);
                    printf("0x%04x, ", data); //data & 0xffff);
                }
            }
            printf("\n");
        } else {
            if (printLast == 0) { // if no data, print ...
                printf("%s", prefix.c_str());
                printf("...\n");
            }
            printLast = std::max(printLast - 1, -1);
        }
    }
    printf(RESET_COLOR);
    
}


void VectorUnit::dumpRegisterFile(int lane) {
    lanes[lane]->dumpRegisterFile();
}

void VectorUnit::dumpQueue(){
    printf(LBLUE);
    printf("Command Queue (Cluster %i, Unit %i):\n", cluster->cluster_id, vector_unit_id);
    int i = 0;
    for (std::shared_ptr<CommandVPRO> &cmd : cmd_queue){
        printf("%i: \t", i);
        cmd.get()->print();
        i++;
    }
    printf("######### Size: %i\n", cmd_queue.size());
    printf(RESET_COLOR);
}


