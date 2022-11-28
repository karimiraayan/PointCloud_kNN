//
// Created by gesper on 14.03.19.
//

#include "DMA.h"
#include "VectorUnit.h"
#include "Cluster.h"
#include "../../simulator/helper/debugHelper.h"

DMA::DMA(Cluster *cluster, std::vector<VectorUnit *> units,
         uint32_t &dma_pad_top, uint32_t &dma_pad_right, uint32_t &dma_pad_bottom, uint32_t &dma_pad_left, uint32_t &dma_pad_value,
         uint8_t *main_memory, uint32_t main_memory_size):
    cluster(cluster),
    units(units),
    main_memory(main_memory),
    main_memory_size(main_memory_size),
    dma_pad_top(dma_pad_top),
    dma_pad_right(dma_pad_right),
    dma_pad_bottom(dma_pad_bottom),
    dma_pad_left(dma_pad_left),
    dma_pad_value(dma_pad_value)
{
    //printf_info("SIM: \tInstanziated new DMA\n");
    stat = DMAStat();
    noneCmd = std::make_shared<CommandDMA>();
    command = noneCmd;

    cmd_queue = std::list<std::shared_ptr<CommandDMA> >();

    ext_addr_base_lst = -1;
}

std::shared_ptr<CommandDMA> DMA::getCmd() {
    return command;
}

bool DMA::isBusy(){
    return !command->is_done() || !cmd_queue.empty();
}

void DMA::execute_cmd(std::shared_ptr<CommandDMA> cmd){
    cmd_queue.push_back(cmd);
}

void DMA::tick(){
    if (command->is_done() && !cmd_queue.empty()){
        command = cmd_queue.front();
        cmd_queue.pop_front();
        stat.addExecutedCmdQueue(command.get());

//        if (command->type == CommandDMA::EXT_2D_TO_LOC_1D &&     // padding only for 2D ext to loc
//            ((dma_pad_top > 0) && command->pad[CommandDMA::PAD::TOP] ||
//             (dma_pad_bottom > 0) && command->pad[CommandDMA::PAD::BOTTOM] ||
//             (dma_pad_left > 0) && command->pad[CommandDMA::PAD::LEFT] ||
//             (dma_pad_right > 0) && command->pad[CommandDMA::PAD::RIGHT])) {
//            // modify base address to load correct data inside the pad region (the addressed ones)
//
//            if (command->pad[CommandDMA::PAD::LEFT])
//                command->x_stride -= dma_pad_left;
//            if (command->pad[CommandDMA::PAD::RIGHT])
//                command->x_stride -= dma_pad_right;
//
////            command->ext_base = command->ext_base +
////            command->x_size + 2 * pad_width * (2 * pad_width - (command->x_size + 2 * pad_width) - 1);
//
//// 2* due to byte addressing
//            if (command->pad[CommandDMA::PAD::TOP]){
//                command->ext_base -= 2*((command->x_size + (command->x_stride-1)) * dma_pad_top); // move base up to top padding
//            }
//
//            if (command->pad[CommandDMA::PAD::LEFT]){
//                command->ext_base -= 2*(dma_pad_left);
//            }
//        }

        if (debug & DEBUG_DMA){
            for (auto u : command->unit) {
                printf_info("Unit %i: ", u);
                if (command->type == CommandDMA::EXT_1D_TO_LOC_1D || command->type == CommandDMA::EXT_2D_TO_LOC_1D) {
                    printf_info("Cluster %i, Start Load ext from (0x%" PRIx64 ") to loc (unit %i, 0x%08X) [size=%i]\n",
                                cluster->cluster_id, command->ext_base, u, (command->loc_base & 0x000fffff),
                                (command->x_size * command->y_size));
                } else if (command->type == CommandDMA::LOC_1D_TO_EXT_1D ||
                           command->type == CommandDMA::LOC_1D_TO_EXT_2D) {
                    printf_info("Cluster %i, Start Store loc from (unit %i, 0x%08X) to ext (0x%08X) [size=%i]\n",
                                cluster->cluster_id, u, (command->loc_base & 0x000fffff), command->ext_base,
                                (command->x_size * command->y_size));
                }
            }
        }
    }

    // process
    if (!command->is_done()) {
        stat.addExecutedCmdTick(command.get());

        uint64_t ext_addr, loc_addr;
        uint32_t data = 0;

        if (command->type == CommandDMA::EXT_2D_TO_LOC_1D &&     // padding only for 2D ext to loc
            ((command->y < dma_pad_top) && command->pad[CommandDMA::PAD::TOP] ||
             (command->y >= command->y_size - dma_pad_bottom) && command->pad[CommandDMA::PAD::BOTTOM] ||
             (command->x < dma_pad_left) && command->pad[CommandDMA::PAD::LEFT] ||
             (command->x >= command->x_size - dma_pad_right) && command->pad[CommandDMA::PAD::RIGHT])) {

            // this is a padding region. return pad value
            data = uint32_t(dma_pad_value);

            loc_addr = (command->loc_base & 0x000fffffu) + (command->x + command->y * command->x_size);

            if (debug & DEBUG_DMA_DETAIL) printf_info(" data: %i = 0x%04X \n", int16_t(data), int16_t(data));

            // write to selected local memories
            for (auto u : command->unit) {
                units[u]->writeLocalMemoryData(loc_addr, data, (MAIN_MEMORY_DATA_WIDTH / 8));
            }
        } else {
            int tmp_stride;
            switch (command->type) {
                case CommandDMA::EXT_1D_TO_LOC_1D:
                    ext_addr = command->ext_base + 2 * command->x;
                    loc_addr = (command->loc_base & 0x000fffffu) + command->x;
                    break;
                case CommandDMA::EXT_2D_TO_LOC_1D:
                    tmp_stride = command->x_stride;
                    if (command->pad[CommandDMA::PAD::LEFT])
                        tmp_stride -= dma_pad_left;
                    if (command->pad[CommandDMA::PAD::RIGHT])
                        tmp_stride -= dma_pad_right;

                    ext_addr = command->ext_base + 2 * ((command->x) + (command->y) * (tmp_stride+ command->x_size - 1));   // -1 because: stride always have +1 (HW reason)
                    if (command->pad[CommandDMA::PAD::TOP] && command->pad[CommandDMA::PAD::LEFT])
                        ext_addr = command->ext_base + 2 * ((command->x-dma_pad_left) + (command->y-dma_pad_top) * (tmp_stride+ command->x_size - 1));
                    else if (command->pad[CommandDMA::PAD::LEFT])
                        ext_addr = command->ext_base + 2 * ((command->x-dma_pad_left) + (command->y) * (tmp_stride+ command->x_size - 1));
                    else if (command->pad[CommandDMA::PAD::TOP])
                        ext_addr = command->ext_base + 2 * ((command->x) + (command->y-dma_pad_top) * (tmp_stride+ command->x_size - 1));

//                    if (command->pad[CommandDMA::PAD::TOP])
//                        ext_addr -= 2*((command->x_size + (tmp_stride)) * dma_pad_top);
//                    if (command->pad[CommandDMA::PAD::LEFT])
//                        ext_addr -= 2*(dma_pad_left);
//                    if (command->pad[CommandDMA::PAD::RIGHT])
//                        ext_addr -= 2*(dma_pad_right);


                    loc_addr = (command->loc_base & 0x000fffffu) + (command->x + command->y * command->x_size);
                    break;
                case CommandDMA::LOC_1D_TO_EXT_1D:
                    ext_addr = command->ext_base + 2 * command->x;
                    loc_addr = (command->loc_base & 0x000fffffu) + command->x;
                    break;
                case CommandDMA::LOC_1D_TO_EXT_2D:
                    ext_addr =
                            command->ext_base + 2 * (command->x + command->y * (command->x_stride + command->x_size - 1));
                    loc_addr = (command->loc_base & 0x000fffffu) + command->x + command->y * command->x_size;
                    break;
                default:
                    ext_addr = 0;
                    loc_addr = 0;
                    break;
            } // address calc

            for (auto u : command->unit) {
                if (u > cluster->getVectorUnitCount())
                    printf_error("DMA using unit with id: %i (more than VU/Cluster! max: %i)\n", u,
                                 cluster->getVectorUnitCount());
            }

            switch (command->type) {
                case CommandDMA::EXT_1D_TO_LOC_1D:
                case CommandDMA::EXT_2D_TO_LOC_1D:
                    if (debug & DEBUG_DMA) printf_info(".");
                    if (debug & DEBUG_DMA_DETAIL) {
                        for (auto u : command->unit) {
                            printf_info("Cluster %i, Load from ext (%i = 0x%08X) to loc (unit %i, %i = 0x%08X).",
                                        cluster->cluster_id, ext_addr, ext_addr, u, loc_addr, loc_addr);
//                            printf_info("Cluster %i, Load ext from (0x%08X) to loc (unit %i, 0x%08X).", cluster->cluster_id,
//                                        ext_addr, u, loc_addr);
                        }
                    }
                    if (ext_addr <= main_memory_size - 1) {  // address inside main_memory array
                        if (debug & DEBUG_DMA_DETAIL)
                            printf_info(" MM ");
                        data = (*((uint16_t *) &main_memory[ext_addr]));
                    } else if (command->ext_base <= main_memory_size - 1 && ext_addr > main_memory_size - 1) {
                        printf_warning(
                                "Possible access to invalid data detected! base address inside main memory, address iteration outside main memory!\n");
                        printf_error("DMAs current command: ");
                        command->print();
                        printf("\n");
                    } else if (ext_addr > main_memory_size -
                                          1) { //} || ext_addr < SIM_MEM_DATA_SEGMENT_BEGIN){ // pointer to static object or similar not in mm...
                        if (ext_addr_base_lst != command->ext_base + THIS_IS_LOAD &&
                            debug & DEBUG_DMA_ACCESS_TO_EXT_VARIABLE) {
                            for (auto u : command->unit) {
                                printf_info(
                                        "DMA Load from variable ext of main_memory. From (0x%" PRIx64 ") to loc (unit %i, %i = 0x%08X)\n",
                                        ext_addr, u, loc_addr, loc_addr);
                            }
                            command->print();
                            printf("\n");
                            ext_addr_base_lst = command->ext_base + THIS_IS_LOAD;
                        }
                        if (debug & DEBUG_DMA_DETAIL)
                            printf_info(" EXT Variable ");
                        uint16_t *obj;
                        obj = (uint16_t *) ext_addr;
                        try {
    //                        data = uint32_t((uint8_t)(*obj)) + uint32_t((uint8_t)((*obj) >> 8));
                            data = uint32_t((uint16_t) *obj);
                        }
                        catch (std::exception &ex) {
                            printf_error("Access global variable or region of main memory which is too large! ");
                            printf_error("DMAs current command: ");
                            command->print();
                            exit(2);
                        }
                    }

                    if (debug & DEBUG_DMA_DETAIL) printf_info(" data: %i = 0x%04X \n", int16_t(data), int16_t(data));

                    // write to selected local memories
                    for (auto u : command->unit) {
                        units[u]->writeLocalMemoryData(loc_addr, data, (MAIN_MEMORY_DATA_WIDTH / 8));
                    }
                    break;
                case CommandDMA::LOC_1D_TO_EXT_1D:
                case CommandDMA::LOC_1D_TO_EXT_2D:

                    // get data from local memory
                    for (auto u : command->unit) {
                        if (debug & DEBUG_DMA) printf_info(".");
                        if (debug & DEBUG_DMA_DETAIL) {
                            printf_info("Cluster %i, Store from loc (unit %i, %i = 0x%08X) to ext (%i = 0x%08X).",
                                        cluster->cluster_id, u, loc_addr, loc_addr, ext_addr, ext_addr);
                        }

                        data = units[u]->getLocalMemoryData(loc_addr, (MAIN_MEMORY_DATA_WIDTH / 8));

                        if (ext_addr <= main_memory_size - 1) { // in mm...
                            for (uint j = 0; j < (MAIN_MEMORY_DATA_WIDTH / 8); j++) {
                                main_memory[ext_addr + j] = uint8_t(data >> (j * 8u));
                            }
                        } else {  // pointer to static object or similar not in mm...
                            if (ext_addr_base_lst != command->ext_base + THIS_IS_STORE &&
                                debug & DEBUG_DMA_ACCESS_TO_EXT_VARIABLE) {
                                printf_info(
                                        "DMA Store to variable ext of main_memory. From loc (unit %i, %i = 0x%08X) to (0x%" PRIx64 ")\n",
                                        u, loc_addr, loc_addr, ext_addr);
                                command->print();
                                printf("\n");
                                ext_addr_base_lst = command->ext_base + THIS_IS_STORE;
                            }
                            uint16_t *obj;
                            try {
                                obj = (uint16_t *) ext_addr;
                                *obj = uint16_t(data);
                            }
                            catch (std::exception &ex) {
                                printf_error("Access global variable outside addressable region! ");
                                printf_error("DMAs current command: ");
                                command->print();
                                exit(2);
                            }
                        }
                    }

                    if (debug & DEBUG_DMA_DETAIL) printf_info(" data: %i = 0x%04X \n", int16_t(data), int16_t(data));
                    break;
                default:
                    break;
            }// execute of type
        }
        // Update x and y
        command->x++;
        if (command->x >= command->x_size){
            command->x = 0;
            command->y++;
            if (command->y  >= command->y_size){
                if (debug & DEBUG_DMA) printf_info(" [DMA done]\n");
//                command->done = true;
                command = noneCmd;
            }
        }
    } else { // cmd is done
        stat.addExecutedCmdTick(command.get());
    }

}
