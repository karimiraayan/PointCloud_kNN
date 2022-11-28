// ########################################################
// # VPRO instruction & system simulation library         #
// # Sven Gesper, IMS, Uni Hannover, 2019                 #
// ########################################################
// # VPRO types                                           #
// ########################################################

#ifndef VPRO_CPP_STRUCTTYPES_HPP
#define VPRO_CPP_STRUCTTYPES_HPP

// C std libraries
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <inttypes.h>
#include <vector>

#include <QString>

#include "../../isa_intrinsic_aux_lib.h"

struct addr_field_t {
    uint8_t sel;
    uint32_t imm;
    uint32_t offset;
    uint32_t alpha;
    uint32_t beta;
    uint8_t chain_id;
    bool chain_left;
    bool chain_right;
    bool chain_ls;
    bool delayed_chain;

    addr_field_t() {
        sel = SRC_SEL_ADDR;
        imm = 0;
        offset = 0;
        alpha = 0;
        beta = 0;
        chain_id = 0;
        chain_left = false;
        chain_right = false;
        chain_ls = false;
        delayed_chain = false;
    }

    QString __toString() {
        return QString::number(sel, 2).leftJustified(3, ' ') + ", Imm " +
               QString::number(imm).leftJustified(7, ' ') + ", Off" +
               QString::number(offset).leftJustified(4, ' ') + ", a " +
               QString::number(alpha).leftJustified(2, ' ') +  ", b " +
               QString::number(beta).leftJustified(2, ' ') +  ", chain " +
               QString::number(chain_id).leftJustified(1, ' ') + "," +
               QString::number(chain_left) +  "," +
               QString::number(chain_right) +  "," +
               QString::number(chain_ls) + "," +
               QString::number(delayed_chain);
    }
};

#endif //VPRO_CPP_STRUCTTYPES_HPP
