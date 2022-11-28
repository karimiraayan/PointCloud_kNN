//
// Created by gesper on 06.03.19.
//

#include "CommandVPRO.h"
#include "../../simulator/helper/debugHelper.h"

#include <sstream>
#include <iomanip>

CommandVPRO::CommandVPRO() : CommandBase(VPRO) {
    type = CommandVPRO::NONE;
    blocking = false;
    is_chain = false;
    fu_sel = CLASS_OTHER;
    func = OPCODE_ADD;
    flag_update = false;
    x_end = 0;
    x = 0;
    y_end = 0;
    y = 0;
    id_mask = 0xffffffff;
    cluster = 0;
    done = false;

    dst = addr_field_t();
    src1 = addr_field_t();
    src2 = addr_field_t();

    pipelineALUDepth = 3;
}
CommandVPRO::CommandVPRO(CommandVPRO *ref) : CommandBase(ref->class_type)
{
    type = ref->type;
    id_mask = ref->id_mask;
    cluster = ref->cluster;
//    vector_unit_sel = ref->vector_unit_sel;
//    vector_lane_sel = ref->vector_lane_sel;
    dst = ref->dst;
    src1 = ref->src1;
    src2 = ref->src2;
    blocking = ref->blocking;
    is_chain = ref->is_chain;
    fu_sel = ref->fu_sel;
    func = ref->func;
    flag_update = ref->flag_update;
    x_end = ref->x_end;
    x = ref->x;
    y_end = ref->y_end;
    y = ref->y;
    done = ref->done;
    pipelineALUDepth = ref->pipelineALUDepth;
}
CommandVPRO::CommandVPRO(CommandVPRO &ref) : CommandBase(ref.class_type)
{
    type = ref.type;
    id_mask = ref.id_mask;
    cluster = ref.cluster;
//    vector_unit_sel = ref.vector_unit_sel;
//    vector_lane_sel = ref.vector_lane_sel;
    dst = ref.dst;
    src1 = ref.src1;
    src2 = ref.src2;
    blocking = ref.blocking;
    is_chain = ref.is_chain;
    fu_sel = ref.fu_sel;
    func = ref.func;
    flag_update = ref.flag_update;
    x_end = ref.x_end;
    x = ref.x;
    y_end = ref.y_end;
    y = ref.y;
    done = ref.done;
    pipelineALUDepth = ref.pipelineALUDepth;
}
CommandVPRO::CommandVPRO(std::shared_ptr<CommandVPRO> ref) : CommandBase(ref->class_type)
{
    type = ref->type;
    id_mask = ref->id_mask;
    cluster = ref->cluster;
    //    vector_unit_sel = ref->vector_unit_sel;
    //    vector_lane_sel = ref->vector_lane_sel;
    dst = ref->dst;
    src1 = ref->src1;
    src2 = ref->src2;
    blocking = ref->blocking;
    is_chain = ref->is_chain;
    fu_sel = ref->fu_sel;
    func = ref->func;
    flag_update = ref->flag_update;
    x_end = ref->x_end;
    x = ref->x;
    y_end = ref->y_end;
    y = ref->y;
    done = ref->done;
    pipelineALUDepth = ref->pipelineALUDepth;
}

void CommandVPRO::printType(CommandVPRO::TYPE t, FILE * out){
	CommandVPRO c;
	c.type = t;
	c.print_type(out);
}
    
QString CommandVPRO::getType(CommandVPRO::TYPE t){
	CommandVPRO c;
	c.type = t;
	return c.get_type();
}
    
bool CommandVPRO::is_done() {
    return done || ( x > x_end && y > y_end );
//    if (type == CommandVPRO::NONE)
//        return true;
//    else
//        return done;
}

bool CommandVPRO::isWriteRF(){
    return (type == ADD  || type == SUB     || type == MULL    || type == MULH      || type == DIVL || type == DIVH ||
            type == MACL || type == MACH    || type == MACL_PRE|| type == MACH_PRE  ||
            type == XOR  || type == XNOR    || type == AND     || type == ANDN      || type == NAND  ||
            type == OR   || type == ORN     || type == NOR     || type == SHIFT_LR  || type == SHIFT_AR || type == SHIFT_LL ||
            type == MV_ZE|| type == MV_NZ   || type == MV_MI   || type == MV_PL     ||
            type == SHIFT_AR_NEG            || type == SHIFT_AR_POS ||
            type == MULL_NEG                || type == MULL_POS     ||
            type == MULH_NEG                || type == MULH_POS     ||
            type == MIN                     || type == MAX          || type == ABS  ||
            type == MIN_VECTOR              || type == MAX_VECTOR   || type == BIT_REVERSAL);
}
bool CommandVPRO::isWriteLM(){
    return (type == STORE || type == STORE_SHIFT_LEFT || type == STORE_SHIFT_RIGHT || type == STORE_REVERSE);
}

bool CommandVPRO::isLS(){
    return (type == STORE || type == LOAD || type == LOADB || type == LOADBS || type == LOADS  ||
            type == STORE_SHIFT_LEFT      || type == STORE_SHIFT_RIGHT ||
            type == LOADS_SHIFT_LEFT      || type == LOADS_SHIFT_RIGHT ||
            type == STORE_REVERSE         || type == LOAD_REVERSE);
}

bool CommandVPRO::isChainPartCMD(){
    return (is_chain || src1.sel == SRC_SEL_CHAIN || src1.sel == SRC_SEL_LS || src2.sel == SRC_SEL_CHAIN || src2.sel == SRC_SEL_LS );
}

QString CommandVPRO::get_type(){
    switch (type) {
        case NONE:                  return QString("NONE     ");
        case LOAD:                  return QString("LOAD     ");
        case LOADB:                 return QString("LOADB    ");
        case LOADS:                 return QString("LOADS    ");
        case LOADS_SHIFT_LEFT:      return QString("LD_SFT_L ");
        case LOADS_SHIFT_RIGHT:     return QString("LD_SFT_R ");
        case LOADBS:                return QString("LOADBS   ");
        case LOAD_REVERSE:          return QString("LOAD_REV ");
        case STORE:                 return QString("STORE    ");
        case STORE_SHIFT_LEFT:      return QString("ST_SFT_L ");
        case STORE_SHIFT_RIGHT:     return QString("ST_SFT_R ");
        case STORE_REVERSE:         return QString("STORE_REV");
        case LOOP_START:     return QString("LOOP_STRT");
        case LOOP_END:       return QString("LOOP_END ");
        case LOOP_MASK:      return QString("LOOP_MASK");
        case ADD:            return QString("ADD      "); 
        case SUB:            return QString("SUB      "); 
        case MULL:           return QString("MULL     "); 
        case MULH:           return QString("MULH     "); 
        case DIVL:           return QString("DIVL     ");
        case DIVH:           return QString("DIVH     ");
        case MACL:           return QString("MACL     "); 
        case MACH:           return QString("MACH     "); 
        case MACL_PRE:       return QString("MACL_PRE "); 
        case MACH_PRE:       return QString("MACH_PRE "); 
        case XOR:            return QString("XOR      "); 
        case XNOR:           return QString("XNOR     "); 
        case AND:            return QString("AND      "); 
        case ANDN:           return QString("ANDN     "); 
        case NAND:           return QString("NAND     "); 
        case OR:             return QString("OR       "); 
        case ORN:            return QString("ORN      "); 
        case NOR:            return QString("NOR      "); 
        case SHIFT_LR:       return QString("SHIFT_LR "); 
        case SHIFT_AR:       return QString("SHIFT_AR ");
        case SHIFT_LL:       return QString("SHIFT_LL ");
        case ABS:            return QString("ABS      "); 
        case MIN:            return QString("MIN      ");
        case MAX:            return QString("MAX      ");
        case MIN_VECTOR:     return QString("MIN_VEC  ");
        case MAX_VECTOR:     return QString("MAX_VEC  ");
        case BIT_REVERSAL:   return QString("BIT_REV  ");
        case MV_ZE:          return QString("MV_ZE    ");
        case MV_NZ:          return QString("MV_NZ    ");
        case MV_MI:          return QString("MV_MI    ");
        case MV_PL:          return QString("MV_PL    ");
        case SHIFT_AR_NEG:   return QString("SHFTR_NEG");
        case SHIFT_AR_POS:   return QString("SHFTR_POS");
        case MULL_NEG:       return QString("MULL_NEG ");
        case MULL_POS:       return QString("MULL_POS ");
        case MULH_NEG:       return QString("MULH_NEG ");
        case MULH_POS:       return QString("MULH_POS ");
        case NOP:            return QString("NOP      ");
        case WAIT_BUSY:      return QString("WAIT_BUSY");
        case PIPELINE_WAIT:  return QString("PIPELN_W8");
        case IDMASK_GLOBAL:  return QString("IDMASK_GLOBAL: %1").arg(id_mask);
        default:
            return QString("Unknown Function!");
    }
}

void CommandVPRO::print_type(FILE * out){
    switch (type) {
        case NONE:           fprintf(out, "NONE     ");            return;
        case LOAD:           fprintf(out, "LOAD     ");            break;
        case LOADB:          fprintf(out, "LOADB    ");            break;
        case LOADS:          fprintf(out, "LOADS    ");            break;
        case LOADS_SHIFT_LEFT:  fprintf(out, "LD_SFT_L ");         break;
        case LOADS_SHIFT_RIGHT: fprintf(out, "LD_SFT_R ");         break;
        case LOAD_REVERSE:      fprintf(out, "LOAD_REV ");         break;
        case LOADBS:            fprintf(out, "LOADBS   ");         break;
        case STORE:             fprintf(out, "STORE    ");         break;
        case STORE_SHIFT_LEFT:  fprintf(out, "ST_SFT_L ");         break;
        case STORE_SHIFT_RIGHT: fprintf(out, "ST_SFT_R ");         break;
        case STORE_REVERSE:     fprintf(out, "STORE_REV");         break;
        case LOOP_START:     fprintf(out, "LOOP_STRT");            break;
        case LOOP_END:       fprintf(out, "LOOP_END ");            break;
        case LOOP_MASK:      fprintf(out, "LOOP_MASK");            break;
        case ADD:            fprintf(out, "ADD      ");            break;
        case SUB:            fprintf(out, "SUB      ");            break;
        case MULL:           fprintf(out, "MULL     ");            break;
        case MULH:           fprintf(out, "MULH     ");            break;
        case MACL:           fprintf(out, "MACL     ");            break;
        case MACH:           fprintf(out, "MACH     ");            break;
        case MACL_PRE:       fprintf(out, "MACL_PRE ");            break;
        case MACH_PRE:       fprintf(out, "MACH_PRE ");            break;
        case XOR:            fprintf(out, "XOR      ");            break;
        case XNOR:           fprintf(out, "XNOR     ");            break;
        case AND:            fprintf(out, "AND      ");            break;
        case ANDN:           fprintf(out, "ANDN     ");            break;
        case NAND:           fprintf(out, "NAND     ");            break;
        case OR:             fprintf(out, "OR       ");            break;
        case ORN:            fprintf(out, "ORN      ");            break;
        case NOR:            fprintf(out, "NOR      ");            break;
        case SHIFT_LR:       fprintf(out, "SHIFT_LR ");            break;
        case SHIFT_LL:       fprintf(out, "SHIFT_LL ");            break;
        case SHIFT_AR:       fprintf(out, "SHIFT_AR ");            break;
        case ABS:            fprintf(out, "ABS      ");            break;
        case MIN:            fprintf(out, "MIN      ");            break;
        case MAX:            fprintf(out, "MAX      ");            break;
        case MIN_VECTOR:     fprintf(out, "MIN_VEC  ");            break;
        case MAX_VECTOR:     fprintf(out, "MAX_VEC  ");            break;
        case BIT_REVERSAL:   fprintf(out, "BIT_REV  ");            break;
        case MV_ZE:          fprintf(out, "MV_ZE    ");            break;
        case MV_NZ:          fprintf(out, "MV_NZ    ");            break;
        case MV_MI:          fprintf(out, "MV_MI    ");            break;
        case MV_PL:          fprintf(out, "MV_PL    ");            break;
        case SHIFT_AR_NEG:   fprintf(out, "SHFTR_NEG");            break;
        case SHIFT_AR_POS:   fprintf(out, "SHFTR_POS");            break;
        case MULL_NEG:       fprintf(out, "MULL_NEG ");            break;
        case MULL_POS:       fprintf(out, "MULL_POS ");            break;
        case MULH_NEG:       fprintf(out, "MULH_NEG ");            break;
        case MULH_POS:       fprintf(out, "MULH_POS ");            break;
        case NOP:            fprintf(out, "NOP      ");            return;
        case WAIT_BUSY:      fprintf(out, "WAIT_BUSY");            return;
        case PIPELINE_WAIT:  fprintf(out, "PIPELN_W8");            return;
        case IDMASK_GLOBAL:  fprintf(out, "IDMASK_GLOBAL: %i", id_mask);           return;
        default:
            fprintf(out, "Unknown Function!");
            printf_warning("Unknown Function!");            return;
    }
}

QString CommandVPRO::get_string(){
    QString str = QString("");
    str += get_class_type();
    str += ", Func: " + get_type();

    str += QString(" Mask: %1,")
            .arg(id_mask,3);
    if (type == CommandVPRO::WAIT_BUSY)
        str += QString(" Cluster: %1, ")
                .arg(cluster,3);

    str += QString("DST(%1, %2, %3), ")
            .arg(dst.offset,3).arg(dst.alpha,3).arg(dst.beta,3);

    if (src1.sel == SRC_SEL_ADDR)
        str += QString("SRC1(%1, %2, %3), ")
                .arg(src1.offset,3).arg(src1.alpha,3).arg(src1.beta,3);
    else if (src1.sel == SRC_SEL_IMM)
        str += QString("SRC1(Imm: %1),     ")
                .arg(src1.imm,4);
    else if (src1.sel == SRC_SEL_CHAIN)
        str += QString("SRC1(chain ") + (src1.chain_right? "RIGHT" : (src1.chain_left? "LEFT" : "ID " + QString::number(src1.chain_id-1))) + QString("),    ");
    else if (src1.chain_ls)
        str += QString("SRC1(LS),            ");
    else
        str += QString("SRC1(undefined),     ");


    if (src2.sel == SRC_SEL_ADDR)
        str += QString("SRC2(%1, %2, %3), ")
                .arg(src2.offset,3).arg(src2.alpha,3).arg(src2.beta,3);
    else if (src2.sel == SRC_SEL_IMM)
        str += QString("SRC2(Imm: %1),     ")
                .arg(src2.imm,4);
    else if (src2.sel == SRC_SEL_CHAIN)
        str += QString("SRC2(chain ") + (src2.chain_right? "RIGHT" : (src2.chain_left? "LEFT" : "ID " + QString::number(src2.chain_id-1))) + QString("),    ");
    else if (src2.chain_ls)
        str += QString("SRC2(LS),            ");
    else
        str += QString("SRC2(undefined),     ");

    str += QString("(x=%1 [0;%2], y=%3 [0;%4]), %5%6%7")
            .arg(x).arg(x_end).arg(y).arg(y_end)
            .arg((blocking) ? "BLOCK, " : "").arg((is_chain) ? "CHAIN, " : "").arg((flag_update) ? "FLAG_UPDATE" : "");
    return str;
}

void CommandVPRO::print(FILE * out) {
    print_class_type(out);
    fprintf(out, "Func: ");
    print_type(out);

    fprintf(out, " Mask: %03d,", id_mask);
    if (type == CommandVPRO::WAIT_BUSY)
        fprintf(out, " Cluster: %03d, ", cluster);
//    fprintf(out, " Mask: %03d, Cluster: %03d, Unit: %03d, Lane: %03d, ", id_mask, cluster, vector_unit_sel, vector_lane_sel);

//    string_format("%0x4", src1.offset, );

    fprintf(out, "DST(%03d, %03d, %03d), ", dst.offset, dst.alpha, dst.beta);

    if (src1.sel == SRC_SEL_ADDR)
        fprintf(out, "SRC1(%03d, %03d, %03d), ", src1.offset, src1.alpha, src1.beta);
    else if (src1.sel == SRC_SEL_IMM)
        fprintf(out, "SRC1(Imm: %04d),     ", src1.imm);
    else if (src1.sel == SRC_SEL_CHAIN)
        fprintf(out, "SRC1(chain %s%i),    ", (src1.chain_right? "RIGHT" : (src1.chain_left? "LEFT" : "ID ")),  src1.chain_id-1);
    else if (src1.chain_ls)
        fprintf(out, "SRC1(LS),            ");
    else
        fprintf(out, "SRC1(undefined),     ");


    if (src2.sel == SRC_SEL_ADDR)
        fprintf(out, "SRC2(%03d, %03d, %03d), ", src2.offset, src2.alpha, src2.beta);
    else if (src2.sel == SRC_SEL_IMM)
        fprintf(out, "SRC2(Imm: %04d),     ", src2.imm);
    else if (src2.sel == SRC_SEL_CHAIN)
        fprintf(out, "SRC2(chain %s%i),    ", (src2.chain_right? "RIGHT" : (src2.chain_left? "LEFT" : "ID ")),  src2.chain_id-1);
    else if (src2.chain_ls)
        fprintf(out, "SRC1(LS),            ");
    else
        fprintf(out, "SRC2(undefined),     ");
    
    fprintf(out, "(x=%i [0;%i], y=%i [0;%i]), %s%s%s\n",
           x, x_end, y, y_end,
           ((blocking) ? "BLOCK, " : ""), ((is_chain) ? "CHAIN, " : ""),
           ((flag_update) ? "FLAG_UPDATE" : ""));
}

void CommandVPRO::updateType() {
    if (type == CommandVPRO::NONE) {
        switch (fu_sel) {
            case CLASS_MEM:
                // CHANGE for L/S Lane
                id_mask = LS;
                if (!is_chain && func != OPCODE_STORE) {
                    printf_warning("Set IS_CHAIN in L/S commands to IS_CHAIN! Only chaining from L/S commands is allowed! [got set to True]\n");
                    is_chain = true;
                }
//                if (blocking) {
//                    printf_warning("Set BLOCKING in L/S commands to NON_...! blocking in L/S commands is ignored! [blocking dropped]\n");
//                    blocking = false;
//                }
                switch (func) {
                    case OPCODE_LOAD:                  type = CommandVPRO::LOAD;                        break;
                    case OPCODE_LOADS:                 type = CommandVPRO::LOADS;                       break;
                    case OPCODE_LOADS_SHIFT_LEFT:      type = CommandVPRO::LOADS_SHIFT_LEFT;            break;
                    case OPCODE_LOADS_SHIFT_RIGHT:     type = CommandVPRO::LOADS_SHIFT_RIGHT;           break;
                    case OPCODE_LOADB:                 type = CommandVPRO::LOADB;                       break;
                    case OPCODE_LOADBS:                type = CommandVPRO::LOADBS;                      break;
                    case OPCODE_STORE:                 type = CommandVPRO::STORE;                       break;
                    case OPCODE_STORE_SHIFT_LEFT:      type = CommandVPRO::STORE_SHIFT_LEFT;            break;
                    case OPCODE_STORE_SHIFT_RIGHT:     type = CommandVPRO::STORE_SHIFT_RIGHT;           break;
                    case OPCODE_LOAD_REVERSE:          type = CommandVPRO::LOAD_REVERSE;                break;
                    case OPCODE_STORE_REVERSE:         type = CommandVPRO::STORE_REVERSE;               break;

                    default:
                        printf_error(
                                "VPRO_SIM ERROR: Invalid CLASS_MEM function! (fu=%d, func=%d, src1_sel=%d, src2_sel=%d)\n",
                                fu_sel, func, src1.sel, src2.sel);
                        break;
                }
                break;
            case CLASS_ALU:
                switch (func) {
                    case OPCODE_ADD:                    type = CommandVPRO::ADD;                       break;
                    case OPCODE_SUB:                    type = CommandVPRO::SUB;                       break;
                    case OPCODE_MULL:                   type = CommandVPRO::MULL;                      break;
                    case OPCODE_MULH:                   type = CommandVPRO::MULH;                      break;
                    case OPCODE_DIVL:                   type = CommandVPRO::DIVL;                      break;
                    case OPCODE_DIVH:                   type = CommandVPRO::DIVH;                      break;
                    case OPCODE_MACL:                   type = CommandVPRO::MACL;                      break;
                    case OPCODE_MACH:                   type = CommandVPRO::MACH;                      break;
                    case OPCODE_MACL_PRE:               type = CommandVPRO::MACL_PRE;                  break;
                    case OPCODE_MACH_PRE:               type = CommandVPRO::MACH_PRE;                  break;
                    case OPCODE_XOR:                    type = CommandVPRO::XOR;                       break;
                    case OPCODE_XNOR:                   type = CommandVPRO::XNOR;                      break;
                    case OPCODE_AND:                    type = CommandVPRO::AND;                       break;
                    case OPCODE_ANDN:                   type = CommandVPRO::ANDN;                      break;
                    case OPCODE_NAND:                   type = CommandVPRO::NAND;                      break;
                    case OPCODE_OR:                     type = CommandVPRO::OR;                        break;
                    case OPCODE_ORN:                    type = CommandVPRO::ORN;                       break;
                    case OPCODE_NOR:                    type = CommandVPRO::NOR;                       break;
                    default:
                        printf_error(
                                "VPRO_SIM ERROR: Invalid CLASS_ALU function! (fu=%d, func=%d, src1_sel=%d, src2_sel=%d)\n",
                                fu_sel, func, src1.sel, src2.sel);
                        break;
                }
                break;
            case CLASS_SPECIAL:
                switch (func) {
                    case OPCODE_SHIFT_LL:               type = CommandVPRO::SHIFT_LL;                   break;
                    case OPCODE_SHIFT_LR:               type = CommandVPRO::SHIFT_LR;                   break;
                    case OPCODE_SHIFT_AR:               type = CommandVPRO::SHIFT_AR;                   break;
                    case OPCODE_ABS:                    type = CommandVPRO::ABS;                        break;
                    case OPCODE_MIN:                    type = CommandVPRO::MIN;                        break;
                    case OPCODE_MAX:                    type = CommandVPRO::MAX;                        break;
                    case OPCODE_MIN_VECTOR:             type = CommandVPRO::MIN_VECTOR;                 break;
                    case OPCODE_MAX_VECTOR:             type = CommandVPRO::MAX_VECTOR;                 break;
                    case OPCODE_BIT_REVERSAL:           type = CommandVPRO::BIT_REVERSAL;               break;
                    case OPCODE_SHIFT_AR_NEG:           type = CommandVPRO::SHIFT_AR_NEG;               break;
                    case OPCODE_SHIFT_AR_POS:           type = CommandVPRO::SHIFT_AR_POS;               break;
                    case OPCODE_LOOP_START:             type = CommandVPRO::LOOP_START;                 break;
                    case OPCODE_LOOP_END:               type = CommandVPRO::LOOP_END;                   break;
                    case OPCODE_LOOP_MASK:              type = CommandVPRO::LOOP_MASK;                  break;
                    default:
                        printf_error(
                                "VPRO_SIM ERROR: Invalid CLASS_SPECIAL function! (fu=%d, func=%d, src1_sel=%d, src2_sel=%d)\n",
                                fu_sel, func, src1.sel, src2.sel);
                        break;
                }
                break;
            case CLASS_TRANSFER:
                switch (func) {
                    case OPCODE_MV_ZE:                  type = CommandVPRO::MV_ZE;                        break;
                    case OPCODE_MV_NZ:                  type = CommandVPRO::MV_NZ;                        break;
                    case OPCODE_MV_MI:                  type = CommandVPRO::MV_MI;                        break;
                    case OPCODE_MV_PL:                  type = CommandVPRO::MV_PL;                        break;
                    case OPCODE_MULL_NEG:               type = CommandVPRO::MULL_NEG;                    break;
                    case OPCODE_MULL_POS:               type = CommandVPRO::MULL_POS;                    break;
                    case OPCODE_MULH_NEG:               type = CommandVPRO::MULH_NEG;                    break;
                    case OPCODE_MULH_POS:               type = CommandVPRO::MULH_POS;                    break;
                    default:
                        printf_error(
                                "VPRO_SIM ERROR: Invalid CLASS_TRANSFER function! (fu=%d, func=%d, src1_sel=%d, src2_sel=%d)\n",
                                fu_sel, func, src1.sel, src2.sel);
                        break;
                }
                break;
            default:
                printf_error(
                        "VPRO_SIM ERROR: Invalid FU_SEL! (fu=%d, func=%d, src1_sel=%d, src2_sel=%d)\n",
                        fu_sel, func, src1.sel, src2.sel);
                break;
        }
    } else { // type already set
        return;
    }
}; // updateType


