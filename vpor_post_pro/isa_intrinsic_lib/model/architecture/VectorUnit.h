// ########################################################
// # VPRO instruction & system simulation library         #
// # Sven Gesper, IMS, Uni Hannover, 2019                 #
// ########################################################
// # Unit class                                           #
// ########################################################

#ifndef VPRO_CPP_VECTORUNIT_H
#define VPRO_CPP_VECTORUNIT_H

// C std libraries
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <inttypes.h>
#include <vector>
#include <list>
#include <QVector>
#include <memory>

#include <QThread>
#include <QSemaphore>

#include "VectorLane.h"
#include "VectorLaneLS.h"
#include "../../simulator/setting.h"
#include "stats/UnitStat.h"

// to be included in cpp
class Cluster;

class VectorUnit : public QThread {
    Q_OBJECT


public:
    VectorUnit(int id, Cluster *cluster, long &clock, int local_memory_size = 8192, int vector_lane_count = 2, int register_file_size = 1024, int pipeline_depth = 10);

#ifdef THREAD_UNIT
    QSemaphore tick_en;
    QSemaphore tick_done;

    void run() override {
        while(true){
            tick_en.acquire();
            tick();
            tick_done.release();
        }
    }
#endif

    void update_stall_conditions();
    void update_loop_registers();
    void check_stall_conditions();
    void tick();
    void update();

    VectorUnit *getLeftNeighbor(){ return left_unit; }
    VectorUnit *getRightNeighbor(){ return right_unit; }
    void setNeighbors(VectorUnit *left, VectorUnit *right){
        left_unit = left;
        right_unit = right;
    };
    void setLSLane(VectorLaneLS *ls){
        ls_lane = ls;
    };
    VectorLaneLS *getLSLane() {return ls_lane; };

    uint32_t getLocalMemoryData(int addr, int size = 2);
    void writeLocalMemoryData(int addr, uint32_t data, int size = 2);

    bool sendCMD(std::shared_ptr<CommandVPRO> cmd);

    void clearCommands();

    void dumpLocalMemory(const std::string prefix = "");
    void dumpRegisterFile(int lane);
    void dumpQueue();

    Cluster *getCluster(){
        return cluster;
    };

    const static int LOCAL_MEMORY_DATA_WIDTH = 16;

    int vector_unit_id;

    // reference to parent cluster
    Cluster *cluster;

    bool isBusy();
    bool isLoopUnitBlocking();

    std::shared_ptr<CommandVPRO> getNextCommandForLane(int id);

    std::vector<VectorLane *> &getLanes(){ return lanes; }

    bool isLaneSelected(CommandVPRO *cmd, long lane_id);

    bool isCmdQueueFull();
    uint8_t *getlocalmemory(){return local_memory;}
    bool isLoopPushing(){return loopPushing;};
    bool isLoopParsing(){return loopParsing;};
    QVector<std::shared_ptr<CommandVPRO>> getCopyOfCommandQueue(){
        QVector<std::shared_ptr<CommandVPRO>> copy;
        for (auto cmd : cmd_queue){
            copy.push_back(cmd);
        }
        return copy;
    }

    UnitStat stat;

    int local_memory_size, vector_lane_count, register_file_size, pipeline_depth;
private:

    // all the VectorLanes in this unit are stored here
    std::vector<VectorLane *> lanes;
    // neighbors to access (e.g. from LS chain)
    VectorUnit *left_unit;
    VectorUnit *right_unit;

    VectorLaneLS *ls_lane;

    // cmd-queue from this unit.
    QVector<std::shared_ptr<CommandVPRO>> cmd_queue;

    // reference to clock from sim
    long &clock;

    uint8_t *local_memory; // each unit has a local_memory, the lanes can access

    bool loopPushing; // whether received commands belong into loop / loop register
    bool loopParsing; // whether the loop unit should parse commands from loop register into queue

    uint32_t loop_index; // for pushing commands from mips to correct loop register
    uint32_t loop_instruction_index;

    uint32_t loop_fetch_index; // for fetching commands from loop unit from correct loop register
    uint32_t loop_instruction_fetch_index;

    struct LOOP{
        uint32_t value;
        uint32_t start;
        uint32_t end;
        uint32_t incr_step;
        uint32_t start_position;
        bool is_running;
    };
    std::array<LOOP, MAX_LOOP_CASCADES> loop_register;

    struct LOOP_Instruction{
        std::shared_ptr<CommandVPRO> command;
        uint32_t src1_flags;
        uint32_t src2_flags;
        uint32_t dst_flags;
    };

    std::array<LOOP_Instruction, MAX_LOOP_INSTRUCTIONS> loop_instruction_register;

    // returned if cmd queue is empty (or similar...)
    std::shared_ptr<CommandVPRO> noneCmd;

    //flag to indicate a command was pulled from cmd queue. in hw only once per cycle a cmd is received from queue...
    bool cmdQueueFetchedCmd;
};


#endif //VPRO_CPP_VECTORUNIT_H
