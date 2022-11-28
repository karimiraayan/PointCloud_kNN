// ########################################################
// # VPRO instruction & system simulation library         #
// # Sven Gesper, IMS, Uni Hannover, 2019                 #
// ########################################################

#ifndef dma_broadcasts
#define dma_broadcasts

#include "core_class_wrapper.h"
//#include "../includes/helper.h"
// used by idma descriptor:
//MM ADDR:  32-bit
//LM ADDR:  32-bit
//x size    13-bit
//y size    13-bit
//x-stride  13-bit
//================
//total    103-bit (+1-bit for direction, based on io address of trigger)
//5 IO-instructions! (use of 6 different io adresses -> for each dma instr)

// 5 * 32-bit (per IO-Instruction) = 160-bit
//  57-bit unused

// to  encode (mask/one-hot coding) the unit for broadcast targets:
// use upper 19-bits of x_stride (drop 13-bits of x_stride) concat with
// use upper 19-bits of x_size   (drop 13-bits of x_size  ) concat with
// use upper 19-bits of y_size   (drop 13-bits of y_size  ) => result with 3*19-bit = 57-bit

// modify of dma descriptor...
//    descriptor with 104-bit... to be extended by 57-bit for unit mask



void dma_ext1D_to_loc1D_broadcast(uint32_t cluster, uint64_t unit_mask, uint64_t ext_base, uint32_t loc_base, uint32_t word_count){
    uint32_t y_size = 0, x_size = 0, x_stride = 0;

    if ((word_count & 0b1111111111111u) != word_count){
        printf_error("DMA Ext 1D to Loc 1D, word_count overflow (max 13-bit): %i != %i\n", (word_count & 0b1111111111111u), word_count);
    }

    for( uint64_t i = 0 ; i < 57u ; i++){ // check each bit
        if (((unit_mask >> i) & 0b1u) == 1){ // bit at position i is set

            if(i < 19){
                y_size |= (0b1u << (i+13));
            } else if (i < 38){
                x_size |= (0b1u << (i-19+13));
            } else {
                x_stride |= (0b1u << (i-38+13));
            }
        }
    }
//    if (unit_mask != 0){
//        printf_warning("DMA 1d... unit mask: ");
//        printBits(8, &unit_mask);
//        printf("\n");
//    }

    dma_ext1D_to_loc1D(cluster, ext_base, loc_base, word_count, x_stride, x_size, y_size, true);
}



void dma_ext2D_to_loc1D_broadcast(uint32_t cluster, uint64_t unit_mask, uint64_t ext_base, uint32_t loc_base, int32_t x_stride, uint32_t x_size, uint32_t y_size, bool *pad_flags) {

    if ((x_size & 0x1fff) != x_size){
        printf_error("DMA Ext 2D to Loc 1D, x_size overflow (max 13-bit): %i != %i\n", (x_size & 0x1fff), x_size);
    }
    if ((x_stride > 0 && ((x_stride & 0x1fff) != x_stride)) ||
        (x_stride < 0 && ((x_stride & 0b11111111111111111110000000000000) != 0b11111111111111111110000000000000))){
        if (x_stride > 0)
            printf_error("DMA Ext 2D to Loc 1D, x_stride overflow (max 13-bit): %i != %i\n", (x_stride & 0x1fff), x_stride);
        else
            printf_error("DMA Ext 2D to Loc 1D, x_stride overflow (max 13-bit): %i != %i\n", (x_stride & 0x1fff) | 0b11111111111111111110000000000000, x_stride);
    }
    if ((y_size & 0x1fff) != y_size){
        printf_error("DMA Ext 2D to Loc 1D, y_size overflow (max 13-bit): %i != %i\n", (y_size & 0x1fff), y_size);
    }

    x_stride &= 0x1fff; // 13-bit
    y_size &= 0x1fff; // 13-bit
    x_size &= 0x1fff; // 13-bit

    for (uint64_t i = 0; i < 57u; i++) { // check each bit
        if (((unit_mask >> i) & 0b1u) == 1) { // bit at position i is set

            if (i < 19) {
                y_size |= (0b1u << (i + 13));
            } else if (i < 38) {
                x_size |= (0b1u << (i - 19 + 13));
            } else {
                x_stride |= (0b1u << (i - 38 + 13));
            }
        }
    }
//    if (unit_mask != 0){
//        printf_warning("DMA 2d... unit mask: ");
//        printBits(8, &unit_mask);
//        printf("\n");
//    }

    dma_ext2D_to_loc1D(cluster, ext_base, loc_base, x_stride, x_size, y_size, pad_flags, true);
}



#endif // dma_broadcasts
