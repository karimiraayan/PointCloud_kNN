// ########################################################
// # VPRO instruction & system simulation library         #
// # Sven Gesper, IMS, Uni Hannover, 2019                 #
// ########################################################
// # model of "real" architecture for (later?) c++ coding #
// ########################################################

#ifndef vpro_core_class
#define vpro_core_class

#include "core.h"


class VProCore : public Core {
public:
    VProCore();

    // use real vpro instructions (processor functions)
#if (IS_SIMULATION == 0)
    // Core libraries
    #include "../common/vpro_mips.h" // VPRO.MIPS address map
    #include "../common/mips_aux.h" // MIPS CPU helper functions
    #include "../common/vpro_aux.h" // VPRO helper functions
#endif

private:
};


#endif
