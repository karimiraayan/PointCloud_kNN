//
// Created by gesper on 06.03.19.
//

#include <limits>       // std::numeric_limits
#include <bitset>

#include "Cluster.h"
#include "../../simulator/helper/debugHelper.h"
#include "../../simulator/helper/typeConversion.h"
#include "../../simulator/simCore.h"

Cluster::Cluster(SimCore *core, int id, uint32_t &unit_mask_global, uint32_t &loop_mask_src1, uint32_t &loop_mask_src2, uint32_t &loop_mask_dst,
                 uint32_t &ACCU_MAC_HIGH_BIT_SHIFT, uint32_t &ACCU_MUL_HIGH_BIT_SHIFT,
                 uint32_t &dma_pad_top, uint32_t &dma_pad_right, uint32_t &dma_pad_bottom, uint32_t &dma_pad_left, uint32_t &dma_pad_value,
                 uint8_t *main_memory, long &clock,
                 int main_memory_size, int vector_unit_count, int local_memory_size, int vector_lane_count, int register_file_size, int pipeline_depth):
        core(core),
        cluster_id(id),
        unit_mask_global(unit_mask_global),
        loop_mask_src1(loop_mask_src1),
        loop_mask_src2(loop_mask_src2),
        loop_mask_dst(loop_mask_dst),
        main_memory_size(main_memory_size),
        vector_unit_count(vector_unit_count),
        local_memory_size(local_memory_size),
        vector_lane_count(vector_lane_count),
        register_file_size(register_file_size),
        pipeline_depth(pipeline_depth),
        clock(clock),
        ACCU_MAC_HIGH_BIT_SHIFT(ACCU_MAC_HIGH_BIT_SHIFT),
        ACCU_MUL_HIGH_BIT_SHIFT(ACCU_MUL_HIGH_BIT_SHIFT),
        waitBusy(false),
        waitDMABusy(false)
{
    //printf("SIM: Instanziated new Cluster (cluster: %i)\n", cluster_id);
    for (int i = 0; i < vector_unit_count; i++){
        units.push_back(new VectorUnit(i, this, clock, local_memory_size, vector_lane_count, register_file_size, pipeline_depth));
    }
    dma = new DMA(this, units,
                  dma_pad_top, dma_pad_right, dma_pad_bottom, dma_pad_left, dma_pad_value,
                  main_memory, main_memory_size);

    if (vector_unit_count > 1){
        units[0]->setNeighbors(units[vector_unit_count - 1], units[1]);
        for (int i = 1; i < vector_unit_count - 1; i++) {
            units[i]->setNeighbors(units[i - 1], units[i + 1]);
        }
        units[vector_unit_count - 1]->setNeighbors(units[vector_unit_count - 2], units[0]);
    } else {
        units[0]->setNeighbors(units[0], units[0]);
    }

#ifdef THREAD_UNIT
        for (VectorUnit *unit : units){
            unit->start();
        }
#endif

}


bool Cluster::isBusy(){
    for(VectorUnit *unit : units){
        if (unit->isBusy())
            return true;
    }

    return dma->isBusy();
}

/**
 * Clock tick
 */
void Cluster::tick(){


    if (debug & DEBUG_TICK)
        printf("Tick Cluster %i\n", cluster_id);

    dma->tick();

#ifdef THREAD_UNIT
        for (VectorUnit *unit : units){
            unit->tick_en.release();
        }
        for(VectorUnit *unit : units){
            unit->tick_done.acquire();
        }
#else
    for(VectorUnit *unit : units){
        unit->update_loop_registers();
    }

    constexpr int update_iterations = 3;
    for(int i = 0; i < update_iterations; ++i) {
        for(VectorUnit *unit : units){
            unit->check_stall_conditions();
        }
        for(VectorUnit *unit : units){
            unit->update_stall_conditions();
        }
    }

    for(VectorUnit *unit : units){
        unit->tick();
    }

    for(VectorUnit *unit : units){
        unit->update();
    }
#endif

}

bool Cluster::isReadyForCommand(){
    for (auto unit : units){
        if (unit->isCmdQueueFull() || unit->isLoopUnitBlocking())
            return false;
    }
    if (waitBusy){
        bool busy = false;
        for(VectorUnit *unit : units){
            busy |= unit->isBusy();
        }
        if (!busy)
            waitBusy = false;
        return !waitBusy;
    }
    if (waitDMABusy){
        if (!dma->isBusy())
            waitDMABusy = false;
        return !waitDMABusy;
    }
    return true;
}

bool Cluster::isWaitingForDMA(){
    return waitDMABusy;
}
void Cluster::setWaitingForDMA(bool dma){
    waitDMABusy = dma;
}
bool Cluster::isWaitingForVPRO(){
    return waitBusy;
}
void Cluster::setWaitingForVPRO(bool vpro){
    waitBusy = vpro;
}
/**
 * command from sim to this cluster. push to units or to dma
 * @param cmd
 */
bool Cluster::sendCMD(std::shared_ptr<CommandBase> cmd){
    if (cmd->class_type == CommandBase::VPRO){
        auto vprocmd = std::dynamic_pointer_cast<CommandVPRO>(cmd);
        if (debug & DEBUG_INSTRUCTION_SCHEDULING || debug & DEBUG_INSTRUCTION_SCHEDULING_BASIC){
            printf("Cluster %i Got a new VPRO command (@clock = %li):", cluster_id, clock);
            printf("\t");
            if (debug & DEBUG_INSTRUCTION_SCHEDULING_BASIC){
                vprocmd.get()->print_type();printf("\n");
            } else {
                printf("\n\t"); print_cmd(vprocmd.get());
            }
        }
        if (vprocmd->type == CommandVPRO::LOOP_MASK){
            printf_error("LOOP_MASK received in CLUSTER %i (@clock = %li)! should have been processed by a global register... \n", cluster_id, clock);
            return false;
        } else{ // other command to process by vpro
            uint32_t ret = 0;
            for (VectorUnit *unit : units)
            {
                //*********************************************
                // Check global mask (for this unit)
                //*********************************************
                if (((1u << unit->vector_unit_id) & unit_mask_global) > 0){
                    if (!unit->sendCMD(std::make_shared<CommandVPRO>(vprocmd.get()))){
                        ret += (1u << unit->vector_unit_id);
                    }
                }
            }
            if (ret != 0){
                printf_error("Command push to units failed! units queue full or looper active! -> TODO check before + repeat\n");
                printf_error("unit error count is: %i\n", ret);
            }
            return (ret == 0);
        }
    } else if (cmd->class_type == CommandBase::DMA){
        auto dmacmd = std::dynamic_pointer_cast<CommandDMA>(cmd);
        if (debug & DEBUG_INSTRUCTION_SCHEDULING){
            printf("Cluster %i Got a new DMA command (@clock = %li):\n", cluster_id, clock);
            printf("\t"); print_cmd(dmacmd.get());
        }
        dma->execute_cmd(dmacmd);
        return true;
    } else{
        printf_warning("Unknown Command type in Cluster %i received (@clock = %li)!\n", cluster_id, clock);
        return true;
    }
}


void Cluster::dumpQueue(int unit){
    this->units[unit]->dumpQueue();
}
void Cluster::dumpLocalMemory(int unit){
    this->units[unit]->dumpLocalMemory();
}
void Cluster::dumpRegisterFile(int unit, int lane){
    this->units[unit]->dumpRegisterFile(lane);
}
