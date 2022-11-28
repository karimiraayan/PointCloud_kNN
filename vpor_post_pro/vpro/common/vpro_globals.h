//
// Created by gesper on 07.10.20.
//

#ifndef HW_VERIFICATION_APP_VERSION_0_0_VPRO_GLOBALS_H
#define HW_VERIFICATION_APP_VERSION_0_0_VPRO_GLOBALS_H

struct HW_CONFIG{
    int CLUSTERS;
    int UNITS;
    int LANES;
    int MM_SIZE;
    int LM_SIZE;
    int RF_SIZE;
    HW_CONFIG(){
        LANES = 2;
        UNITS = 1;
        CLUSTERS = 1;
        MM_SIZE = 1024*1024*512;
        LM_SIZE = 8192;
        RF_SIZE = 1024;
    }
};
extern HW_CONFIG HW;


#endif //HW_VERIFICATION_APP_VERSION_0_0_VPRO_GLOBALS_H
