// ########################################################
// # VPRO instruction & system simulation library         #
// # Sven Gesper, IMS, Uni Hannover, 2019                 #
// ########################################################
// # Cluster class                                        #
// ########################################################

#ifndef VPRO_CPP_CLUSTER_H
#define VPRO_CPP_CLUSTER_H

// C std libraries
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <inttypes.h>
#include <vector>
#include <list>

#include <QThread>
#include <QMutex>

#include "VectorUnit.h"
#include "DMA.h"

class SimCore;

class Cluster : public QThread{
Q_OBJECT
public:
    Cluster(SimCore *core, int id, uint32_t &unit_mask_global, uint32_t &loop_mask_src1, uint32_t &loop_mask_src2,
            uint32_t &loop_mask_dst, uint32_t &ACCU_MAC_HIGH_BIT_SHIFT, uint32_t &ACCU_MUL_HIGH_BIT_SHIFT,
            uint32_t &dma_pad_top, uint32_t &dma_pad_right, uint32_t &dma_pad_bottom, uint32_t &dma_pad_left, uint32_t &dma_pad_value,
            uint8_t *main_memory, long &clock, int main_memory_size = 1024 * 1024 * 512,
            int vector_unit_count = 2, int local_memory_size = 8192, int vector_lane_count = 2,
            int register_file_size = 1024, int pipeline_depth = 10);


#ifdef THREAD_CLUSTER
    QMutex tick_en;
    QMutex tick_done;

    [[noreturn]] void run() override {
        while(true){
            tick_en.lock();
            tick();
            tick_done.unlock();
        }
    }

#endif

    void tick();

    /**
     * send commands into unit queue or dma queue
     * @param cmd vpro command to be processed (id sensitive for cluster/unit?!)
     */
    bool sendCMD(std::shared_ptr<CommandBase> cmd);

    void dumpLocalMemory(int unit);
    void dumpQueue(int unit);
    void dumpRegisterFile(int unit, int lane);

    uint32_t &unit_mask_global;
    uint32_t &loop_mask_src1;
    uint32_t &loop_mask_src2;
    uint32_t &loop_mask_dst;
    uint32_t &ACCU_MAC_HIGH_BIT_SHIFT;
    uint32_t &ACCU_MUL_HIGH_BIT_SHIFT;

    int cluster_id;

    DMA *dma;
    SimCore *core;

    bool isBusy();

    bool isReadyForCommand();

    bool isWaitingForDMA();
    void setWaitingForDMA(bool dma);
    bool isWaitingForVPRO();
    void setWaitingForVPRO(bool vpro);

    [[nodiscard]] bool isWaitBusy() const{return waitBusy;};
    [[nodiscard]] bool isWaitDMABusy() const{return waitDMABusy;};

    std::vector<VectorUnit *> &getUnits() { return units; }

    [[nodiscard]] int getVectorUnitCount() const{
        return vector_unit_count;
    }
private:
    // if receive of command wait busy caused waiting
    bool waitBusy;
    bool waitDMABusy;

    long &clock; // sim clock
    int main_memory_size, vector_unit_count, local_memory_size, vector_lane_count, register_file_size, pipeline_depth;

    // all the VectorUnits in this cluster are stored here
    std::vector<VectorUnit *> units;
};


#endif //VPRO_CPP_CLUSTER_H
