// ########################################################
// # VPRO instruction & system simulation library         #
// # Sven Gesper, IMS, Uni Hannover, 2019                 #
// ########################################################
// # Main entry for simulation.                           #
// # Here are the Core related functions                  #
// # -> call commands for vpro                            #
// # -> stop                                              #
// ########################################################
// # here are memory/DMA related functions                #
// ########################################################


#include <iostream>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <vector>
#include <bitset>

#include "simCore.h"
#include "helper/debugHelper.h"


void SimCore::run_vpro_instruction(std::shared_ptr<CommandVPRO> command) {
    runUntilReadyForCmd();
    check_vpro_instruction_length(command);
    for (auto cluster: clusters) {
        if (((1u << cluster->cluster_id) & cluster_mask_global) > 0) {
            if (!cluster->sendCMD(std::dynamic_pointer_cast<CommandBase>(command))) {
                printf_warning("Cluster did not receive this command... Queue full?");
                command->print();
            }
        }
    }

    if (CREATE_CMD_HISTORY_FILE) {
        QString cmd_description = "VPRO " +
                                  command->get_type().leftJustified(17, ' ') + ", id " +
                                  QString::number(command->id_mask, 2).leftJustified(3, ' ') + ", bl " +
                                  //                QString::number(command->fu_sel) + ", " +
                                  //                QString::number(command->func) + ", " +
                                  QString::number(command->blocking) + ", ch " +
                                  QString::number(command->is_chain) + ", flag " +
                                  QString::number(command->flag_update) + ", SRC1 " +
                                  command->src1.__toString() + ", SRC2 " +
                                  command->src2.__toString() + ", DST " +
                                  command->dst.__toString() + ", xE " +
                                  QString::number(command->x_end).leftJustified(3, ' ') + ", yE " +
                                  QString::number(command->y_end).leftJustified(3, ' ');

        *CMD_HISTORY_FILE_STREAM << cmd_description << "\n";
    }
}

void SimCore::run_dma_instruction(std::shared_ptr<CommandDMA> command) {
    runUntilReadyForCmd();
    for (auto cluster: clusters) {
        if (cluster->cluster_id == command->cluster)
            cluster->sendCMD(std::dynamic_pointer_cast<CommandBase>(command));
    }

    if (CREATE_CMD_HISTORY_FILE) {
        uint64_t unit_mask = 0lu;
        for (auto u : command->unit)
            unit_mask |= 1u << u;

        QString cmd_description = "DMA  " +
                                  command->get_type().leftJustified(17, ' ') + ", Cl " +
                                  QString::number(command->cluster).leftJustified(2, ' ') + ", uni_mask " +
                                  QString::number(unit_mask) + ", E " +
                                  QString::number(command->ext_base).leftJustified(10, ' ') + ", L " +
                                  QString::number(command->loc_base).leftJustified(4, ' ') + ", pad " +
                                  QString::number(command->pad[0]) + ", " +
                                  QString::number(command->pad[1]) + ", " +
                                  QString::number(command->pad[2]) + ", " +
                                  QString::number(command->pad[3]) + ", xE " +
                                  QString::number(command->x_size).leftJustified(3, ' ') + ", yE " +
                                  QString::number(command->y_size).leftJustified(3, ' ');

        *CMD_HISTORY_FILE_STREAM << cmd_description << "\n";
    }
}


//todo move to core_class_wrapper
// ###############################################################################################################################
// Simulate actual VPRO instruction word
// ###############################################################################################################################
void SimCore::__builtin_vpro_instruction_word(uint32_t id, uint32_t blocking, uint32_t is_chain, uint32_t fu_sel,
                                              uint32_t func, uint32_t flag_update, uint32_t dst, uint32_t src1_sel,
                                              uint32_t src1, uint32_t src2_sel, uint32_t src2, uint32_t x_end,
                                              uint32_t y_end) {

    auto command = std::make_shared<CommandVPRO>();

//    command->dst.sel = SRC_SEL_ADDR;              // must be ADDR! default.
//    command->dst.imm = dst & ISA_COMPLEX_MASK;    // never used for dst
    command->dst.beta = dst & ISA_BETA_MASK;
    command->dst.alpha = (dst >> ISA_ALPHA_SHIFT) & ISA_ALPHA_MASK;
    command->dst.offset = (dst >> ISA_OFFSET_SHIFT) & ISA_OFFSET_MASK;

    command->src1.sel = (uint8_t) src1_sel;
    command->src1.imm = src1 & ISA_COMPLEX_MASK;
    command->src1.beta = src1 & ISA_BETA_MASK;
    command->src1.alpha = (src1 >> ISA_ALPHA_SHIFT) & ISA_ALPHA_MASK;
    command->src1.offset = (src1 >> ISA_OFFSET_SHIFT) & ISA_OFFSET_MASK;
    if (src1_sel == SRC_SEL_CHAIN) {
        command->src1.chain_id = 1 + (src1 &
                                      ISA_CHAIN_ID_MASK); // TODO chain id + 1 is ugly! use of src1_sel ? -> e.g. in Lanes forward,...
        command->src1.chain_left = ((src1 & 0x8300) == 0x8100);
        command->src1.chain_right = ((src1 & 0x8300) == 0x8200);
    } else if (src1_sel == SRC_SEL_LS) {
        command->src1.chain_ls = true;
        command->src1.chain_left = ((src1 & 0x8300) == 0x8100);
        command->src1.chain_right = ((src1 & 0x8300) == 0x8200);
    }
    command->src1.delayed_chain = (command->src1.chain_left || command->src1.chain_right || command->src1.chain_ls) &&
                                  (src1 & 0x0002) == 0x0002;

    command->src2.sel = (uint8_t) src2_sel;
    command->src2.imm = src2 & ISA_COMPLEX_MASK;
    command->src2.beta = src2 & ISA_BETA_MASK;
    command->src2.alpha = (src2 >> ISA_ALPHA_SHIFT) & ISA_ALPHA_MASK;
    command->src2.offset = (src2 >> ISA_OFFSET_SHIFT) & ISA_OFFSET_MASK;
    if (src2_sel == SRC_SEL_CHAIN) {
        command->src2.chain_id = 1 + (src2 & ISA_CHAIN_ID_MASK);
        command->src2.chain_left = ((src2 & 0x8300) == 0x8100);
        command->src2.chain_right = ((src2 & 0x8300) == 0x8200);
    } else if (src2_sel == SRC_SEL_LS) {
        command->src2.chain_ls = true;
        command->src2.chain_left = ((src2 & 0x8300) == 0x8100);
        command->src2.chain_right = ((src2 & 0x8300) == 0x8200);
    }
    command->src2.delayed_chain = (command->src2.chain_left || command->src2.chain_right || command->src2.chain_ls) &&
                                  (src2 & 0x0002) == 0x0002;

    command->is_chain = (is_chain == IS_CHAIN);
    command->blocking = (blocking == BLOCKING);
    command->fu_sel = fu_sel;
    command->func = func;
    command->flag_update = (flag_update == FLAG_UPDATE);
    command->x_end = x_end;
    command->y_end = y_end;
    command->id_mask = id;
    command->updateType();


//    // cannot chain from both sides at the same time
//    if ((src1_sel == SRC_SEL_CHAIN) && (src2_sel == SRC_SEL_CHAIN)) {
//        printf_error("VPRO_SIM ERROR: Cannot forward from both sides at the same time! (unit=%d, fu=%d, func=%d, src1_sel=%d, src2_sel=%d)\n",
//                id, fu_sel, func, src1_sel, src2_sel);
//        print_cmd(command.get());
//
//        printSimResults();
//        exit(EXIT_FAILURE); // will continue?  with main!?
//        std::_Exit(EXIT_FAILURE);
//        exit(1);
//    }
//    // cannot use blocking operation for receiving lane
//    if ((blocking == 1) && is_chain == IS_CHAIN){
//        printf_error("VPRO_SIM ERROR: Chain-emitting lane uses blocking! This will cause a DEADLOCK! (unit=%d, fu=%d, func=%d, src1_sel=%d, src2_sel=%d)\n",
//                id, fu_sel, func, src1_sel, src2_sel);
//        print_cmd(command.get());
//
//        printSimResults();
//        exit(EXIT_FAILURE); // will continue?  with main!?
//        std::_Exit(EXIT_FAILURE);
//        exit(1);
//    }

    run_vpro_instruction(command);
}

void SimCore::check_vpro_instruction_length(std::shared_ptr<CommandVPRO> command) {
    if (SIM_ONLY_WARNING_PRINTED_ONCE_XEND_YEND) {
        if (command->x_end > MAX_X_END) {
            printf_error("ERROR! x_end out of range (is %d, max is %i)!\n", command->x_end, MAX_X_END);
            print_cmd(command.get());
        }
        if (command->y_end > MAX_Y_END) {
            printf_error("ERROR! y_end out of range (is %d, max is %i)!\n", command->y_end, MAX_Y_END);
            print_cmd(command.get());
        }
        if ((command->x_end > MAX_X_END) || (command->y_end > MAX_Y_END)) {
            char ans = 'N';
            std::cout << ORANGE << "\n------------------------------------------------------------------\n\n" <<
                      "This program will only work in simulation. For VPRO emulation/HW-system you'll have to fix the length of x_end/y_end.\n"
                      <<
                      "This message will not appear again in this simulation run. \n" <<
                      "Do you want to continue (type in: 'y'/'n')?\n>" << RESET_COLOR;
            std::cin >> ans;
            if ((ans == 'Y') || (ans == 'y')) {
                SIM_ONLY_WARNING_PRINTED_ONCE_XEND_YEND = false;
            } else {
                printSimResults();
                exit(EXIT_FAILURE); // will continue?  with main!?
                std::_Exit(EXIT_FAILURE);
                exit(1);
            }
        }
    }
}


// ###############################################################################################################################
// VPRO Status/Control
// ###############################################################################################################################

//todo move to core_class_wrapper
// ***********************************************************************
// Wait for specified vector units to finish operations
// ***********************************************************************
void SimCore::vpro_wait_busy(uint32_t cluster_mask, uint32_t unit_mask) {
    runUntilMIPSReadyForCmd();

    for (auto cluster: clusters) {
        if (((1u << cluster->cluster_id) & cluster_mask) != 0) {
            cluster->setWaitingForVPRO(true);
        }
    }
    if (debug & DEBUG_INSTRUCTIONS)
        printf("\tCluster_mask %i got WAIT_BUSY (@clock = %li). Waiting for VPRO to finish current commands...\n",
               cluster_mask, clock);

    // evaluate busy and run until no more busy
    while (true) {
        bool busy = false;
        for (auto cluster: clusters) {
            if (((1u << cluster->cluster_id) & cluster_mask) != 0) {
                for (VectorUnit *unit : cluster->getUnits()) {
                    if ((unit_mask & (1u << unit->vector_unit_id)) > 0) {
                        busy |= unit->isBusy();
                    }
                }
            }
        }
        if (busy) {
            run(); // next vpro/mips is on start of new func...
        } else {
            break;
        }
    }

    if (debug & DEBUG_INSTRUCTIONS)
        printf("\tCluster_mask %i (@clock = %li); VPRO finished current commands! [Waiting ommited]\n", cluster_mask,
               clock);

    for (auto cluster: clusters) {
        if (((1u << cluster->cluster_id) & cluster_mask) != 0) {
            cluster->setWaitingForVPRO(false);
        }
    }

    if (CREATE_CMD_HISTORY_FILE) {
        QString cmd_description = "MIPS " + QString("vpro_wait_busy").leftJustified(17, ' ')+ ", c: 0x" + QString::number(cluster_mask, 16) + ", u: 0x" +
                                  QString::number(unit_mask, 16);
        *CMD_HISTORY_FILE_STREAM << cmd_description << "\n";
    }
}

//todo move to core_class_wrapper
// ***********************************************************************
// Set global vector instruction ID mask
// ***********************************************************************
void SimCore::vpro_set_unit_mask(uint32_t unit_mask) {
    runUntilMIPSReadyForCmd();

    unit_mask_global = unit_mask;

    if (debug & DEBUG_INSTRUCTIONS) {
        printf("ID Mask Command received %x \n", unit_mask);
    }
    if (CREATE_CMD_HISTORY_FILE) {
        QString cmd_description = "MIPS " + QString("vpro_set_unit_mask").leftJustified(17, ' ')+ ", u: 0x" + QString::number(unit_mask, 16);
        *CMD_HISTORY_FILE_STREAM << cmd_description << "\n";
    }
}

//todo move to core_class_wrapper
// ***********************************************************************
// Set global vector instruction ID mask
// ***********************************************************************
void SimCore::vpro_set_cluster_mask(uint32_t cluster_mask) {
    runUntilMIPSReadyForCmd();

    cluster_mask_global = cluster_mask;

    if (debug & DEBUG_INSTRUCTIONS) {
        printf("ID Mask Command received %x \n", cluster_mask);
    }
    if (CREATE_CMD_HISTORY_FILE) {
        QString cmd_description = "MIPS " + QString("vpro_set_cluster_mask").leftJustified(17, ' ')+ ", c: 0x" + QString::number(cluster_mask, 16);
        *CMD_HISTORY_FILE_STREAM << cmd_description << "\n";
    }
}

// todo or
// ***********************************************************************
// Set global vector instruction ID mask
// ***********************************************************************
//void SimCore::vpro_set_cluster_mask_global(uint32_t cluster_mask) {
//    runUntilReadyForCmd();
//    uint32_t cluster_mask_global;
//
//    auto command = std::make_shared<CommandVPRO>();
//    command->type = CommandVPRO::IDMASK_GLOBAL;
//    command->id_mask = idmask;
//    command->updateType();
//
//    if (debug & DEBUG_INSTRUCTIONS) {
//        printf("ID Mask Command received ");
//        print_cmd(command.get());
//    }
//    unit_mask_global = command->id_mask;
//}

//todo move to core_class_wrapper
// ***********************************************************************
// Wait until pipeline is "empty"
// ***********************************************************************
void SimCore::vpro_pipeline_wait(void) {
    printf_warning("PIPELINE WAIT NOT YET IMPLEMENTED!");
    auto command = std::make_shared<CommandVPRO>();
    command->type = CommandVPRO::PIPELINE_WAIT;
    command->x_end = 11;
    command->updateType();

    run_vpro_instruction(command);
}


// Start HW LOOPING
void SimCore::vpro_loop_start(uint32_t start, uint32_t end, uint32_t increment_step) {
    auto command = std::make_shared<CommandVPRO>();
    command->type = CommandVPRO::LOOP_START;
    command->updateType();

    command->x = start;
    command->x_end = end;
    command->y = increment_step;

    run_vpro_instruction(command);

    if (debug & DEBUG_INSTRUCTIONS) {
        printf("VPRO LOOP Start received \n");
    }
    if (CREATE_CMD_HISTORY_FILE) {
        QString cmd_description =
                "VPRO Loop Start (i = " + QString::number(start) + "; i < " + QString::number(end) + "; i += " +
                QString::number(increment_step) + ")";
        *CMD_HISTORY_FILE_STREAM << cmd_description << "\n";
    }
}


// End HW LOOPING
void SimCore::vpro_loop_end() {
    auto command = std::make_shared<CommandVPRO>();
    command->type = CommandVPRO::LOOP_END;
    command->updateType();

    run_vpro_instruction(command);

    if (debug & DEBUG_INSTRUCTIONS) {
        printf("VPRO LOOP END received \n");
    }
    // TODO: while looping is active && filling complete -> run()!
    // TODO: lane -> get cmd from unit should get looped cmd if looping active...
    if (CREATE_CMD_HISTORY_FILE) {
        QString cmd_description = "VPRO Loop End";
        *CMD_HISTORY_FILE_STREAM << cmd_description << "\n";
    }
}


// Set mask for commands performed HW LOOPING
void SimCore::vpro_loop_mask(uint32_t src1_mask, uint32_t src2_mask, uint32_t dst_mask) {
    runUntilMIPSReadyForCmd();

    loop_mask_src1 = src1_mask;
    loop_mask_src2 = src2_mask;
    loop_mask_dst = dst_mask;

    if (debug & DEBUG_INSTRUCTIONS) {
        printf("VPRO LOOP MASK received SRC1 %x, SRC2 %x , DST %x \n", src1_mask, src2_mask, dst_mask);
    }
    if (CREATE_CMD_HISTORY_FILE) {
        QString cmd_description =
                "VPRO Loop Mask, src1: " + QString::number(src1_mask) + ", src2: " + QString::number(src2_mask) +
                ", dst: " + QString::number(dst_mask);
        *CMD_HISTORY_FILE_STREAM << cmd_description << "\n";
    }
}

void SimCore::vpro_mac_h_bit_shift(uint32_t shift) {
    runUntilMIPSReadyForCmd();
    if (shift > 24) {
        printf_error("vpro_mac_h_bit_shift(); Set ACCU_MAC_HIGH_BIT_SHIFT > 24! Overflow! set to 24... \n");
        shift = 24;
    }

    ACCU_MAC_HIGH_BIT_SHIFT = shift;

    if (debug & DEBUG_INSTRUCTIONS) {
        printf("ACCU_MAC_HIGH_BIT_SHIFT received %i \n", ACCU_MAC_HIGH_BIT_SHIFT);
    }
    if (CREATE_CMD_HISTORY_FILE) {
        QString cmd_description = "MIPS vpro_mac_h_bit_shift, shift: " + QString::number(shift);
        *CMD_HISTORY_FILE_STREAM << cmd_description << "\n";
    }
}

void SimCore::vpro_mul_h_bit_shift(uint32_t shift) {
    runUntilMIPSReadyForCmd();
    if (shift > 24) {
        printf_error("vpro_mul_h_bit_shift(); Set ACCU_MUL_HIGH_BIT_SHIFT > 24! Overflow! set to 24... \n");
        shift = 24;
    }
    ACCU_MUL_HIGH_BIT_SHIFT = shift;

    if (debug & DEBUG_INSTRUCTIONS) {
        printf("ACCU_MUL_HIGH_BIT_SHIFT received %i \n", ACCU_MUL_HIGH_BIT_SHIFT);
    }
    if (CREATE_CMD_HISTORY_FILE) {
        QString cmd_description = "MIPS vpro_mul_h_bit_shift, shift: " + QString::number(shift);
        *CMD_HISTORY_FILE_STREAM << cmd_description << "\n";
    }
}



// ###############################################################################################################################
// DMA Transfers
// ###############################################################################################################################

// ***********************************************************************
// External 1D to local 1D transfer
// ***********************************************************************
void SimCore::dma_ext1D_to_loc1D(uint32_t cluster, uint64_t ext_base, uint32_t loc_base, uint32_t word_count,
                                 uint32_t x_stride, uint32_t x_size, uint32_t y_size, bool is_broadcast_no_checking) {
    auto command = std::make_shared<CommandDMA>();
    command->type = CommandDMA::EXT_1D_TO_LOC_1D;
    command->cluster = cluster;
    command->ext_base = ext_base;
    command->loc_base = loc_base & 0x3ffffu; // use 22-bit
    command->x_size = word_count & 0b1111111111111u; // use 13-bit!
    command->x_stride = 0;
    command->y_size = 1;

    if (!is_broadcast_no_checking) {
        if (command->x_size != word_count) {
            printf_error("DMA Ext 1D to Loc 1D, word_count overflow (max 13-bit): %i != %i\n", command->x_size,
                         word_count);
        }
    }

    uint64_t units = (uint64_t(x_stride & 0xffffe000) << 25u) |
                     // use upper 19-bits of x_stride (drop 13-bits of x_stride) concat with
                     (uint64_t(x_size & 0xffffe000) << 6u) |
                     // use upper 19-bits of x_size   (drop 13-bits of x_size  ) concat with
                     (uint64_t(y_size & 0xffffe000)
                             >> 13u);   // use upper 19-bits of y_size   (drop 13-bits of y_size  ) => result with 3*19-bit = 57-bit

    for (uint i = 0; i < 57u && i < vector_unit_count; i++) { // check each bit
        if (((units >> i) & 0b1u) == 1) // bit at position i is set
            command->unit.push_back(i);
    }
    if (command->unit.empty()) {
        command->unit.push_back(loc_base >> 22u);
    }

    run_dma_instruction(command);
}

// ***********************************************************************
// External 2D to local 1D transfer
// ***********************************************************************
/**
 * allgemeine beschreibung der funktion
 * @param cluster dies ist ein cluster
 * @param ext_base
 * @param loc_base
 * @param x_stride
 * @param x_size
 * @param y_size
 */
void
SimCore::dma_ext2D_to_loc1D(uint32_t cluster, uint64_t ext_base, uint32_t loc_base, int32_t x_stride, uint32_t x_size,
                            uint32_t y_size, bool pad_flags[4], bool is_broadcast_no_checking) {
    auto command = std::make_shared<CommandDMA>();
    command->type = CommandDMA::EXT_2D_TO_LOC_1D;
    command->cluster = cluster;
    command->ext_base = ext_base;   // use 32-bit
    command->loc_base = loc_base & 0x3ffffu; // use 22-bit
    command->x_size = x_size & 0b1111111111111u; // use 13-bit!
    if (x_stride < 0)
        command->x_stride = (x_stride & 0b1111111111111u) | 0b11111111111111111110000000000000; // sign extension
    else
        command->x_stride = (x_stride & 0b1111111111111u);
    command->y_size = y_size & 0b1111111111111u;

    command->pad[0] = pad_flags[0];
    command->pad[1] = pad_flags[1];
    command->pad[2] = pad_flags[2];
    command->pad[3] = pad_flags[3];

    if (!is_broadcast_no_checking) {
        if (command->x_size != x_size) {
            printf_error("DMA Ext 2D to Loc 1D, x_size overflow (max 13-bit): %i != %i\n", command->x_size, x_size);
        }
        if ((x_stride > 0 && (command->x_stride != x_stride)) ||
            (x_stride < 0 && ((x_stride & 0b11111111111111111110000000000000) != 0b11111111111111111110000000000000))) {
            if (x_stride > 0)
                printf_error("DMA Ext 2D to Loc 1D, x_stride overflow (max 13-bit): %i != %i\n", command->x_stride,
                             x_stride);
            else
                printf_error("DMA Ext 2D to Loc 1D, x_stride overflow (max 13-bit): %i != %i\n",
                             command->x_stride | 0b11111111111111111110000000000000, x_stride);
        }
        if (command->y_size != y_size) {
            printf_error("DMA Ext 2D to Loc 1D, y_size overflow (max 13-bit): %i != %i\n", command->y_size, y_size);
        }
    }

    uint64_t units = (uint64_t(x_stride & 0xffffe000) << 25u) |
                     // use upper 19-bits of x_stride (drop 13-bits of x_stride) concat with
                     (uint64_t(x_size & 0xffffe000) << 6u) |
                     // use upper 19-bits of x_size   (drop 13-bits of x_size  ) concat with
                     (uint64_t(y_size & 0xffffe000)
                             >> 13u);   // use upper 19-bits of y_size   (drop 13-bits of y_size  ) => result with 3*19-bit = 57-bit


    for( uint i = 0 ; i < 57u && i < vector_unit_count ; i++){ // check each bit
        if (((units >> i) & 0b1u) == 1){ // bit at position i is set
            //printf("unit %i \n", i);
            command->unit.push_back(i);
        }
    }
    if (command->unit.empty()) {
        command->unit.push_back(loc_base >> 22u);
    }

    run_dma_instruction(command);
}


// ***********************************************************************
// Local 1D to external 1D transfer
// ***********************************************************************
void SimCore::dma_loc1D_to_ext1D(uint32_t cluster, uint64_t ext_base, uint32_t loc_base, uint32_t word_count) {
    auto command = std::make_shared<CommandDMA>();
    command->type = CommandDMA::LOC_1D_TO_EXT_1D;
    command->cluster = cluster;
    command->ext_base = ext_base;
    command->loc_base = loc_base;
    command->x_size = word_count;
    command->x_stride = 0;
    command->y_size = 1;
    command->unit.push_back(loc_base >> 22);

    run_dma_instruction(command);
}


// ***********************************************************************
// Local 1D to external 2D transfer
// ***********************************************************************
void
SimCore::dma_loc1D_to_ext2D(uint32_t cluster, uint64_t ext_base, uint32_t loc_base, int32_t x_stride, uint32_t x_size,
                            uint32_t y_size) {

    auto command = std::make_shared<CommandDMA>();
    command->type = CommandDMA::LOC_1D_TO_EXT_2D;
    command->cluster = cluster;
    command->ext_base = ext_base;
    command->loc_base = loc_base;
    command->x_size = x_size & 0b1111111111111u; // use 13-bit!
    if (x_stride < 0)
        command->x_stride = (x_stride & 0b1111111111111u) | 0b11111111111111111110000000000000; // sign extension
    else
        command->x_stride = (x_stride & 0b1111111111111u);
    command->y_size = y_size & 0b1111111111111u;
    command->unit.push_back(loc_base >> 22);

    if (command->x_size != x_size) {
        printf_error("DMA Ext 1D to Loc 1D, x_size overflow (max 13-bit): %i != %i\n", command->x_size, x_size);
    }
    if ((x_stride > 0 && (command->x_stride != x_stride)) ||
        (x_stride < 0 && ((x_stride & 0b11111111111111111110000000000000) != 0b11111111111111111110000000000000))) {
        if (x_stride > 0)
            printf_error("DMA Ext 1D to Loc 1D, x_stride overflow (max 13-bit): %i != %i\n", command->x_stride,
                         x_stride);
        else
            printf_error("DMA Ext 1D to Loc 1D, x_stride overflow (max 13-bit): %i != %i\n",
                         command->x_stride | 0b11111111111111111110000000000000, x_stride);
    }
    if (command->y_size != y_size) {
        printf_error("DMA Ext 1D to Loc 1D, y_size overflow (max 13-bit): %i != %i\n", command->y_size, y_size);
    }

    run_dma_instruction(command);
}

// ***********************************************************************
// Block until all DMA transactions are done
// ***********************************************************************
void SimCore::dma_wait_to_finish(uint32_t cluster_mask) {
    runUntilMIPSReadyForCmd();

    if (debug & DEBUG_INSTRUCTIONS)
        printf("\tCluster_mask %i got WAIT_FINISH (@clock = %li). Waiting for DMA to finish current commands...\n",
               cluster_mask, clock);

    for (auto cluster: clusters) {
        if (((1u << cluster->cluster_id) & cluster_mask) != 0)
            cluster->setWaitingForDMA(true);
    }

    // evaluate busy and run until no more busy
    while (true) {
        bool busy = false;
        for (auto cluster: clusters) {
            if (((1u << cluster->cluster_id) & cluster_mask) != 0)
                busy |= cluster->dma->isBusy();
        }
        if (busy) {
            run(); // next vpro/mips is on start of new func...
        } else {
            break;
        }
    }
    if (debug & DEBUG_INSTRUCTIONS)
        printf("\tCluster_mask %i (@clock = %li); DMA finished current commands! [Waiting ommited]\n", cluster_mask,
               clock);

    for (auto cluster: clusters) {
        if (((1u << cluster->cluster_id) & cluster_mask) != 0)
            cluster->setWaitingForDMA(false);
    }
    if (CREATE_CMD_HISTORY_FILE) {
        QString cmd_description = "DMA  dma_wait_to_finish, c: " + QString::number(cluster_mask);
        *CMD_HISTORY_FILE_STREAM << cmd_description << "\n";
    }
}


void SimCore::dma_set_pad_widths(uint32_t top, uint32_t right, uint32_t bottom, uint32_t left) {
    runUntilMIPSReadyForCmd();

    dma_pad_top = top;
    dma_pad_right = right;
    dma_pad_bottom = bottom;
    dma_pad_left = left;

    if (debug & DEBUG_INSTRUCTIONS) {
        printf("DMA PAD Widths received top %i, right %i, bottom %i, left %i \n", top, right, bottom, left);
    }
    if (CREATE_CMD_HISTORY_FILE) {
        QString cmd_description =
                "DMA  dma_set_pad_widths, top: " + QString::number(top) + ", right: " + QString::number(right) +
                ", bottom: " + QString::number(bottom) + ", left: " + QString::number(left);
        *CMD_HISTORY_FILE_STREAM << cmd_description << "\n";
    }
}

void SimCore::dma_set_pad_value(int16_t value) {
    runUntilMIPSReadyForCmd();

    dma_pad_value = value;

    if (debug & DEBUG_INSTRUCTIONS) {
        printf("DMA PAD Value received %i \n", value);
    }
    if (CREATE_CMD_HISTORY_FILE) {
        QString cmd_description = "DMA  dma_set_pad_value, value: " + QString::number(value);
        *CMD_HISTORY_FILE_STREAM << cmd_description << "\n";
    }
}




// ###############################################################################################################################
// System
// ###############################################################################################################################

void SimCore::aux_print_debugfifo(uint32_t data) {
    runUntilMIPSReadyForCmd();
    if (debug & DEBUG_INSTRUCTIONS) {
        printf("Sim print debug fifo \n");
    }
    if (debug & DEBUG_FIFO_MSG) {
        printf("#SIM DEBUG_FIFO: 0x%08x\n", data);
    }
}

void SimCore::aux_dev_null(uint32_t data) {
    runUntilMIPSReadyForCmd();
    if (debug & DEBUG_INSTRUCTIONS) {
        printf("Sim print dev null \n");
    }
    if (debug & DEBUG_DEV_NULL) {
        printf("#SIM DEV_NULL: 0x%08x\n", data);
    }
}

/*
uint32_t SimCore::aux_dev_null(uint32_t data) {
    runUntilReadyForCmd();
    auto cmd = new CommandSim();
    cmd->type = CommandSim::AUX_DEV_NULL;
    cmd->data = data;

    if (debug & DEBUG_DEV_NULL){
        printf("#SIM DEV_NULL: 0x%08x\n", cmd->data);
    }
}
*/

// ***********************************************************************
// Write and read data to/from DEV_NULL
// ***********************************************************************
void SimCore::aux_flush_dcache() {
    runUntilMIPSReadyForCmd();
    if (debug & DEBUG_INSTRUCTIONS) {
        printf("Sim flush dcache \n");
    }
    // just a dummy
}


// ***********************************************************************
// Clear system timer
// ***********************************************************************
void SimCore::aux_clr_sys_time() {
    runUntilMIPSReadyForCmd();
    aux_sys_time = 0;
    // just a dummy
    if (debug & DEBUG_INSTRUCTIONS) {
        printf("Sim aux clr sys time \n");
    }
}

// ***********************************************************************
// Clear CPU-internal cycle counter
// ***********************************************************************
void SimCore::aux_clr_cycle_cnt() {
    runUntilMIPSReadyForCmd();
    aux_cycle_counter = 0;
    // just a dummy
    if (debug & DEBUG_INSTRUCTIONS) {
        printf("Sim aux clr cycle caunt \n");
    }
}

// ***********************************************************************
// Get CPU-internal cycle counter
// ***********************************************************************
uint32_t SimCore::aux_get_cycle_cnt() {
    runUntilMIPSReadyForCmd();

    if (debug & DEBUG_INSTRUCTIONS) {
        printf("Sim get cycle cnt %i \n", aux_cycle_counter);
    }
    return aux_cycle_counter;
}

// ***********************************************************************
// Send system run time
// ***********************************************************************
void SimCore::aux_send_system_runtime() {
    runUntilMIPSReadyForCmd();
    printf("\n#SIM aux_sys_time send: 0x%08x\n", aux_sys_time);
    // just a dummy
    if (debug & DEBUG_INSTRUCTIONS) {
        printf("Sim send system runtime \n");
    }
}








// ###############################################################################################################################
// Debugging Functions
// ###############################################################################################################################

void SimCore::sim_dump_local_memory(uint32_t cluster, uint32_t unit) {

    if (debug & DEBUG_USER_DUMP) {
        if (unit > vector_unit_count) {
            printf_warning("#SIM_DUMP: Invalid unit selection (%d)!. Maximum is %d.", unit, vector_unit_count);
        } else if (cluster > cluster_count) {
            printf_warning("#SIM_DUMP: Invalid cluster selection (%d)!. Maximum is %d.", cluster, cluster_count);
        } else {
            printf("#SIM_DUMP: Local memory, cluster=%d unit=%d\n", cluster, unit);
            this->clusters[cluster]->dumpLocalMemory(unit);
        }
    }
}

void SimCore::sim_dump_queue(uint32_t cluster, uint32_t unit) {

    if (debug & DEBUG_USER_DUMP) {
        if (unit > vector_unit_count) {
            printf_warning("#SIM_DUMP: Invalid unit selection (%d)!. Maximum is %d.", unit, vector_unit_count);
        } else if (cluster > cluster_count) {
            printf_warning("#SIM_DUMP: Invalid cluster selection (%d)!. Maximum is %d.", cluster, cluster_count);
        } else {
            printf("#SIM_DUMP: Local memory, cluster=%d unit=%d\n", cluster, unit);
            this->clusters[cluster]->dumpQueue(unit);
        }
    }
}

void SimCore::sim_dump_register_file(uint32_t cluster, uint32_t unit, uint32_t lane) {

    if (debug & DEBUG_USER_DUMP) {
        if (unit > vector_unit_count) {
            printf_warning("#SIM_DUMP: Invalid unit selection (%d)!. Maximum is %d.", unit, vector_unit_count);
        } else if (cluster > cluster_count) {
            printf_warning("#SIM_DUMP: Invalid cluster selection (%d)!. Maximum is %d.", cluster, cluster_count);
        } else if (lane > vector_lane_count) {
            printf_warning("#SIM_DUMP: Invalid lane selection (%d)!. Maximum is %d.", lane, vector_lane_count);
        } else {
            printf("#SIM_DUMP: Register file, cluster=%d unit=%d lane=%d\n", cluster, unit, lane);
            this->clusters[cluster]->dumpRegisterFile(unit, lane);
        }
    }
}


void SimCore::sim_wait_step(bool finish, char const *msg) {
    printf("%s", msg);
    if (finish) {
        dma_wait_to_finish(0xffffffff);
        vpro_wait_busy(0xffffffff, 0xffffffff);
    }

    sendSimUpdate();
    printf(ORANGE);

    if (!windowless && GUI_WAITKEY) {
        bool is_pressed = false;
        emit sendGuiwaitkeypress(&is_pressed);
        while (!is_pressed) {
            usleep(200);
        }
    } else {
        printf("Press [Enter] to continue...\n");
        getchar();
    }

    printf("execution resumed!\n");
    printf(RESET_COLOR);
}


// copies data in simulated main memory
int SimCore::bin_file_send(uint32_t addr, int num_bytes, char const *file_name) {
    if (debug & DEBUG_MODE) {
        printf("\n#SIM: bin_file_send\nBase: 0x%08x\nSize: %d bytes\nSrc:  %s\n\n", addr, num_bytes, file_name);
    }
    while ((num_bytes & 7) != 0) { num_bytes++; }// must be a multiple of 8

    auto pfh = fopen(file_name, "rb");
    if (pfh == NULL) {
        printf_error("Unable to open file %s!\n", file_name);
        return -1;
    }

    auto buf = (uint8_t *) malloc(num_bytes * sizeof(uint8_t));
    auto read = fread(buf, 1, num_bytes, pfh);
    fclose(pfh);

    // copy to simulated main memory
    for (int i = 0; i < (uint32_t) num_bytes; i++) {
        main_memory[addr + i] = *(buf + i);
        // main memory access in range?
        if ((addr + i) > (main_memory_size - 1)) {
            printf_error("\n#SIM: bin_file_send\n");
            printf_error("#SIM: Main memory access out of range!\n");
            printf_error("Access address: 0x%08x, max address: 0x%08x\nAborting.\n", addr + i, main_memory_size - 1);
            exit(1);
        }
    }
    free(buf);
    return 0;
}


// returns data from simulated main memory
uint8_t *SimCore::bin_file_return(uint32_t addr, int num_bytes) {
    if (debug & DEBUG_MODE) {
        printf("\n#SIM: bin_file_return\n");
        printf("Base: 0x%08x\n", addr);
        printf("Size: %d bytes\n", num_bytes);
    }
    while ((num_bytes & 7) != 0) { num_bytes++; }// must be a multiple of 8

    uint8_t *buf = (uint8_t *) malloc(num_bytes);
    for (int i = 0; i < (uint32_t) num_bytes; i++) { // copy from simulated main memory
        *(buf + i) = main_memory[addr + i];
        if ((addr + i) > (main_memory_size - 1)) { // main memory access in range?
            printf_error("\n#SIM: bin_file_return\n");
            printf_error("#SIM: Main memory access out of range!\n");
            printf_error("Access address: 0x%08x, max address: 0x%08x\nAborting.\n", addr + i, main_memory_size - 1);
            exit(1);
        }
    }
    return (buf);
}

// returns data from simulated main memory
uint16_t *SimCore::get_main_memory(uint32_t addr, unsigned int num_elements_16bit){
    if(num_elements_16bit == 0) printf("get_main_memory: num_elements_16bit <=0 -> no memory was allocated \n");

    unsigned int num_elements_8bit;
    num_elements_8bit = num_elements_16bit * 2;
    auto *buffer_8bit = (uint8_t *) malloc(num_elements_8bit * sizeof(uint8_t));
    if ((addr + num_elements_8bit) > (main_memory_size - 1)) { // main memory access in range?
        printf_error("\n#SIM: bin_file_return\n");
        printf_error("#SIM: Main memory access out of range!\n");
        printf_error("Access address: 0x%08x, max address: 0x%08x\nAborting.\n", addr + num_elements_8bit,
                     main_memory_size - 1);
        exit(1);
    }
    memcpy(buffer_8bit, (main_memory + addr), num_elements_8bit);
    auto buffer_16bit = (uint16_t *) buffer_8bit;
    return buffer_16bit;
}

uint8_t *SimCore::bin_lm_return(uint32_t cluster, uint32_t unit) {

    VectorUnit *u = this->clusters[cluster]->getUnits().at(unit);

    auto lmdata = new uint8_t[u->local_memory_size * u->LOCAL_MEMORY_DATA_WIDTH]();
    for (int a = 0; a < u->local_memory_size * u->LOCAL_MEMORY_DATA_WIDTH; a++)
        lmdata[a] = u->getlocalmemory()[a];

    return lmdata;
}

uint8_t *SimCore::bin_rf_return(uint32_t cluster, uint32_t unit, uint32_t lane) {

    VectorUnit *u = this->clusters[cluster]->getUnits().at(unit);
    VectorLane *l = u->getLanes().at(lane);

    auto rfdata = new uint8_t[l->register_file_size * 3]();
    for (int a = 0; a < l->register_file_size * 3; a++) {
        rfdata[a] = l->getregister()[a];
    }
    return rfdata;

}

// writes data from simulated main memory to file
int SimCore::bin_file_dump(uint32_t addr, int num_bytes, char const *file_name) {

    if (debug & DEBUG_MODE) {
        printf("\n#SIM: bin_file_dump\n");
        printf("Base: 0x%08x\n", addr);
        printf("Size: %d bytes\n", num_bytes);
        printf("Dst:  %s\n\n", file_name);
    }
    while ((num_bytes & 7) != 0) { num_bytes++; }// must be a multiple of 8

    auto buf = (uint8_t *) malloc(num_bytes * sizeof(uint8_t));

    for (int i = 0; i < (uint32_t) num_bytes; i++) { // copy from simulated main memory
        *(buf + i) = main_memory[addr + i];
        if ((addr + i) > (main_memory_size - 1)) { // main memory access in range?
            printf_error("\n#SIM: bin_file_dump\n");
            printf_error("#SIM: Main memory access out of range!\n");
            printf_error("Access address: 0x%08x, max address: 0x%08x\nAborting.\n", addr + i, main_memory_size - 1);
            exit(1);
        }
    }

    auto pfh = fopen(file_name, "wb+");
    if (pfh == NULL) {
        printf_error("Unable to open file %s!\n", file_name);
        return -1;
    }
    fwrite(buf, 1, num_bytes, pfh);
    fclose(pfh);
    free(buf);

//            if (debug & DEBUG_MODE){
//                for (int k=cmd->addr; k<(cmd->addr + (uint32_t)cmd->num_bytes); k++) {
//                    printf("%02x ", main_memory[k]);
//                }
//            }
    return 0;
}

void SimCore::aux_memset(uint32_t base_addr, uint8_t value, uint32_t num_bytes) {
    auto cmd = new CommandSim();
    cmd->type = CommandSim::AUX_MEMSET;
    cmd->num_bytes = num_bytes;
    cmd->value = value;
    cmd->addr = base_addr;

    if (debug & DEBUG_MODE) {
        printf("\n#SIM: aux_memset\n");
        printf("Base: 0x%08x\n", cmd->addr);
        printf("Data: 0x%02x\n", cmd->value);
        printf("Size: %d bytes\n", cmd->num_bytes);
    }
    for (int i = cmd->addr; i < (cmd->addr + cmd->num_bytes); i++) {
        // main memory access in range?
        if (i > (main_memory_size - 1)) {
            printf_error("\n#SIM: aux_memset\n");
            printf_error("#SIM: Main memory access out of range!\n");
            printf_error("Access address: 0x%08x, max address: 0x%08x\nAborting.\n", cmd->addr + i,
                         main_memory_size - 1);
            exit(1);
        }
        main_memory[i] = cmd->value;
    }
}

uint32_t SimCore::aux_get_sys_time_lo() {
    runUntilMIPSReadyForCmd();
    return aux_cycle_counter;
}

uint32_t SimCore::aux_get_sys_time_hi() {
    runUntilMIPSReadyForCmd();
    return aux_cycle_counter;
}
