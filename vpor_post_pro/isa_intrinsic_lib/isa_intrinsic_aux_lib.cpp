//
// Created by gesper on 23.07.19.
//

#include <stdlib.h>
#include <stdio.h>

#include "isa_intrinsic_aux_lib.h"
#include "simulator/simCore.h"
#include "core_class_wrapper.h"
//
//uint32_t  get_SRC1_ADDR(uint32_t offset, uint32_t alpha, uint32_t beta) {
//    if (beta > MAX_BETA){
//        printf_error("SRC1 ERROR! beta in Addressing more than 5-bit!\n\n");
//        core_->printSimResults();
//        exit(1);
//    }
//    if (alpha > MAX_ALPHA){
//        printf_error("SRC1 ERROR! alpha in Addressing more than 5-bit!\n\n");
//        core_->printSimResults();
//        exit(1);
//    }
//    if (offset > MAX_OFFSET){
//        printf_error("SRC1 ERROR! offset in Addressing more than 10-bit!\n\n");
//        core_->printSimResults();
//        exit(1);
//    }
//    uint32_t result = 0;
//    result += S_EXT((((offset & 0x3ff) << 10) | ((alpha & 0x1f) << 5) | (beta & 0x1f)) & 0xfffff);
//    return result;
//}
//
//
//uint32_t  get_SRC2_ADDR(uint32_t offset, uint32_t alpha, uint32_t beta) {
//    if (beta > MAX_BETA){
//        printf_error("SRC2 ERROR! beta in Addressing more than 5-bit!\n\n");
//        core_->printSimResults();
//        exit(1);
//    }
//    if (alpha > MAX_ALPHA){
//        printf_error("SRC2 ERROR! alpha in Addressing more than 5-bit!\n\n");
//        core_->printSimResults();
//        exit(1);
//    }
//    if (offset > MAX_OFFSET){
//        printf_error("SRC2 ERROR! offset in Addressing more than 10-bit!\n\n");
//        core_->printSimResults();
//        exit(1);
//    }
//    uint32_t result = 0;
//    result += S_EXT((((offset & 0x3ff) << 10) | ((alpha & 0x1f) << 5) | (beta & 0x1f)) & 0xfffff);
//    return result;
//}
//
//
//// max and min value for 20-bit number, as max = 0007ffff, min = fff80000
//uint32_t get_S_EXT_20(uint32_t x){
//    if (int(x) > 0x0007ffff || int(x) < int(0xfff80000)){ // check for overflow
//        printf_error("S_EXT Error! immediate in Addressing more than 20-bit! x (%i) outside range [%i - %i]\n\n", x, int(0xfff80000), 0x0007ffff);
//        core_->printSimResults();
//        exit(1);
//    }
//    return  ((signed)(((x) & 0x80000u) ? ((x & 0xfffffu) | 0xFFF00000u) : (x & 0xfffffu)));
//}
//
//uint32_t get_CUT(uint32_t x, uint32_t mask){
//    uint32_t result = (x & mask);
//    if (result != x){
//        printf_error("CUT Error! OFFSET/SRC1_CHAINING(id)/... intrinsic more bit than allowed (?10-bit?) x: %i [0x%x]. Mask for this parameter: %x\n\n", x, x, mask);
//        core_->printSimResults();
//        exit(1);
//    }
//    return result;
//}
