//
// Created by gesper on 16.12.20.
//

#include "MipsDebugReader.h"
#include "../../../common/vpro_memory_map.h"

// UEMU libraries
#include <dma_common.h>

#include <QProcessEnvironment>
#include <QDebug>

#include <iostream>
#include <stdio.h>	/* printf */
#include <iostream>
#include <iomanip>


void MipsDebugReader::run() {
    console.printf_success("[MIPS Debug]\t\tThread started (listening %s)\n", (isRunning())? "RUNNING": "PAUSED");

    while (isRunning()) {

        int tmp;
        unsigned char debug_buf_tmp[8];
        while(true){
            mutex.lock();
            // get single word from fifo
            dma_common_read((char*) debug_buf_tmp, MIPS_INSTR_ADDR, sizeof(debug_buf_tmp));
            if ((debug_buf_tmp[0] >> 7) != 0) { // valid flag set? (bit #63)
                tmp =       (debug_buf_tmp[4] << 24)  | (debug_buf_tmp[5] << 16);
                tmp = tmp | (debug_buf_tmp[6] <<  8)  | (debug_buf_tmp[7] <<  0);
                break;
            }
            mutex.unlock();
            usleep(5000); // 5 ms
        }
        mutex.unlock();

        console.printf_warning_("[MIPS Debug Addr] "+QString::number(tmp, 10)+" | 0x"+QString::number(tmp, 16), false);

//        printf_error("[MIPS Debug Addr]\t\t%i = 0x%x", tmp, tmp);

        if (QProcessEnvironment::systemEnvironment().value("UEMU_CONN_MODE", "NONE") == "SIM_IO"){
//            printf("MIPS IS RUNNING IN RTL SIM!");
            usleep(10000000); // 10 s
        } else if (QProcessEnvironment::systemEnvironment().value("UEMU_CONN_MODE", "NONE") == "ETH_IO"){
//            printf("MIPS IS RUNNING ON FPGA!");
            usleep(1000000); // 1 s
        } else if (QProcessEnvironment::systemEnvironment().value("UEMU_CONN_MODE", "NONE") == "NONE"){
            printf("MIPS IS RUNNING <???> !\n");
        }
    }
}

MipsDebugReader::MipsDebugReader(QMutex &mutex, ConsolePrinter &console) : mutex(mutex), console(console), QThread(){

}
