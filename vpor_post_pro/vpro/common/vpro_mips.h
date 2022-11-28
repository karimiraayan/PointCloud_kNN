// ###############################################################
// # vpro_mips.h - System IO map - visible from MIPS processors  #
// # ----------------------------------------------------------- #
// # Created:     Stephan Nolting, 03.03.2017                    #
// # Last update: Stephan Nolting, 14.02.2018                    #
// ###############################################################

#include <inttypes.h>

#ifndef vpro_mips_h
#define vpro_mips_h

// Base address of vector processor system: 0xFFFE0000
// Base address of IO utility devices:      0xFFFF0000



// Global VCP Command Mask Registers
// -----------------------------------------------------------------------------
#define VCP_CLUSTER_MASK (*((volatile uint32_t*) (0xFFFE0000))) // -/w: AND mask for broadcasting VCMDs
#define VCP_UNIT_MASK    (*((volatile uint32_t*) (0xFFFE0004))) // -/w: AND mask for broadcasting VCMDs

#define VCP_MUL_SHIFT_REG    (*((volatile uint32_t*) (0xFFFE0040))) // MUL 5-bit of data are used!
#define VCP_MAC_SHIFT_REG    (*((volatile uint32_t*) (0xFFFE0044))) // MAC 5-bit!

// VCP Status Register, N instances (one for each cluster)
// -----------------------------------------------------------------------------
// BUSY register base address 0xFFFEyy00, yy: 8-bit specifies cluster
#define VCP_BUSY_BASE (*((volatile uint32_t*) (0xFFFE0008))) // r/-: bits in reg correspond to VU in cluster


// for variable passing to VPRO - IO
// These are addresses for MIPS io to replace vpro parameter content
#define VPRO_CMD_REGISTER_0 (*((volatile uint32_t*) (0xFFFFFE00)))
#define VPRO_CMD_REGISTER_1 (*((volatile uint32_t*) (0xFFFFFE04)))
#define VPRO_CMD_REGISTER_2 (*((volatile uint32_t*) (0xFFFFFE08)))

#define VPRO_LOOP_START_REG_0 (*((volatile uint32_t*) (0xFFFFFE10)))
#define VPRO_LOOP_START_REG_1 (*((volatile uint32_t*) (0xFFFFFE14)))
#define VPRO_LOOP_MASK (*((volatile uint32_t*) (0xFFFFFE18)))
#define VPRO_LOOP_END (*((volatile uint32_t*) (0xFFFFFE1C)))

#define VPRO_CMD_REGISTER_SRC1BETA_SRC2IMM       (*((volatile uint32_t*) (0xFFFFFE20))) // ((beta << 20u) | imm)
#define VPRO_CMD_REGISTER_DSTOFFSET              (*((volatile uint32_t*) (0xFFFFFE24))) // offset
#define VPRO_CMD_REGISTER_DSTSRC1OFFSET_SRC2IMM  (*((volatile uint32_t*) (0xFFFFFE28))) // ((offset << 20u) | imm)
#define VPRO_CMD_REGISTER_DSTOFFSET_SRC2IMM      (*((volatile uint32_t*) (0xFFFFFE2C))) // ((offset << 20u) | imm)

// MIPS iDMA Controller, N instances (one for each cluster)
// -----------------------------------------------------------------------------
// DMA register base address 0xFFFEyy80..0xFFFEyyFE, yy: 8-bit specifies cluster
#define IDMA_EXT_BASE_ADDR_E2L (*((volatile uint32_t*) (0xFFFE0080))) // -/w: external main memory base read address + L<=E trigger
#define IDMA_EXT_BASE_ADDR_L2E (*((volatile uint32_t*) (0xFFFE0084))) // -/w: external main memory base write address + L=>E trigger
#define IDMA_LOC_BASE_ADDR     (*((volatile uint32_t*) (0xFFFE0088))) // -/w: local memory base address (including cluster/unit/LM select)
#define IDMA_BLOCK_X           (*((volatile uint32_t*) (0xFFFE0090))) // -/w: x block size in #elements (16-bit words))
#define IDMA_BLOCK_Y           (*((volatile uint32_t*) (0xFFFE0094))) // -/w: y block size in #elements (16-bit words))
#define IDMA_BLOCK_STRIDE      (*((volatile uint32_t*) (0xFFFE0098))) // -/w: block stride in #elements (16-bit words))
#define IDMA_STATUS_BUSY       (*((volatile uint32_t*) (0xFFFE00BC))) // r/-: bit 2: queue full, bit 1: fifo not empty, bit 0: fsm busy

#define IDMA_PAD_TOP_SIZE     (*((volatile uint32_t*) (0xFFFE00A0))) // -/w: padding for e2l 2d operations
#define IDMA_PAD_BOTTOM_SIZE  (*((volatile uint32_t*) (0xFFFE00A4))) // -/w
#define IDMA_PAD_LEFT_SIZE    (*((volatile uint32_t*) (0xFFFE00A8))) // -/w
#define IDMA_PAD_RIGHT_SIZE   (*((volatile uint32_t*) (0xFFFE00AC))) // -/w
#define IDMA_PAD_VALUE        (*((volatile uint32_t*) (0xFFFE00B0))) // -/w

// DEBUG FIFO
// -----------------------------------------------------------------------------
#define DEBUG_FIFO (*((volatile uint32_t*) (0xFFFFFFF0))) // -/w


// Sytem time counter
// -----------------------------------------------------------------------------
#define SYS_TIME_CNT_LO (*((volatile uint32_t*) (0xFFFFFFF4))) // r/c - read first!
#define SYS_TIME_CNT_HI (*((volatile uint32_t*) (0xFFFFFFF8))) // r/c


// dev/null for dummy read/write transfers
// -----------------------------------------------------------------------------
#define DEV_NULL (*((volatile uint32_t*) (0xFFFFFFFC))) // r/w


#endif // vpro_mips_h
