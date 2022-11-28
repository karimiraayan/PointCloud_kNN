################################################################################
# crt0.asm - Execution startup routine for the MIPS32 processor
# Stephan Nolting, Diploma thesis, IMS, Uni Hannover, 2014-2016
################################################################################

    .text
    .global entry
    .ent entry
    .set noat
entry:
    .set noreorder

# -------------------------------------------------------------------
# Simplified Interrupt/Exception vectors.
# These are defined in the rtl/core/MIPS_package.vhd file.
# -------------------------------------------------------------------
EXC_Vector_Base_Reset_c:      # x"00000000"
    b reset_vec
    lui $at, 0xFF00

EXC_Vector_Offset_General:    # x"00000008"
    b no_irq_vec
    lui $at, 0xFF08

EXC_Vector_Offset_Special:    # x"00000010"
    b no_irq_vec
    lui $at, 0xFF10

# -------------------------------------------------------------------
# This handler uses r1 ("at"), which is reserved for the assembler.
# Therefore, the context of the actual app is not affected at all.
# High half-word of last entry in DEBUG FIO contains IQR base address
# and marker "FF", low half-word is always 0xDEAD.
# -------------------------------------------------------------------
no_irq_vec: # gather all requests
    ori   $at, $at, 0xDEAD
    sw    $at, -16($0) # address of debug fifo
no_irq_vec_end:
    b     no_irq_vec_end # freeze processor
    nop


# -------------------------------------------------------------------
# Clear caches (invalidate all entries and force reload)
# -------------------------------------------------------------------
reset_vec:
    ori   $20, $0,  1   # bit #0: clear i-cache
    mtc0  $20, $16, 7   # CP0, R16, SEL7 = cache control register

    ori   $20, $0,  2   # bit #1: clear d-cache
    mtc0  $20, $16, 7   # CP0, R16, SEL7 = cache control register


# -------------------------------------------------------------------
# Init global constants (configuration by linker script)
# -------------------------------------------------------------------
    # mask for word-boundary accesses
    lui   $1, 0xFFFF
    ori   $1, $1, 0xFFFC

    # .bss area start
    lui   $10, %hi(__bss_start)
    addi   $10, $10, %lo(__bss_start)
    and   $10, $10, $1

    # .bss area end
    lui   $11, %hi(__bss_end)
    addi   $11, $11, %lo(__bss_end)
    and   $11, $11, $1

    # init global pointer
    lui   $28, %hi(__gp)
    addi   $28, $28, %lo(__gp)
    and   $28, $28, $1

    # init stack pointer (and frame pointer)
    lui   $29, %lo(__sp_hi)
    addi  $29, $29, %lo(__sp_lo)
    and   $29, $29, $1
	addiu $29, $29, -64 /* create our (large?!) frame */
    addiu $30, $29, 0


# -------------------------------------------------------------------
# Clear .bss area
# -------------------------------------------------------------------
crt0_clear_bss_loop:
    beq   $10, $11, crt0_clear_bss_end
    addiu $10, $10, 4
    b     crt0_clear_bss_loop
    sw    $0, 0($10)
crt0_clear_bss_end:


# -------------------------------------------------------------------
# Prefetch first stack page to d-cache
# -------------------------------------------------------------------
prefetch_stack_page:
    lw    $zero, -4($sp) # very last entry of stack


# -------------------------------------------------------------------
# Clear all registers (since no reg-file reset is implemented)
# -------------------------------------------------------------------
    and $0,  $zero, $zero # write to dummy reg (just for sim...)
    and $1,  $zero, $zero
    and $2,  $zero, $zero
    and $3,  $zero, $zero
    and $4,  $zero, $zero
    and $5,  $zero, $zero
    and $6,  $zero, $zero
    and $7,  $zero, $zero
    and $8,  $zero, $zero
    and $9,  $zero, $zero
    and $10, $zero, $zero
    and $11, $zero, $zero
    and $12, $zero, $zero
    and $13, $zero, $zero
    and $14, $zero, $zero
    and $15, $zero, $zero
    and $16, $zero, $zero
    and $17, $zero, $zero
    and $18, $zero, $zero
    and $19, $zero, $zero
    and $20, $zero, $zero
    and $21, $zero, $zero
    and $22, $zero, $zero
    and $23, $zero, $zero
    and $24, $zero, $zero
    and $25, $zero, $zero
    and $26, $zero, $zero
    and $27, $zero, $zero
    # DO NOT CLEAR GP ($28)
    # DO NOT CLEAR SP ($29)
    # DO NOT CLEAR SP ($30)
    and $31, $zero, $zero


# -------------------------------------------------------------------
# TEST AND DEBUGGING STUFF
# -------------------------------------------------------------------
#   addiu $1, $0, 1
#   addiu $2, $0, 2
#   j test_end
#   addiu $3, $0, 3
#   addiu $4, $0, 4
#
#test_end:
#   j     test_end
#   nop


# -------------------------------------------------------------------
# Call application's main function
# -------------------------------------------------------------------
    jal   main
    mtc0  $zero, $9, 0 # clear internal counter register (CP0)
    nop


# -------------------------------------------------------------------
# Endless loop if main function returns (flush d-cache before)
# -------------------------------------------------------------------
    # sync mem with cache
#   ori   $10, $0,  4 # set bit 2: flush d-cache
#   mtc0  $10, $16, 7 # CP=0, REG=16, SEL=7

    # print return value to debug fifo
#   sw    $2, -16($0) # address of debug fifo

this_is_the_end:
    j     this_is_the_end
    nop

    .end entry

