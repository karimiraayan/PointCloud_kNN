#include "mips_aux.h"


// ***************************************************************************************************************
// Main memory helper functions
// ***************************************************************************************************************

// ---------------------------------------------------------------------------------
// Initialize main memory
// ---------------------------------------------------------------------------------
void aux_memset(uint32_t base_addr, uint8_t value, uint32_t num_bytes) {

  // "volatile" is not efficient here, but without the volatile statement this
  // function is lowered to the 'memset' from stdlib.h, which cannot be included,
  // since stddef.h is missing / not up-to-date :'(

  // don't forget to resync with extermal memory via flushing the d-cache!

  uint32_t *pnt32 = (uint32_t*)base_addr;
  volatile uint8_t *pnt = (uint8_t*)pnt32; // to access byte-wise

  while(num_bytes--) {
    *pnt++ = value;
  }

}
