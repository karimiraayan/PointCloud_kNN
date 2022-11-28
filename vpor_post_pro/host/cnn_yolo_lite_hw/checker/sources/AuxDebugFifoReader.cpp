//
// Created by gesper on 20.11.20.
//

#include "AuxDebugFifoReader.h"
#include "../../../common/vpro_memory_map.h"

// UEMU libraries
#include <sys/stat.h>
#include <dma_common.h>
#include <mcpa.h>
#include <byteswap.h>
#include <chrono>
#include <thread>

void AuxDebugFifoReader::run() {
    console.printf_success("[DEBUG FIFO]\t\tThread started\n"); //  (listening %s) , (isRunning())? "RUNNING": "PAUSED"

    while (isRunning()) {

        int tmp = 0;
        unsigned char debug_buf_tmp[8];
        while(1){
            mutex.lock();
            // get single word from fifo
            dma_common_read((char*) debug_buf_tmp, DEBUG_FIFO_ADDR, sizeof(debug_buf_tmp));
            if ((debug_buf_tmp[0] >> 7) != 0) { // valid flag set? (bit #63)
                tmp =       (debug_buf_tmp[4] << 24)  | (debug_buf_tmp[5] << 16);
                tmp = tmp | (debug_buf_tmp[6] <<  8)  | (debug_buf_tmp[7] <<  0);
                break;
            }
            mutex.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
        mutex.unlock();

        console.printf_error("[DEBUG FIFO]\t\t%i = 0x%x\n", tmp, tmp);

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

AuxDebugFifoReader::AuxDebugFifoReader(QMutex &mutex, ConsolePrinter &console) : mutex(mutex), console(console), QThread(){

}
