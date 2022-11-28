// ########################################################
// # VPRO instruction & system simulation library         #
// # Sven Gesper, IMS, Uni Hannover, 2019                 #
// ########################################################
// # DMA class                                            #
// ########################################################

#ifndef VPRO_CPP_DMA_H
#define VPRO_CPP_DMA_H

// C std libraries
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <inttypes.h>
#include <vector>
#include <list>
#include <memory>

#include <QMap>

#include "../commands/CommandDMA.h"
#include "stats/DMAStat.h"

class VectorUnit;
class Cluster;

class DMA {

public:
    DMA(Cluster *cluster, std::vector<VectorUnit *> units,
        uint32_t &dma_pad_top, uint32_t &dma_pad_right, uint32_t &dma_pad_bottom, uint32_t &dma_pad_left, uint32_t &dma_pad_value,
        uint8_t *main_memory, uint32_t main_memory_size = 1024*1024*512);

    /**
     * check if queue is processed
     * @return
     */
    bool isBusy();

    /**
     * push to DMA's cmd queue
     * @param cmd
     */
    void execute_cmd(std::shared_ptr<CommandDMA> cmd);

    /**
     * clock tick to process queues front command
     */
    void tick();

    // DMA statistics
    DMAStat stat;

    std::shared_ptr<CommandDMA> getCmd();

    void setExternalVariableInfo(QMap<uint64_t , QMap<int, QString>> *map){
        this->map = map;
    };
private:

    // to detect new access to an external memory segment.
    // Could cause memory overflow (access to extern instead of main memory if address too large)
    uint64_t ext_addr_base_lst;

    static const uint64_t THIS_IS_LOAD = 42 << 11; // random numbers to distinguish between LOC -> EXT
    static const uint64_t THIS_IS_STORE = 42 << 12;

    const int MAIN_MEMORY_DATA_WIDTH = 16;
    uint32_t main_memory_size;

    // referenced units with LM instances
    std::vector<VectorUnit *> units;
    // superior cluster (containing the units)
    Cluster *cluster;
    // connection to the main memory (extern; reference)
    uint8_t *main_memory;

    // reference to global registers
    uint32_t &dma_pad_top;
    uint32_t &dma_pad_right;
    uint32_t &dma_pad_bottom;
    uint32_t &dma_pad_left;
    uint32_t &dma_pad_value;

    QMap<uint64_t , QMap<int, QString>> *map;

    // current DMA command with all its informations...
    std::shared_ptr<CommandDMA> command;

    std::shared_ptr<CommandDMA> noneCmd;
    std::list<std::shared_ptr<CommandDMA> > cmd_queue;
};


#endif //VPRO_CPP_DMA_H
