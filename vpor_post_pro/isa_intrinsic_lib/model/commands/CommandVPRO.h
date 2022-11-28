// ########################################################
// # VPRO instruction & system simulation library         #
// # Sven Gesper, IMS, Uni Hannover, 2019                 #
// ########################################################
// # a class for commands to the Vector processor itself  #
// ########################################################

#ifndef VPRO_CPP_COMMANDVPRO_H
#define VPRO_CPP_COMMANDVPRO_H

#include <QString>
#include <memory>

#include "CommandBase.h"
#include "../../isa_intrinsic_aux_lib.h"
#include "../../simulator/helper/structTypes.hpp"

class CommandVPRO : public CommandBase{
public:
    CommandVPRO();
    CommandVPRO(CommandVPRO *ref);
    CommandVPRO(CommandVPRO &ref);
    CommandVPRO(std::shared_ptr<CommandVPRO> ref);

    static const int MAX_ALU_DEPTH = 16;

    enum TYPE{
        NONE,
        LOAD,        LOADB,     LOADS,      LOADBS,     STORE,
        LOADS_SHIFT_LEFT, LOADS_SHIFT_RIGHT,    // data = data << pipe.dst.offset;
        STORE_SHIFT_LEFT, STORE_SHIFT_RIGHT,    // data = data << pipe.opb;
        LOAD_REVERSE, STORE_REVERSE,
        LOOP_START,  LOOP_END,  LOOP_MASK,
        ADD,         SUB,       MULL,       MULH,       MACL,       DIVL,	   DIVH,      MACH,    MACL_PRE,    MACH_PRE,
        XOR,         XNOR,      AND,        ANDN,       NAND,      OR,      ORN,         NOR,       SHIFT_LR,   SHIFT_AR, SHIFT_LL,
        ABS,         MIN,       MAX,        MIN_VECTOR,  MAX_VECTOR,    BIT_REVERSAL,
        MV_ZE,       MV_NZ,     MV_MI,      MV_PL,
        SHIFT_AR_NEG,  SHIFT_AR_POS,
        MULL_NEG,   MULL_POS, MULH_NEG,   MULH_POS,
        NOP,
        WAIT_BUSY,   PIPELINE_WAIT,
        IDMASK_GLOBAL,
        enumTypeEnd
    } type;

    uint32_t id_mask;
    uint32_t cluster;
//    uint32_t vector_unit_sel;
//    uint32_t vector_lane_sel;
    bool blocking;
    bool is_chain;
    uint32_t fu_sel;
    uint32_t func;
    bool flag_update;

    addr_field_t dst, src1, src2;

    uint32_t x_end;
    uint32_t x;
    uint32_t y_end;
    uint32_t y;

    uint32_t pipelineALUDepth;
    /**
     * update 'type' based on bits on 'fu_sel' && 'func'
     */
    void updateType();

    void print(FILE * out = stdout);
    void print_type(FILE * out = stdout);
    static void printType(CommandVPRO::TYPE t, FILE * out = stdout);

    // String returning functions
    QString get_type();
    QString get_string();
    
    static QString getType(CommandVPRO::TYPE t);

    void updateSrc1(int addToOffset = -1){
        src1.offset += addToOffset;
    }; // update Src1

    void updateSrc2(int addToOffset = -1){
        src2.offset += addToOffset;
    }; // update Src1

    bool isWriteRF();
    bool isWriteLM();
    bool isLS();

    bool isChainPartCMD();
    bool is_done() override;

    bool done;
private:
};


#endif //VPRO_CPP_COMMANDVPRO_H
