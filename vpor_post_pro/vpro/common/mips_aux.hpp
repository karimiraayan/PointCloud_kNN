// ################################################################
// # mips_aux.h - Auxiliary functions for the MIPS32 processor    #
// #                                                              #
// # Stephan Nolting, Uni Hannover, 2014-2017                     #
// ################################################################

#ifndef mips_aux_h
#define mips_aux_h

// IO map
#include "vpro_mips.h"


// Function prototypes
//int32_t aux_frac_div32(int32_t a, int32_t b, const short ibits, const short n_terms);
inline int32_t  aux_frac_mul32(int32_t a, int32_t b, const short ibits);
inline uint32_t aux_frac_umul32(uint32_t a, uint32_t b, const short ibits);
inline uint32_t aux_mul(uint32_t a, uint32_t b);
inline uint32_t aux_abs(int32_t dat);
inline uint32_t aux_clz(uint32_t data);
inline uint32_t aux_clo(uint32_t data);
inline uint32_t aux_get_cycle_cnt();
inline void     aux_clr_cycle_cnt();
inline void     aux_clr_icache();
inline void     aux_clr_dcache();
inline void     aux_flush_dcache();
inline void     aux_clr_dhit_cnt();
inline void     aux_clr_dmiss_cnt();
inline uint32_t aux_get_dhit_cnt();
inline uint32_t aux_get_dmiss_cnt();
inline void     aux_rnd_en();
inline void     aux_rnd_dis();
inline void     aux_rnd_seed(uint32_t data);
inline uint32_t aux_rnd_get();
inline void     aux_print_debugfifo(uint32_t data);
inline uint32_t aux_dev_null(uint32_t data);
inline void     aux_wait_cycles(uint32_t n);
inline void     aux_clr_sys_time();
inline uint32_t aux_get_sys_time_lo();
inline uint32_t aux_get_sys_time_hi();
inline void     aux_send_system_runtime();
       void     aux_memset(uint32_t base_addr, uint8_t value, uint32_t num_bytes);


// ***************************************************************************************************************
// Arithmetic macros
// ***************************************************************************************************************

// ---------------------------------------------------------------------------------
// Fractional division: X = A/B
// A, B and X are fractional numbers
// Return +/- infinity (maximum value) if division by zero
// >ibits< specifies the number of integer bits
// >n_terms< specifies the number of fractional approximation terms
// => more terms => higher precision but also longer execution time
// ---------------------------------------------------------------------------------
/*int32_t aux_frac_div32(int32_t a, int32_t b, const short ibits, const short n_terms) {

  register uint32_t Q = 0, Q_0 = 0, Q_1 = 0, Q_2 = 0;
  register uint32_t R = 0, R_0 = 0, R_1 = 0;
  register uint32_t tmp = 0;;

  // Sign compensation (make positive for internal computation)
  register uint32_t sign = (uint32_t)((a ^ b) & 0x80000000);
  register uint32_t x = aux_abs(a);
  register uint32_t y = aux_abs(b);

  // division by zero?
  if (y == 0) {
    register uint32_t n = 0xFFFFFFFF; // max value (-> infinity)
    if (sign != 0)
      return -(n);
    return (n);
  }

  // Integer computation
  register int32_t shift = (int32_t)aux_clz(x);
  x = x << shift;

  Q = x / y; // quotient

  if ((32-ibits-shift) < 0)
    Q = Q >> aux_abs(32-ibits-shift);
  else
    Q = Q << aux_abs(32-ibits-shift);

  // Fractional computation - term 1
  if (n_terms > 0) {
    R = x % y; // remainder
    tmp = aux_clz(R);
    R = R << tmp;
    shift += (int32_t)(tmp);

    Q_0 = R / y;

    if ((32-ibits-shift) < 0)
      Q_0 = Q_0 >> aux_abs(32-ibits-shift);
    else
      Q_0 = Q_0 << aux_abs(32-ibits-shift);
  }

  // Fractional computation - term 2
  if (n_terms > 1) {
    R_0 = R % y;
    tmp = aux_clz(R_0);
    R_0 = R_0 << tmp;
    shift += (int32_t)(tmp);

    Q_1 = R_0 / y;

    if ((32-ibits-shift) < 0)
      Q_1 = Q_1 >> aux_abs(32-ibits-shift);
    else
      Q_1 = Q_1 << aux_abs(32-ibits-shift);
  }

  // Fractional computation - term 3
  if (n_terms > 2) {
    R_1 = R_0 % y;
    tmp = aux_clz(R_1);
    R_1 = R_1 << tmp;
    shift += (int32_t)(tmp);

    Q_2 = R_1 / y;

    if ((32-ibits-shift) < 0)
      Q_2 = Q_2 >> aux_abs(32-ibits-shift);
    else
      Q_2 = Q_2 << aux_abs(32-ibits-shift);
  }

  if (sign != 0)
    return -(Q + Q_0 + Q_1 + Q_2);
  else
    return (Q + Q_0 + Q_1 + Q_2);
}*/

// ---------------------------------------------------------------------------------
// Multiply two signed, fractional 32-bit values
// Output: Low 32 bit of product (with corrected fixed-point format)
// >ibits< specifies the number of integer bits
// ---------------------------------------------------------------------------------
inline int32_t aux_frac_mul32(int32_t a, int32_t b, const short ibits) {

  int32_t tmp_hi = 0, tmp_lo = 0;

  asm volatile (".set noat\r\n" "mult  %0, %1"     : : "r" (a), "r" (b));
  asm volatile (".set noat\r\n" "mflo  %0"         : "=r" (tmp_lo)); // stall several cycles :(
  asm volatile (".set noat\r\n" "mfhi  %0"         : "=r" (tmp_hi));
  asm volatile (".set noat\r\n" "srl   %0, %1, %2" : "=r" (tmp_lo) : "r" (tmp_lo), "i" (32-ibits));
  asm volatile (".set noat\r\n" "sll   %0, %1, %2" : "=r" (tmp_hi) : "r" (tmp_hi), "i" (ibits));
  asm volatile (".set noat\r\n" "or    %0, %1, %2" : "=r" (tmp_lo) : "r" (tmp_hi), "r" (tmp_lo));

  return tmp_lo;
}

// ---------------------------------------------------------------------------------
// Multiply two unsigned, fractional 32-bit values
// Output: Low 32 bit of product (with corrected fixed-point format)
// >ibits< specifies the number of integer bits
// ---------------------------------------------------------------------------------
inline uint32_t aux_frac_umul32(uint32_t a, uint32_t b, const short ibits) {

  uint32_t tmp_hi = 0, tmp_lo = 0;

  asm volatile (".set noat\r\n" "multu %0, %1"     : : "r" (a), "r" (b));
  asm volatile (".set noat\r\n" "mflo  %0"         : "=r" (tmp_lo)); // stall several cycles :(
  asm volatile (".set noat\r\n" "mfhi  %0"         : "=r" (tmp_hi));
  asm volatile (".set noat\r\n" "srl   %0, %1, %2" : "=r" (tmp_lo) : "r" (tmp_lo), "i" (32-ibits));
  asm volatile (".set noat\r\n" "sll   %0, %1, %2" : "=r" (tmp_hi) : "r" (tmp_hi), "i" (ibits));
  asm volatile (".set noat\r\n" "or    %0, %1, %2" : "=r" (tmp_lo) : "r" (tmp_hi), "r" (tmp_lo));

  return tmp_lo;
}

// ---------------------------------------------------------------------------------
// Simple multiplication macro
// ---------------------------------------------------------------------------------
inline uint32_t aux_mul(uint32_t a, uint32_t b) {

  uint32_t res = 0;
  asm volatile (".set noat\r\n" "mul %0, %1, %2" : "=r" (res) : "r" (a), "r" (b));
  return res;
}

// ---------------------------------------------------------------------------------
// Get absolute value of input data
// ---------------------------------------------------------------------------------
inline uint32_t aux_abs(int32_t dat) {

  uint32_t tmp = 0, res = 0;
  asm volatile (".set noat\r\n" "sra %0, %1, 31\r\n" : "=r" (tmp) : "r" (dat));
  asm volatile (".set noat\r\n" "xor %0, %1, %2" : "=r" (res) : "r" (dat), "r" (tmp));
  asm volatile (".set noat\r\n" "sub %0, %1, %2" : "=r" (res) : "r" (res), "r" (tmp));

  return res;
}


// ***************************************************************************************************************
// Logic macros
// ***************************************************************************************************************

// ---------------------------------------------------------------------------------
// Count number of leading zeroes
// ---------------------------------------------------------------------------------
inline uint32_t aux_clz(uint32_t data) {

  uint32_t tmp = 0;
  asm volatile (".set noat\r\n" "clz %0, %1" : "=r" (tmp) : "r" (data));
  return tmp;
}

// ---------------------------------------------------------------------------------
// Count number of leading ones
// ---------------------------------------------------------------------------------
inline uint32_t aux_clo(uint32_t data) {

  uint32_t tmp = 0;
  asm volatile (".set noat\r\n" "clo %0, %1" : "=r" (tmp) : "r" (data));
  return tmp;
}


// ***************************************************************************************************************
// CPU control macros
// ***************************************************************************************************************

// ---------------------------------------------------------------------------------
// get CPU-internal cycle counter
// ---------------------------------------------------------------------------------
inline uint32_t aux_get_cycle_cnt() {

  uint32_t res = 0;
  // CP0, R9, SEL0 = CPU cycle counter
  asm volatile (".set noat\r\n" "mfc0 %0, $9, 0" : "=r" (res));
  return res;
}

// ---------------------------------------------------------------------------------
// reset CPU-internal cycle counter
// ---------------------------------------------------------------------------------
inline void aux_clr_cycle_cnt() {

  asm volatile (".set noat\r\n" "mtc0  $0, $9, 0");
}

// ---------------------------------------------------------------------------------
// Clear instruction cache
// ---------------------------------------------------------------------------------
inline void aux_clr_icache() {

  uint32_t tmp = 0;
  // bit #0: clear i-cache
  // CP0, R16, SEL7 = cache control register
  asm volatile (".set noat\r\n" "ori   %0, $0,  1" : "=r" (tmp));
  asm volatile (".set noat\r\n" "mtc0  %0, $16, 7" : : "r" (tmp));
}

// ---------------------------------------------------------------------------------
// Clear data cache
// ---------------------------------------------------------------------------------
inline void aux_clr_dcache() {

  uint32_t tmp = 0;
  // bit #1: clear d-cache
  // CP0, R16, SEL7 = cache control register
  asm volatile (".set noat\r\n" "ori   %0, $0,  2" : "=r" (tmp));
  asm volatile (".set noat\r\n" "mtc0  %0, $16, 7" : : "r" (tmp));
}

// ---------------------------------------------------------------------------------
// Flush data cache
// ---------------------------------------------------------------------------------
inline void aux_flush_dcache() {

  uint32_t tmp = 0;
  // bit 2: flush d-cache
  // CP=0, REG=16, SEL=7
  asm volatile (".set noat\r\n" "ori   %0, $0,  4" : "=r" (tmp));
  asm volatile (".set noat\r\n" "mtc0  %0, $16, 7" : : "r" (tmp));
}

// ---------------------------------------------------------------------------------
// Clear data cache hit counter
// ---------------------------------------------------------------------------------
inline void aux_clr_dhit_cnt() {

  // CP=0, REG=16, SEL=5
  asm volatile (".set noat\r\n" "mtc0  $0, $16, 5" : : );
}

// ---------------------------------------------------------------------------------
// Clear data cache miss counter
// ---------------------------------------------------------------------------------
inline void aux_clr_dmiss_cnt() {

  // CP=0, REG=16, SEL=6
  asm volatile (".set noat\r\n" "mtc0  $0, $16, 6" : : );
}

// ---------------------------------------------------------------------------------
// Get data cache hit counter (must be multiplied with 2)
// ---------------------------------------------------------------------------------
inline uint32_t aux_get_dhit_cnt() {

  uint32_t res = 0;
  // CP=0, REG=16, SEL=5
  asm volatile (".set noat\r\n" "mfc0 %0, $16, 5" : "=r" (res));
  return res;
}

// ---------------------------------------------------------------------------------
// Get data cache miss counter (must be multiplied with 2)
// ---------------------------------------------------------------------------------
inline uint32_t aux_get_dmiss_cnt() {

  uint32_t res = 0;
  // CP=0, REG=16, SEL=6
  asm volatile (".set noat\r\n" "mfc0 %0, $16, 6" : "=r" (res));
  return res;
}

// ---------------------------------------------------------------------------------
// Activate random generator
// ---------------------------------------------------------------------------------
inline void aux_rnd_en() {

  uint32_t tmp = 0;
  // bit 0: RND enable
  // CP=0, REG=9, SEL=7
  asm volatile (".set noat\r\n" "ori   %0, $0, 1" : "=r" (tmp));
  asm volatile (".set noat\r\n" "mtc0  %0, $9, 7" : : "r" (tmp));
}

// ---------------------------------------------------------------------------------
// Deactivate random generator
// ---------------------------------------------------------------------------------
inline void aux_rnd_dis() {

  uint32_t tmp = 0;
  // bit 0: RND enable
  // CP=0, REG=9, SEL=7
  asm volatile (".set noat\r\n" "ori   %0, $0, 0" : "=r" (tmp));
  asm volatile (".set noat\r\n" "mtc0  %0, $9, 7" : : "r" (tmp));
}

// ---------------------------------------------------------------------------------
// Set seed value of random generator
// ---------------------------------------------------------------------------------
inline void aux_rnd_seed(uint32_t data) {

  // CP=0, REG=9, SEL=6 : Random number generator (LFSR)
  asm volatile (".set noat\r\n" "mtc0  %0, $9, 6" : : "r" (data));
}

// ---------------------------------------------------------------------------------
// Get random data
// ---------------------------------------------------------------------------------
inline uint32_t aux_rnd_get() {

  uint32_t res = 0;
  // CP=0, REG=9, SEL=6 : Random number generator (LFSR)
  asm volatile (".set noat\r\n" "mfc0 %0, $9, 6" : "=r" (res));
  return res;
}


// ***************************************************************************************************************
// Misc macros
// ***************************************************************************************************************

// ---------------------------------------------------------------------------------
// write data word to DEBUG FIFO
// ---------------------------------------------------------------------------------
inline void aux_print_debugfifo(uint32_t data) {

  DEBUG_FIFO = data;
}

// ---------------------------------------------------------------------------------
// Dummy write / read
// ---------------------------------------------------------------------------------
inline uint32_t aux_dev_null(uint32_t data) {

  DEV_NULL = data;
  return DEV_NULL;
}

// ---------------------------------------------------------------------------------
// Wait ~n CPU cycles
// ---------------------------------------------------------------------------------
inline void aux_wait_cycles(uint32_t n) {

  while(n--)
    asm volatile (".set noat\r\n" "nop");
}

// ---------------------------------------------------------------------------------
// Clear system time counter
// ---------------------------------------------------------------------------------
inline void aux_clr_sys_time() {

  SYS_TIME_CNT_LO = 0;
}

// ---------------------------------------------------------------------------------
// Get system time counter (lower part)
// Read FIRST when reading a full 64-bit word!
// ---------------------------------------------------------------------------------
inline uint32_t aux_get_sys_time_lo() {

  return SYS_TIME_CNT_LO;
}

// ---------------------------------------------------------------------------------
// Get system time counter (higher part)
// Read LOW PART FIRST when reading a full 64-bit word!
// ---------------------------------------------------------------------------------
inline uint32_t aux_get_sys_time_hi() {

  return SYS_TIME_CNT_HI;
}


// ---------------------------------------------------------------------------------
// Send system runtime (64-bit) to debug FIFO
// This is the counterpart of the host function "get_system_runtime()"
// ---------------------------------------------------------------------------------
inline void aux_send_system_runtime() {

  aux_print_debugfifo(aux_get_sys_time_lo());
  aux_print_debugfifo(aux_get_sys_time_hi());
  
}


// ***************************************************************************************************************
// Main memory helper functions
// ***************************************************************************************************************

// ---------------------------------------------------------------------------------
// Initialize main memory
// ---------------------------------------------------------------------------------
void aux_memset(uint32_t base_addr, uint8_t value, uint32_t num_bytes) {

  // OBSOLETE!


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


#endif // mips_aux_h
