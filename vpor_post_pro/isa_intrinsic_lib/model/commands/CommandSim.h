// ########################################################
// # VPRO instruction & system simulation library         #
// # Sven Gesper, IMS, Uni Hannover, 2019                 #
// ########################################################
// # a class for commands to the simulator                #
// ########################################################


#ifndef VPRO_CPP_COMMANDSIM_H
#define VPRO_CPP_COMMANDSIM_H

#include <string>

#include "CommandBase.h"

class CommandSim : public CommandBase {
public:
    CommandSim();
    CommandSim(CommandSim *ref);
    CommandSim(CommandSim &ref);

    enum TYPE{
        NONE,
        BIN_FILE_SEND,
        BIN_FILE_DUMP,
        BIN_FILE_RETURN,
        AUX_MEMSET,
        DUMP_LOCAL_MEM,
        DUMP_REGISTER_FILE,
        WAIT_STEP,
        PRINTF,
        AUX_PRINT_FIFO,
        AUX_DEV_NULL
    } type;

    // bin_file_send + bin_file_dump + bin_file_return
    uint32_t addr;
    int num_bytes;

    // bin_file_send + bin_file_dump
    char *file_name;

    // printf
    char *string;

    // memset
    uint8_t value;
    uint32_t data;

    // dump
    uint32_t cluster;
    uint32_t unit;
    uint32_t lane;

    bool is_done() override;
    void print(FILE * out = stdout);
    
    // String returning functions
    QString get_type(){
        switch (type){
            case NONE:                return QString("NONE           ");
            case BIN_FILE_SEND:       return QString("BIN_FILE_SEND  ");
            case BIN_FILE_DUMP:       return QString("BIN_FILE_DUMP  ");
            case BIN_FILE_RETURN:     return QString("BIN_FILE_RETURN");
            case AUX_MEMSET:          return QString("AUX_MEMSET     ");
            case DUMP_LOCAL_MEM:      return QString("DUMP_LOC_MEM   ");
            case DUMP_REGISTER_FILE:  return QString("DUMP_REG_FILE  ");
            case WAIT_STEP:           return QString("WAIT_STEP      ");
            case PRINTF:              return QString("PRINTF         ");
            case AUX_PRINT_FIFO:      return QString("AUX_PRINT_FIFO ");
            case AUX_DEV_NULL:        return QString("AUX_DEV_NULL   ");
        }
        return QString("");
    }
    QString get_string();

    static void printType(CommandSim::TYPE t, FILE * out = stdout){
        switch (t){
            case NONE:                fprintf(out, "NONE           ");    break;
            case BIN_FILE_SEND:       fprintf(out, "BIN_FILE_SEND  ");    break;
            case BIN_FILE_DUMP:       fprintf(out, "BIN_FILE_DUMP  ");    break;
            case BIN_FILE_RETURN:     fprintf(out, "BIN_FILE_RETURN");    break;
            case AUX_MEMSET:          fprintf(out, "AUX_MEMSET     ");    break;
            case DUMP_LOCAL_MEM:      fprintf(out, "DUMP_LOC_MEM   ");    break;
            case DUMP_REGISTER_FILE:  fprintf(out, "DUMP_REG_FILE  ");    break;
            case WAIT_STEP:           fprintf(out, "WAIT_STEP      ");    break;
            case PRINTF:              fprintf(out, "PRINTF         ");    break;
            case AUX_PRINT_FIFO:      fprintf(out, "AUX_PRINT_FIFO ");    break;
            case AUX_DEV_NULL:        fprintf(out, "AUX_DEV_NULL   ");    break;
        }
    }
};




#endif //VPRO_CPP_COMMANDSIM_H
