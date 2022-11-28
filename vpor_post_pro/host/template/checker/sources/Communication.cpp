//
// Created by gesper on 20.11.20.
//

#include "Communication.h"
#include <dma_common.h>

#include <stdint.h>
#include <iostream>
#include <chrono>
#include <thread>

#include "../../../common/vpro_memory_map.h"
#include <QDir>
#include <QDebug>

Communication::Communication(QMutex &mutex, AuxDebugFifoReader &fiforeader, ConsolePrinter &console,
                             int &MM_TEST_RESULT, int &MM_TEST_DATA_1,
                             int &MM_TEST_DATA_2, int &MM_START_FLAG, int &MM_RUNNING_FLAG, int &MM_RETURN_FLAG,
                             const QString &executable, const QString &input_a, const QString &input_b,
                             const QString &output, int num_cl, int num_vu_per_cl, bool &silent, int &silent_testnr,
                             QObject *parent) :
        fiforeader(fiforeader),
        console(console),
        MM_TEST_RESULT(MM_TEST_RESULT),
        MM_TEST_DATA_1(MM_TEST_DATA_1),
        MM_TEST_DATA_2(MM_TEST_DATA_2),
        MM_START_FLAG(MM_START_FLAG),
        MM_RUNNING_FLAG(MM_RUNNING_FLAG),
        MM_RETURN_FLAG(MM_RETURN_FLAG),
        executable(executable),
        input_a(input_a),
        input_b(input_b),
        output(output),
        silent(silent),
        silent_testnr(silent_testnr),
        num_cl(num_cl),
        num_vu_per_cl(num_vu_per_cl),
        mutex(mutex),
        success(false),
        QThread(parent) {

    flag = new uint64_t(1);
    NUM_TEST_ENTRIES = 64;
    opa = new uint16_t[NUM_TEST_ENTRIES]();
    opb = new uint16_t[NUM_TEST_ENTRIES]();

    {
        QFile file_opa(input_a);
        if (!file_opa.open(QIODevice::ReadOnly)) {
            console.printf_error("Input could not be opened! [File: %s]\n", input_a.toStdString().c_str());
        }
        QByteArray input_data_a = file_opa.readAll();
        for (int i = 0; i < input_data_a.size(); i += 2) {
            ((uint8_t *) (opa))[i + 0] = uint8_t(input_data_a.at(i + 1));
            ((uint8_t *) (opa))[i + 1] = uint8_t(input_data_a.at(i + 0));
        }
        file_opa.close();

        QFile file_opb(input_b);
        if (!file_opb.open(QIODevice::ReadOnly)) {
            console.printf_error("Input could not be opened! [File: %s]\n", input_b.toStdString().c_str());
        }
        QByteArray input_data_b = file_opb.readAll();
        for (int i = 0; i < input_data_b.size(); i += 2) {
            ((uint8_t *) (opb))[i + 0] = uint8_t(input_data_b.at(i + 1));
            ((uint8_t *) (opb))[i + 1] = uint8_t(input_data_b.at(i + 0));
        }
        file_opb.close();
    }


/**
 * Get user config
 */
    printHex = true;

    // PRINT all Flags
    vpro_flag = new uint64_t;
    mutex.lock();
    bin_array_dump(MM_RUNNING_FLAG, 8, (uint8_t *) vpro_flag);
    console.printf_success("MM_RUNNING_FLAG: %lx\n", *vpro_flag);
    bin_array_dump(MM_START_FLAG, 8, (uint8_t *) vpro_flag);
    console.printf_success("MM_START_FLAG: %lx\n", *vpro_flag);
    mutex.unlock();
}


void Communication::run() {
    console.printf_info("[COMMUNICATION]\tStarted Thread!\n");

    while (isRunning()) {
/**
 * Test Execute
 */
        console.printf_normal("Transffering Input Data...\n");
        console.printf_normal("\t [%i | 0x%x] File: %s, Size: %i\n", MM_TEST_DATA_1, MM_TEST_DATA_1,
                              input_a.toStdString().c_str(),
                              2 * NUM_TEST_ENTRIES * num_cl * num_vu_per_cl);
        console.printf_normal("\t [%i | 0x%x] File: %s, Size: %i\n", MM_TEST_DATA_2, MM_TEST_DATA_2,
                              input_b.toStdString().c_str(),
                              2 * NUM_TEST_ENTRIES * num_cl * num_vu_per_cl);
        mutex.lock();
        bin_file_send(MM_TEST_DATA_1, 2 * NUM_TEST_ENTRIES * num_cl * num_vu_per_cl, input_a.toStdString().c_str());
        bin_file_send(MM_TEST_DATA_2, 2 * NUM_TEST_ENTRIES * num_cl * num_vu_per_cl, input_b.toStdString().c_str());
        mutex.unlock();

        // OP A+B
        console.printf_normal(
                "\nOperand A (SRC1)                                                              | Operand B (SRC2) \n");
        console.printf_normal(
                "=============================================================================================================================================================\n");
        for (int j = 0; j < NUM_TEST_ENTRIES; j += 8) {
            console.printf_normal("%3i: ", j);
            for (int i = 0; i < 8; ++i) {
                if (printHex)
                    console.printf_info(" %8x", int32_t(opa[j + i]));
                else
                    console.printf_info(" %8i", int32_t(opa[j + i]));
            }
            console.printf_normal(" | ");
            console.printf_normal("%3i: ", j);
            for (int i = 0; i < 8; ++i) {
                if (printHex)
                    console.printf_info(" %8x", int32_t(opb[j + i]));
                else
                    console.printf_info(" %8i", int32_t(opb[j + i]));
            }
            console.printf_normal("\n");
        }
        console.printf_normal(
                "=============================================================================================================================================================\n");


        /**
         * Start Application (select flag = 1)
         */
        console.printf_normal("Starting Application (START_FLAG = 1)\n");
        *flag = 1;
        mutex.lock();
        bin_array_send(MM_START_FLAG, 8, (uint8_t *) flag);
        mutex.unlock();


        /**
         * Wait for finish
         */
        console.printf_normal("Waiting for VPRO to finish execution (RETURN_FLAG = 1)\n");
        mutex.lock();
        bin_array_dump(MM_RETURN_FLAG, 8, (uint8_t *) vpro_flag);
        mutex.unlock();
        while (*vpro_flag == 0) {
            // vpro is executing...
            msleep(10);
            mutex.lock();
            bin_array_dump(MM_RETURN_FLAG, 8, (uint8_t *) vpro_flag);
            mutex.unlock();
        }


        /**
         * Fetch result
         */
        console.printf_normal("Fetching Result from VPRO\n");
        auto *result = new uint16_t[NUM_TEST_ENTRIES];
        mutex.lock();
        bin_array_dump(MM_TEST_RESULT, 2 * NUM_TEST_ENTRIES, reinterpret_cast<uint8_t *>(result));
        mutex.unlock();
        console.printf_normal("\nResult \n");
        console.printf_normal(
                "=============================================================================================================================================================\n");
        for (int j = 0; j < NUM_TEST_ENTRIES; j += 8) {
            console.printf_normal("%3i: ", j);
            for (int i = 0; i < 8; ++i) {
                if (printHex)
                    console.printf_info(" %8x", int32_t(result[j + i]));
                else
                    console.printf_info(" %8i", int32_t(result[j + i]));
            }
            console.printf_normal("\n");
        }
        console.printf_normal(
                "=============================================================================================================================================================\n");


        /**
         * Quit vpro loop
         */
        console.printf_error("Quit Application Loop... \n");
        success = true;
        break;
    }
    console.printf_normal("\n\t ->VPRO is stopping now!\n\n");
    *flag = 0;
    mutex.lock();
    bin_array_send(MM_RUNNING_FLAG, 8, (uint8_t *) flag);
    mutex.unlock();
}
