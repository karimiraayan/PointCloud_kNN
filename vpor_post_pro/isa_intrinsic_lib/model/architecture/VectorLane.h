// ########################################################
// # VPRO instruction & system simulation library         #
// # Sven Gesper, IMS, Uni Hannover, 2019                 #
// ########################################################
// # Lane class                                           #
// ########################################################

#ifndef VPRO_CPP_VECTORLANE_H
#define VPRO_CPP_VECTORLANE_H

// C std libraries
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <inttypes.h>
#include <vector>
#include <string>
#include <memory>
#include <QtCore/QVector>
#include <QThread>
#include <QSemaphore>

#include "../commands/CommandVPRO.h"
#include "stats/LaneStat.h"

// to be included in cpp
class VectorUnit;

class VectorLane : public QThread {
Q_OBJECT
public:
    VectorLane(int id, VectorUnit *vector_unit, int register_file_size = 1024, int pipeline_depth = 10,
               int vector_lane_count = 2);

    VectorLane(int id, int register_file_size = 1024, int pipeline_depth = 10, int vector_lane_count = 2);

    struct PipelineDate {
        // result (got in stage of alu finish)
        uint32_t data;
        // in sim the result is calc in one cycle / loaded in same. stored (tmp) here
        uint32_t pre_data;
        std::shared_ptr<CommandVPRO> cmd;

        uint32_t opa, opb;
        uint32_t res;
        int rf_addr, lm_addr;
        bool move;

        bool flag[2];

        PipelineDate() {
            data = 0;
            opa = 0;
            opb = 0;
            rf_addr = 0;
            lm_addr = 0;
            move = false;
            cmd = std::make_shared<CommandVPRO>();
            flag[0] = false;
            flag[1] = false;
            res = 0;
        };
    };

#ifdef THREAD_LANE
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

    void tick();

    void check_stall_conditions();
    void update_stall_condition();

    bool is_lane_chaining(){return lane_chaining;}
    uint32_t dst_units;
    uint32_t dst_lanes;
    void set_dst_lane_ready(uint32_t _dst_lanes, uint32_t _dst_units){
        dst_lanes |= _dst_lanes;
        dst_units |= _dst_units;
        dst_lane_stall_nxt = false;
        dst_lane_ready = true;
    }
    //todo dont know if the next line is correct
    bool is_dst_lane_ready(){return dst_lane_ready;}
    bool is_dst_lane_stalling(){return dst_lane_stall;}
    bool is_src_lane_stalling(){return src_lane_stall;}

    virtual void tick_pipeline(int from, int until);

    virtual uint32_t get_rf_data(int addr, int size = 3);

    virtual void set_rf_data(int addr, uint32_t data, int size = 3);

    virtual bool get_z_flag(addr_field_t src, uint32_t x, uint32_t y, uint32_t x_end);

    virtual bool get_n_flag(addr_field_t src, uint32_t x, uint32_t y, uint32_t x_end);

    virtual bool get_rf_flag(int addr, int select);

    uint32_t get_pipeline_chain_data();

    bool *getPipelineForwardFlags();

    virtual void dumpRegisterFile(std::string prefix = "");

    std::shared_ptr<CommandVPRO> getCmd();

    bool isBusy();

    bool isBlocking();

    int vector_lane_id;

    LaneStat stat;

    void resetAccu();

    void resetMinMax(uint32_t value);

    VectorLane *getLeftNeighbor() { return left_lane; }

    VectorLane *getRightNeighbor() { return right_lane; }

    VectorLane *getLSLane() { return ls_lane; }

    void setNeighbors(VectorLane *left, VectorLane *right, VectorLane *ls) {
        left_lane = left;
        right_lane = right;
        ls_lane = ls;
    };

    virtual uint8_t *getregister() { return rf_data; }

    virtual bool *getzeroflag() { return rf_flag[0]; }

    virtual bool *getnegativeflag() { return rf_flag[1]; }

    int register_file_size, pipeline_depth, vector_lane_count;

    bool is_src_chaining(addr_field_t src);

    virtual bool isLSLane() { return false; };

    void print_pipeline(const QString prefix = "");

protected:
    long clock_cycle;

    int consecutive_DST_stall_counter;
    int consecutive_SRC_stall_counter;

    // reference to parent vector unit (needed for local mem and cmd queue acccess)
    VectorUnit *vector_unit;
    std::shared_ptr<CommandVPRO> current_cmd, new_cmd, noneCmd;

    bool blocking, blocking_nxt;

    // mac accu
    uint64_t accu;

    // location of min / max instruction
    uint32_t minmax_index;
    uint32_t minmax_value;

    uint8_t *rf_data; // each lane has a register file. 3 elements of this 8-bit array form an element of the rf! [0 - LSB][1][2 - MSB]
    bool *rf_flag[2]; // each register file entry has two additional flags: is_zero, is_negative

    // Neighbors
    VectorLane *left_lane;
    VectorLane *right_lane;
    VectorLane *ls_lane;

    // the processing needs PIPELINE_DEPTH cycles. each cycle the last entry from pipeline is written into rf_data!
    // on parsing a new command, a new pipeline entry is inserted with currently read data from rf
//    PipelineDate results_pipeline[11];
    QVector<PipelineDate> pipeline;

    // resolve address into data (3*8 bit = 24 bit datawidth)
    uint32_t get_operand(addr_field_t src, uint32_t x, uint32_t y, uint32_t x_end);

    VectorLane *get_lane_from_src(addr_field_t src);

    // each tick do this;
//    PipelineDate execute_cmd(CommandVPRO * cmd);
    virtual void execute_cmd(PipelineDate &pipe);

    void processPipeline(std::shared_ptr<CommandVPRO> newElement);

    // flag whether to stall, waiting for src lane chaining output to become rdy
    bool src_lane_stall, src_lane_stall_nxt;

    // stalls until dst is read, released by getOperand/get_pipeline_chain_data.
    // two entries for sim. if sim of other lane is one clock cycle behind. stall is eval for [1].
    // access depending on cycle offset
    bool dst_lane_stall, dst_lane_stall_nxt; // tmp for use during tick (parallel)
    //bool chain_DSTisRdy, chain_DSTisRdy_nxt; // tmp for use during tick (parallel)
    bool *flag, *flag_nxt;
    bool lane_chaining, lane_chaining_nxt;
    bool dst_lane_ready;
    uint32_t chain_data, chain_data_nxt;

    // in this stage, the chain input is read
    int chain_target_stage = 3;
public:
    // take _nxt values and put into register
    void update();

    bool updated;
protected:
    // depth of current pipeline (in which step the wb is executed, may change with new commands...)
    // read in tick stage pipelinedepth + 5 (WB)
    // updated in tick() when a different pipeline depth cmd is received (stage 0/-1)
    int pipelineALUDepth;


};


#endif //VPRO_CPP_VECTORLANE_H
