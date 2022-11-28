// ########################################################
// # VPRO instruction & system simulation library         #
// # Sven Gesper, IMS, Uni Hannover, 2019                 #
// ########################################################
// # type conversion between datas in vpro architecure    #
// # e.g., 24-bit 18-bit ...                              #
// #       interpretation as uint/int32...                #
// #       for memory convert to arrays of uint8          #
// ########################################################

#include "typeConversion.h"
#include "debugHelper.h"


// ***********************************************************************
// Dual logarithm (unsigned integer!)
// ***********************************************************************
uint32_t ld(uint32_t x) {

    if (x == 0) {
        printf_error("VPRO_SIM ERROR: Invalid log2 argument! (%d)\n", x);
        return 0;
    }

    uint32_t i = 0;

    x = x >> 1;
    while(x != 0) {
        x = x >> 1;
        i++;
    }

    return i;
}


// ***********************************************************************
// Size/format conversions
// ***********************************************************************
//uint32_t __8to24(uint16_t val) {
//
//    uint32_t ret = (uint32_t)(val & 0xFF);
//
//    return ret & 0x00FFFFFF;
//}
uint32_t* __8to24(uint32_t &val) {
    val = val & 0xFF;
    return &val;
}

//uint8_t * __8to24(uint8_t * val) {
//    uint8_t *result = new uint8_t[3];
//    result[0] = val[0];
//    result[1] = 0;
//    result[2] = 0;
//
//    return result;
//}

//uint32_t __8to16(uint8_t * val) {
//
//    uint32_t res = val[0];
//    res += val[1] << 8;
//
//    //delete val;
//    return res;
//}

//uint32_t __8to24signed(uint16_t &val) {
//
//    uint32_t ret = (uint32_t)(val & 0xFF);
//
//    // if negative
//    if (ret & 0x00000080)
//        ret = ret | 0xFFFFFF00;
//
//    return ret & 0x00FFFFFF;
//}

uint32_t* __8to24signed(uint32_t &val) {

    val = val & 0xFF;

    // if negative
    if (val & 0x00000080)
        val = val | 0xFFFFFF00;

    val = val & 0x00FFFFFF;
    return &val;
}


//uint8_t * __8to24signed(uint8_t * val) {
//
//    uint32_t ret = val[0];
//
//    // if negative
//    if (ret & 0x00000080)
//        ret = ret | 0xFFFFFF00;
//    ret = ret & 0x00FFFFFF;
//
//    uint8_t *result = new uint8_t[3];
//    result[0] = uint8_t (ret);
//    result[1] = uint8_t (ret >> 8);
//    result[2] = uint8_t (ret >> 16);
//
//    //delete val;
//    return result;
//}
//uint32_t __16to24(uint16_t &val) {
//    uint32_t ret = (uint32_t)val;
//    return ret & 0x00FFFFFF;
//}

uint32_t* __16to24(uint32_t &val) {
    val = val & 0x00FFFFFF;
    return &val;
}

//uint32_t __16to24(uint8_t *val) {
//    uint32_t result = val[0] + (val[1] << 8);
//    return result;
//}


//uint32_t __16to24signed(uint16_t val) {
//    uint32_t ret = (uint32_t)val;
//    // if negative
//    if (ret & 0x00008000)
//        ret = ret | 0xffff0000;
//
//    return ret & 0x00FFFFFF;
//}

uint32_t* __16to24signed(uint32_t &val) {
    // if negative
    if (val & 0x00008000)
        val = val | 0xffff0000;

    val = val & 0x00FFFFFF;
    return &val;
}

//uint8_t * __16to24signed(uint8_t * val) {
//
//    uint32_t ret = val[0] + (val[1] << 8);
//
//    // if negative
//    if (ret & 0x00008000)
//        ret = ret | 0xffff0000;
//    ret = ret & 0x00FFFFFF;
//
//    uint8_t *result = new uint8_t[3];
//    result[0] = uint8_t (ret);
//    result[1] = uint8_t (ret >> 8);
//    result[2] = uint8_t (ret >> 16);
//
//    //delete val;
//    return result;
//}


uint32_t* __18to32signed(uint32_t &val) {

    val =val & 0x0003ffff;

    // if negative
    if (val & 0x00020000)
        val = val | 0xfffc0000;

    return &val;
}

// for immediates
uint32_t* __20to32signed(uint32_t &val) {

    val = val & 0x000fffff;

    // if negative
    if (val & 0x00080000)
        val = val | 0xfff00000;

    return &val;
}


uint64_t* __24to64signed(uint64_t &val) {

    val = val & 0x0000000000ffffff;

    // if negative
    if (val & 0x00800000)
        val = val | 0xffffffffff000000;

    return &val;
}

uint32_t* __24to32signed(uint32_t &val) {

    val = val & 0x00ffffff;

    // if negative
    if (val & 0x00800000)
        val = val | 0xff000000;

    return &val;
}

//uint32_t __24to32signed(uint8_t *val) {
//
//    uint32_t ret = val[0] + (val[1] << 8) + (val[2] << 16);
//    ret = ret & 0x00ffffff;
//
//    // if negative
//    if (ret & 0x00800000)
//        ret = ret | 0xff000000;
//
//    //delete val;
//    return ret;
//}

//uint32_t __18to32signed(uint8_t *val) {
//
//    uint32_t ret = val[0] + (val[1] << 8) + (val[2] << 16);
//    ret = ret & 0x0003ffff;
//
//    // if negative
//    if (ret & 0x00020000)
//        ret = ret | 0xfffc0000;
//
//    //delete val;
//    return ret;
//}


uint32_t* __32to24(uint32_t &val) {
    val = val & 0x00ffffff;
    return &val;
}


//uint16_t __24to16(uint32_t &val) {
//
//    return (uint16_t)(val & 0x0000ffff);
//}


//uint8_t *__32to8array(uint32_t val) {
//
//    uint8_t *result = new uint8_t[3];
//    result[0] = uint8_t (val);
//    result[1] = uint8_t (val >> 8);
//    result[2] = uint8_t (val >> 16);
//
//    return result;
//}


// ***********************************************************************
// Flag helper
// ***********************************************************************
bool __is_zero(uint32_t &val) {

    if ((val & 0x00ffffff) == 0)
        return 1;
    else
        return 0;
}
bool __is_zero(uint8_t *val) {
    return (val[0] == 0 && val[1] == 0 && val[2] == 0);
}

bool __is_negative(uint32_t &val) {

    if ((val & 0x00800000) == 0)
        return 0;
    else
        return 1;
}

bool __is_negative(uint8_t *val) {
    if ((val[2] & 0x80) == 0)
        return 0;
    else
        return 1;
}

