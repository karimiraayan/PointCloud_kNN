// #################################################
// # VPRO instrinsics auxiliary library            #
// # --------------------------------------------- #
// # Stephan Nolting, IMS, Uni Hannover, 2017-2019 #
// #################################################

#ifndef isa_intrinsic_aux_lib
#define isa_intrinsic_aux_lib

#include <stdint.h>

// -----------------------------------------------------------------------------
// SYSTEM definitions
// -----------------------------------------------------------------------------
#define LM_BASE_VU(u)   (u*(1<<22)) // lm base address of VU "u"


// -----------------------------------------------------------------------------
// OPCODE definitions
// -----------------------------------------------------------------------------

// blocking?
#define BLOCKING     1
#define NONBLOCKING  0

// is source of chain?
#define IS_CHAIN  1
#define NO_CHAIN  0

// function classe
#define CLASS_MEM       0
#define CLASS_ALU       1
#define CLASS_SPECIAL   2
#define CLASS_TRANSFER  3
#define CLASS_OTHER     99

// function opcodes
#define OPCODE_LOAD     0b0000
#define OPCODE_LOADB    0b0001
#define OPCODE_LOADS    0b0010
#define OPCODE_LOADBS   0b0011
#define OPCODE_STORE    0b0100

#define OPCODE_LOOP_START 0b1000
#define OPCODE_LOOP_END   0b1001
#define OPCODE_LOOP_MASK  0b1010

#define OPCODE_ADD      0b0000
#define OPCODE_SUB      0b0001
#define OPCODE_MULL     0b0010
#define OPCODE_MULH     0b0011
#define OPCODE_MACL     0b0100
#define OPCODE_MACH     0b0101
#define OPCODE_MACL_PRE 0b0110
#define OPCODE_MACH_PRE 0b0111

#define OPCODE_XOR      0b1000
#define OPCODE_XNOR     0b1001
#define OPCODE_AND      0b1010
#define OPCODE_ANDN     0b1011
#define OPCODE_NAND     0b1100
#define OPCODE_OR       0b1101
#define OPCODE_ORN      0b1110
#define OPCODE_NOR      0b1111

#define OPCODE_DIVL     0b10000
#define OPCODE_DIVH     0b10001

#define OPCODE_SHIFT_LL 0b0000
#define OPCODE_SHIFT_LR 0b0001
#define OPCODE_SHIFT_AR 0b0011
#define OPCODE_ABS      0b0100
#define OPCODE_MIN      0b0110
#define OPCODE_MAX      0b0111

#define OPCODE_MV_ZE    0b0000
#define OPCODE_MV_NZ    0b0001
#define OPCODE_MV_MI    0b0010
#define OPCODE_MV_PL    0b0011

// actual operation ("function macro")
#define FUNC_LOAD     CLASS_MEM, OPCODE_LOAD
#define FUNC_LOADS    CLASS_MEM, OPCODE_LOADS
#define FUNC_LOADB    CLASS_MEM, OPCODE_LOADB
#define FUNC_LOADBS   CLASS_MEM, OPCODE_LOADBS
#define FUNC_STORE    CLASS_MEM, OPCODE_STORE

#define FUNC_LOOP     CLASS_MEM, OPCODE_LOOP

#define FUNC_ADD      CLASS_ALU, OPCODE_ADD
#define FUNC_SUB      CLASS_ALU, OPCODE_SUB
#define FUNC_MULL     CLASS_ALU, OPCODE_MULL
#define FUNC_MULH     CLASS_ALU, OPCODE_MULH
#define FUNC_DIVL	  CLASS_ALU, OPCODE_DIVL
#define FUNC_DIVH     CLASS_ALU, OPCODE_DIVH
#define FUNC_MACL     CLASS_ALU, OPCODE_MACL
#define FUNC_MACH     CLASS_ALU, OPCODE_MACH
#define FUNC_MACL_PRE CLASS_ALU, OPCODE_MACL_PRE
#define FUNC_MACH_PRE CLASS_ALU, OPCODE_MACH_PRE

#define FUNC_XOR      CLASS_ALU, OPCODE_XOR
#define FUNC_XNOR     CLASS_ALU, OPCODE_XNOR
#define FUNC_AND      CLASS_ALU, OPCODE_AND
#define FUNC_ANDN     CLASS_ALU, OPCODE_ANDN
#define FUNC_NAND     CLASS_ALU, OPCODE_NAND
#define FUNC_OR       CLASS_ALU, OPCODE_OR
#define FUNC_ORN      CLASS_ALU, OPCODE_ORN
#define FUNC_NOR      CLASS_ALU, OPCODE_NOR

#define FUNC_SHIFT_LL CLASS_SPECIAL, OPCODE_SHIFT_LL
#define FUNC_SHIFT_LR CLASS_SPECIAL, OPCODE_SHIFT_LR
#define FUNC_SHIFT_AR CLASS_SPECIAL, OPCODE_SHIFT_AR
#define FUNC_ABS      CLASS_SPECIAL, OPCODE_ABS
#define FUNC_MIN      CLASS_SPECIAL, OPCODE_MIN
#define FUNC_MAX      CLASS_SPECIAL, OPCODE_MAX

#define FUNC_MV_ZE    CLASS_TRANSFER, OPCODE_MV_ZE
#define FUNC_MV_NZ    CLASS_TRANSFER, OPCODE_MV_NZ
#define FUNC_MV_MI    CLASS_TRANSFER, OPCODE_MV_MI
#define FUNC_MV_PL    CLASS_TRANSFER, OPCODE_MV_PL

// status flag update
#define FLAG_UPDATE    1
#define NO_FLAG_UPDATE 0

// src selection
#define SRC_SEL_ADDR   0
#define SRC_SEL_IMM    1
#define SRC_SEL_CLEFT  2
#define SRC_SEL_CRIGHT 3


// -----------------------------------------------------------------------------
// Helper macros
// -----------------------------------------------------------------------------

// sign extension
#define S_EXT(x) ((signed)(((x) & 0x80000) ? ((x) | 0xFFF00000) : (x)))


// -----------------------------------------------------------------------------
// Instruction macros
// -----------------------------------------------------------------------------

// destination
#define DST_ADDR(offset, alpha, beta)  S_EXT(((((offset & 0x3ff) << 10) | ((alpha & 0x1f) << 5) | (beta & 0x1f)) & 0xfffff)) // complex addressing: offset, alpha, beta

// source1
    #define SRC1_ADDR(offset, alpha, beta)  SRC_SEL_ADDR, S_EXT(((((offset & 0x3ff) << 10) | ((alpha & 0x1f) << 5) | (beta & 0x1f)) & 0xfffff)) // complex addressing: offset, alpha, beta
#define SRC1_IMM(imm)                   SRC_SEL_IMM, S_EXT((imm & 0xfffff)) // 20-bit imm will be sign-extended to 24-bit
#define SRC1_CHAINING_LEFT              SRC_SEL_CLEFT, 0 // use chaining from left side, actual src1 value is irrelevant
#define SRC1_CHAINING_RIGHT             SRC_SEL_CRIGHT, 0 // use chaining from right side, actual src1 value is irrelevant
#define SRC1_DONT_CARE                  SRC_SEL_ADDR, 0

    #define SRC2_ADDR(offset, alpha, beta)  SRC_SEL_ADDR, S_EXT(((((offset & 0x3ff) << 10) | ((alpha & 0x1f) << 5) | (beta & 0x1f)) & 0xfffff)) // complex addressing: offset, alpha, beta
#define SRC2_IMM(imm)                   SRC_SEL_IMM, S_EXT((imm & 0xfffff)) // 20-bit imm will be sign-extended to 24-bit
#define SRC2_CHAINING_LEFT              SRC_SEL_CLEFT, 0 // use chaining from left side, actual src2 value is irrelevant
#define SRC2_CHAINING_RIGHT             SRC_SEL_CRIGHT, 0 // use chaining from right side, actual src2 value is irrelevant
#define SRC2_DONT_CARE                  SRC_SEL_ADDR, 0

// hardware looping "instruction"
#define VPRO_LOOP(id, num, uend, umul0, umul1, vend, vmul, os0, os1, os2, os3, os4, os5, os6, os7) \
  __builtin_vpro_instruction_word(id, BLOCKING, NO_CHAIN, FUNC_LOOP, \
  (((num-1) >> 2) & 1), \
  S_EXT((((num-1) & 3) << 18) | ((uend & 0xff) << 10) | ((umul0 & 0x3ff) << 0)), \
  (umul1 >> 8) & 3, \
  S_EXT((((umul1 & 0xff) << 12) | ((vend & 0xff) << 4) | ((vmul & 3) << 2) | ((os0 >> 2) & 3)) & 0xfffff), \
  os0 & 3, \
  S_EXT((((os1 & 0xf) << 16) | ((os2 & 0xf) << 12) | ((os3 & 0xf) << 8) | ((os4 & 0xf) << 4) | ((os5 & 0xf) << 0)) & 0xfffff), \
  os6 & 0xf, \
  os7 & 0xf);

//(int)((((os1 & 0xf) << 16) | ((os2 & 0xf) << 12) | ((os3 & 0xf) << 8) | ((os4 & 0xf) << 4) | ((os5 & 0xf) << 0)) & 0xfffff)
// -----------------------------------------------------------------------------
// VPRO pre-defined instruction
// -----------------------------------------------------------------------------

// This macro might be used to insulate two consecutive vector instructions, which process less than 10 elements (= depth of pipeline) and also access the SAME elements.
// Since the lanes do not support forwarding, a dummy delay using this NOP command has to be used!
#define vpro_nop __builtin_vpro_instruction_word(0, NONBLOCKING, NO_CHAIN, FUNC_ADD, NO_FLAG_UPDATE, DST_ADDR(0, 0, 0), SRC1_ADDR(0, 0, 0), SRC2_IMM(0), 2, 3);


#endif // isa_intrinsic_aux_lib

