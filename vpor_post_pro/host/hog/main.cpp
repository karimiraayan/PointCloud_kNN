// ############################################################
// # VPRO image convolution demo program / Host side          #
// # Stephan Nolting, IMS, Uni Hannover, August 2018          #
// ############################################################

// C std libraries
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <inttypes.h>
#include <limits.h>

// VPRO memory map and helper functions
#include "../common/vpro_memory_map.h"
#include "../common/vpro_host_aux.h"
#include "../common/opencv_helper.h"

// Main memory layout
#define MM_IN_IMG  		(0x20000000)
#define MM_OUT_IMG_GX 	(0x21000000)
#define MM_OUT_IMG_GY	(0x22000000)


// VPRO signals
#define SIGNAL_DONE 0x0000BEEF


int main(int argc, char **argv) {

  // Disable buffering on std out
  setbuf(stdout, NULL);

  if (argc != 7) {
    printf("Invalid input arguments!\n");
    printf("1st: Input binary image file\n");
    printf("2nd: Input x resolution (width)\n");
    printf("3rd: Input y resolution (height)\n");
    printf("4th: Output binary image file\n");
    printf("5th: Output x resolution (width)\n");
    printf("6th: Output y resolution (height)\n");
    return -1;
  }

  int input_x  = atoi(argv[2]);
  int input_y  = atoi(argv[3]);
  int output_x = atoi(argv[5]);
  int output_y = atoi(argv[6]);

  // init UEMU connection
  init_connection_via_cfg(0);


  // deactivate processor
  printf("Deactivating processor... ");
  set_reset();
  printf("Ok\n");


  // transmit image
  printf("Sending input binary 16-bit image file %s with resolution %dx%dpx (%d bytes) to address 0x%08x... ", argv[1], input_x, input_y, input_x*input_y*2, MM_IN_IMG);
  bin_file_send(MM_IN_IMG, input_x*input_y*2, argv[1]);
  printf("Ok\n");


  // transfer program and start
  printf("Transmitting executable... ");
  send_executable("data.bin");
  printf("Ok\n");


  // execute program on VPRO
  printf("Starting processor... ");
  release_reset();
  printf("Ok\n");


  // wait for VPRO to finish processing + DEBUGGING output
  printf("Running...\n");
  while(1) {
    int tmp = get_debug_fifo_entry();
    if (tmp == SIGNAL_DONE)
      break;
    printf("Debug FIFO: 0x%08x\n", tmp);
  }


  // read output image
  printf("Reading output binary 16-bit image file %s with resolution %dx%dpx (%d bytes) from address 0x%08x... ", argv[4], output_x, output_y, output_x*output_y*2, MM_OUT_IMG_GX);
  bin_file_dump(MM_OUT_IMG_GX, output_x*output_y*2, argv[4]);
  printf("Ok\n");
  
  printf("Reading output binary 16-bit image file %s with resolution %dx%dpx (%d bytes) from address 0x%08x... ", argv[4], output_x, output_y, output_x*output_y*2, MM_OUT_IMG_GY);
  bin_file_dump(MM_OUT_IMG_GY, output_x*output_y*2, argv[4]);
  printf("Ok\n");


  // show performance data
  uint64_t runtime = get_system_runtime();
  printf("\nTotal cycles:   %" PRIu64 "\n", runtime);
  printf("Execution time: %03.5f ms\n", ((float)runtime)*0.00001);

  return 0;
}
