//
// Created by gesper on 20.11.20.
//

#include "communication.h"
#include <dma_common.h>

#include <stdint.h>
#include <iostream>
#include <chrono>
#include <thread>

#include "../../../common/vpro_memory_map.h"
#include <QDir>
#include <QDebug>
#include <QRandomGenerator>

#include <boost/endian/conversion.hpp>
#include <boost/endian/buffers.hpp>  // see Synopsis below
using namespace boost::endian;

communication::communication(QMutex &mutex, AuxDebugFifoReader &fiforeader, ConsolePrinter &console, int &MM_TEST_RESULT, int &MM_TEST_DATA_1,
                             int &MM_TEST_DATA_2, int &MM_START_FLAG, int &MM_RUNNING_FLAG,
                             int &MM_USE_SINGLE_DMA_TRANSFERS, const QString& executable, const QString& input_a, const QString& input_b,
                             const QString& output, int num_cl, int num_vu_per_cl, bool &silent, int &silent_testnr,
                             QObject *parent) :
        fiforeader(fiforeader),
        console(console),
        MM_TEST_RESULT(MM_TEST_RESULT),
        MM_TEST_DATA_1(MM_TEST_DATA_1),
        MM_TEST_DATA_2(MM_TEST_DATA_2),
        MM_START_FLAG(MM_START_FLAG),
        MM_RUNNING_FLAG(MM_RUNNING_FLAG),
        MM_USE_SINGLE_DMA_TRANSFERS(MM_USE_SINGLE_DMA_TRANSFERS),
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

    testmap = initialize_test_groups();
    flag = new uint64_t(1);
    opa = new uint16_t[NUM_TEST_ENTRIES]();
    opb = new uint16_t[NUM_TEST_ENTRIES]();
    opa_mips = new uint16_t[NUM_TEST_ENTRIES]();
    opb_mips = new uint16_t[NUM_TEST_ENTRIES]();

    {
        QFile file_opa(input_a);
        if (!file_opa.open(QIODevice::ReadOnly)) {
            console.printf_error("Input could not be opened! [File: %s]\n", input_a.toStdString().c_str());
            console.printf_warning("Generating new random input data ...\n");

            QVector<quint32> vector;
            vector.resize(NUM_TEST_ENTRIES/2); // 16-bit to out file...
            QRandomGenerator::global()->fillRange(vector.data(), vector.size());
            for (int i = 0; i < NUM_TEST_ENTRIES; i += 2) {
                ((uint32_t *) (opa))[i/2] = uint32_t(vector.at(i/2));
            }
        } else {
            QByteArray input_data_a = file_opa.readAll();
            for (int i = 0; i < input_data_a.size(); i += 2) {
                ((uint8_t *) (opa))[i + 0] = uint8_t(input_data_a.at(i + 1));
                ((uint8_t *) (opa))[i + 1] = uint8_t(input_data_a.at(i + 0));
            }
            file_opa.close();
        }

        QFile file_opb(input_b);
        if (!file_opb.open(QIODevice::ReadOnly)) {
            console.printf_error("Input could not be opened! [File: %s]\n", input_b.toStdString().c_str());
            console.printf_warning("Generating new random input data ...\n");

            QVector<quint32> vector;
            vector.resize(NUM_TEST_ENTRIES/2); // 16-bit to out file...
            QRandomGenerator::global()->fillRange(vector.data(), vector.size());
            for (int i = 0; i < NUM_TEST_ENTRIES; i += 2) {
                ((uint32_t *) (opb))[i/2] = uint32_t(vector.at(i/2));
            }
        } else {
            QByteArray input_data_b = file_opb.readAll();
            for (int i = 0; i < input_data_b.size(); i += 2) {
                ((uint8_t *) (opb))[i + 0] = uint8_t(input_data_b.at(i + 1));
                ((uint8_t *) (opb))[i + 1] = uint8_t(input_data_b.at(i + 0));
            }
            file_opb.close();
        }

        for (int i = 0; i < NUM_TEST_ENTRIES; ++i) {
            opa_mips[i] = endian_reverse(opa[i]);
            opb_mips[i] = endian_reverse(opb[i]);
        }
    }


/**
 * Get user config
 */
    printHex = true;
    if (!silent)
        getUserHexChoice(printHex);

    dmaSingleTransfers = false;
    if (!silent)
        getUserDMAChoice(dmaSingleTransfers);
    *flag = 0;
    if (dmaSingleTransfers) *flag = 1;
    console.printf_normal("Transmitting Choice: %li\n", *flag);
    mutex.lock();
    bin_array_send(MM_USE_SINGLE_DMA_TRANSFERS, 8, (uint8_t *) flag);
    mutex.unlock();

    // PRINT all Flags
    vpro_flag = new uint64_t;
    mutex.lock();
    bin_array_dump(MM_USE_SINGLE_DMA_TRANSFERS, 8, (uint8_t *) vpro_flag);
    console.printf_success("MM_USE_SINGLE_DMA_TRANSFERS: %lx\n", *vpro_flag);
    bin_array_dump(MM_RUNNING_FLAG, 8, (uint8_t *) vpro_flag);
    console.printf_success("MM_RUNNING_FLAG: %lx\n", *vpro_flag);
    bin_array_dump(MM_START_FLAG, 8, (uint8_t *) vpro_flag);
    console.printf_success("MM_START_FLAG: %lx\n", *vpro_flag);
    mutex.unlock();

/**
 *  BASIC HOST - MM Test
 */

    console.printf_normal("Basic HOST <--> MM Read Test... \n");
    console.printf_normal("\tTransfering Data to MM\n");
    sleep(1);
    console.printf_normal("\t\t[%i | 0x%x] File: %s, Size: %i\n", MM_TEST_DATA_1, MM_TEST_DATA_1, "OPA", 2*NUM_TEST_ENTRIES);
    mutex.lock();
    bin_array_send(MM_TEST_DATA_1, 2* NUM_TEST_ENTRIES, (uint8_t *)(opa_mips));
    mutex.unlock();
    console.printf_normal("\tTransfering Data from MM\n");
    {
        auto *vpro_result = new uint8_t[2 * NUM_TEST_ENTRIES * num_cl * num_vu_per_cl];
        mutex.lock();
        bin_array_dump(MM_TEST_DATA_1, 2 * NUM_TEST_ENTRIES * num_cl * num_vu_per_cl, vpro_result);
        mutex.unlock();
        auto *vpro_result_ = (uint16_t *)vpro_result;
        for (int i = 0; i < NUM_TEST_ENTRIES; ++i) {
            vpro_result_[i] = endian_reverse(vpro_result_[i]);
        }
        QList<TESTS> tests_fails;
        QList<TESTS> tests_success;
        TESTS t = DMA_1DE_1DL_1DE;  // get reference for opa
        int32_t *reference_result = execute(console, static_cast<TESTS>(t), opa, opb);
        console.printf_normal("\t");
        int entries = NUM_TEST_ENTRIES;
        check_correctness(console, vpro_result_, reference_result, t, tests_fails, tests_success, true, nullptr, nullptr, entries);
        delete[] vpro_result;
    }
}

void communication::getUserHexChoice(bool &hex) {
    // get user choice
//    char c;
    console.printf_error("On ERROR on verification, print data as Hex? [Y|n]\n");
//    std::cin >> c;
    std::string name;
    getline(std::cin, name);
    hex = true;
    if (name == "n" || name == "N")
        hex = false;
    console.printf_normal("Selected Print as %s\n", (hex) ? "Hex" : "Dec");
}

void communication::getUserDMAChoice(bool &dma) {
    // get user choice
//    char c;
    console.printf_error("Should the DMA transfers back to mm be emitted in single words (1D, size = 1)? [y|N]\n");
//    std::cin >> c;
    std::string name;
    getline(std::cin, name);
    dma = false;
    if (name == "y" || name == "y")
        dma = true;
    console.printf_normal("Selected DMA Transfers as %s\n", (dma) ? "Single Words (Size = 1)" : "Blockwise (Size > 1)");
}

void communication::run() {
    console.printf_info("[CNN COMMUNICATION]\tStarted Thread!\n");

    while (isRunning()) {
/**
 * Test Select
 */
        console.printf_normal("\nNew Test Run...\n");

        int select; // test nr to run now
        if (!silent){ // print options to select a test
            print_test_index_label(console);
            console.printf_normal("\nSelect Test to run [Enter number between 1-%i]: ", static_cast<int>(TEST_END_END) - 1);
            [[maybe_unused]] int ret_ = scanf("%i", &select);
            console.printf_normal("\nSelected TESTS::");
            console.printf_normal_(testName(static_cast<TESTS>(select)));
            console.printf_normal("\n");

            if (select == 911) {
                console.printf_normal("\tGonna Read Debug fifo for 1s...\n");
                fiforeader.start();
                usleep(1000000); //1S
                fiforeader.quit();
                continue;
            }
            if (select == 912) {
                auto filename = "mm_dump.bin";
                console.printf_normal("\tGonna Dump the complete MM (0x40000000/1GB) into file: %s ... ", filename);
                mutex.lock();
                bin_file_dump(0, 0x40000000, filename); // 1GB
                mutex.unlock();
                console.printf_normal("[Done]\n");
                continue;
//        bin_file_dump(0, 0x10000000, "mm_before_run.bin");
//        bin_file_dump(0, 0x10000000, "mm_after_run.bin");
//        diff -ua mm_after_run.bin mm_before_run.bin > diff_dma_fpga.bin
//
//        xxd mm_before_run.bin > mm_before_run.bin.hex
//        xxd mm_after_run.bin > mm_after_run.bin.hex
//        diff mm_after_run.bin.hex mm_before_run.bin.hex > diff_dma_fpga.bin.hex
//
//        diff <(xxd mm_before_run.bin) <(xxd mm_before_run.bin) > diff_dma_fpga.bin.hex
            }
        } else { // run silent, use parameter test
            console.printf_normal("Autostarted Test Execution!\n");
            select = silent_testnr;
            console.printf_normal("\nSelected TESTS::");
            console.printf_normal_(testName(static_cast<TESTS>(select)));
            console.printf_normal("\n");
        }
/**
 * Test Execute
 */
        console.printf_normal("5. Transffering Input Data...\n");

        console.printf_normal("\t\t[%i | 0x%x] File: %s, Size: %i\n", MM_TEST_DATA_1, MM_TEST_DATA_1, "OPA", 2*NUM_TEST_ENTRIES);
        console.printf_normal("\t\t[%i | 0x%x] File: %s, Size: %i\n", MM_TEST_DATA_2, MM_TEST_DATA_2, "OPB", 2*NUM_TEST_ENTRIES);
        mutex.lock();
        bin_array_send(MM_TEST_DATA_1, 2* NUM_TEST_ENTRIES, (uint8_t *)(opa_mips));
        bin_array_send(MM_TEST_DATA_2, 2* NUM_TEST_ENTRIES, (uint8_t *)(opb_mips));
        mutex.unlock();

        // OP A+B
        console.printf_normal("\nOperand A (SRC1)                                                              | Operand B (SRC2) \n");
        console.printf_normal("=============================================================================================================================================================\n");
        for (int j = 0; j < NUM_TEST_ENTRIES; j += 8) {
            // print a line
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
        console.printf_normal("=============================================================================================================================================================\n");

        QList<TESTS> testlist = testmap[static_cast<TESTS>(select)];
        QList<TESTS> tests_fails;
        QList<TESTS> tests_success;
        QMap<TESTS, uint64_t> runtime;

        while (!testlist.empty()) {
            TESTS currentTest = testlist.front();
            testlist.pop_front();

            auto *testflag = new uint32_t(0);
            *testflag = static_cast<uint32_t>(currentTest);
//            console.printf_normal("6. Setting Start Flag to %i\n", *flag);
            mutex.lock();
            bin_array_send(MM_START_FLAG, 1, testflag);
            mutex.unlock();

            auto test_start_time = std::chrono::high_resolution_clock::now();
//            console.printf_normal("\t ->VPRO is running now!\n\n");
            console.printf_normal("[%3i/%3i]", tests_success.size() + tests_fails.size(),
                   tests_success.size() + tests_fails.size() + testlist.size());

            mutex.lock();
            auto selected_test = static_cast<TESTS>(get_debug_fifo_entry());
            mutex.unlock();
            console.printf_warning(" TEST:");
            console.printf_warning_(testName(selected_test));

//            console.printf_normal("7. wait for VPRO to finish processing...\n");
            int return_cnt = 0;
            uint32_t sys_time_lo = 0;
            while (true) {
                mutex.lock();
                // READ DEBUG FIFO
                int tmp = 0;
                unsigned char debug_buf_tmp[8];
                while(true){
                    // get single word from fifo
                    dma_common_read((char*) debug_buf_tmp, DEBUG_FIFO_ADDR, sizeof(debug_buf_tmp));
                    if ((debug_buf_tmp[0] >> 7) != 0) { // valid flag set? (bit #63)
                        tmp =       (debug_buf_tmp[4] << 24)  | (debug_buf_tmp[5] << 16);
                        tmp = tmp | (debug_buf_tmp[6] <<  8)  | (debug_buf_tmp[7] <<  0);
                        break;
                    }
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    auto time_now = std::chrono::high_resolution_clock::now();
                    auto duration = std::chrono::duration_cast<std::chrono::seconds>( time_now - test_start_time ).count();
                    if (duration >= MAX_TEST_DURATION){
                        console.printf_error("\n##########################################################################\n");
                        console.printf_error("[ERROR] TEST running too long (> maximum time: %is [communication.h])\n", MAX_TEST_DURATION);
                        console.printf_error("##########################################################################\n");
                        success = false;
                        mutex.unlock();
                        exit(2);
                        return;
                    }
                }
                mutex.unlock();
//                console.printf_error("\t [DEBUG] received: (0x%x = %i)\n", tmp, tmp);
                if (tmp == SIGNAL_DONE) {
//                    console.printf_normal("\t received SIGNAL_DONE (%x)\n\n", SIGNAL_DONE);
                    break;
                } else if (tmp == (SIGNAL_DONE|0x0000dead)) {
                    console.printf_error("\tTest not implemented!");
                } else  {
                    if (return_cnt == SIGNAL_SYS_TIME_LO) {
                        sys_time_lo = tmp;
                        return_cnt = -1;
                        continue;
                    }
                    if (return_cnt == SIGNAL_SYS_TIME_HI) {
                        uint64_t runtime_value = uint64_t(sys_time_lo) + (uint64_t(tmp) << 32);
                        runtime[currentTest] = runtime_value;
                        console.printf_normal("\tSystem Time: %6li", runtime_value);
                        return_cnt = -1;
                        continue;
                    }
                    if ((tmp == SIGNAL_SYS_TIME_LO) || (tmp == SIGNAL_SYS_TIME_HI)) {
                        return_cnt = tmp;
                        continue;
                    }
                    console.printf_normal("\tDEBUG FIFO read: %i [0x%x]\n", tmp, tmp);
                    usleep(10000); // 10ms
                }
            }
            fflush(stdout);
            // dump results to file
//            console.printf_normal("Dumping Result Array (Size: %i) to %s ...\n", 2 * NUM_TEST_ENTRIES * num_cl * num_vu_per_cl,
//                   output);
            mutex.lock();
            bin_file_dump(MM_TEST_RESULT, 2 * NUM_TEST_ENTRIES * num_cl * num_vu_per_cl, output.toStdString().c_str());
            mutex.unlock();

/**
 * VERIFY RESULT AND PRINT
 */
            // Reference Result
            int32_t *reference_result = execute(console, static_cast<TESTS>(currentTest), opa, opb);

            int entries = NUM_TEST_ENTRIES;
            if (static_cast<TESTS>(currentTest) == CNN_28_K1 || static_cast<TESTS>(currentTest) == CNN_28_K3){
                entries = 180+28*28;
            } else if (static_cast<TESTS>(currentTest) == CNN_7_K3 || static_cast<TESTS>(currentTest) == CNN_7_K1){
                entries = 180+7*7;
            }
            // get VPRO Result
            auto *vpro_result_ = new uint8_t[2*entries];
            mutex.lock();
            bin_array_dump(MM_TEST_RESULT, 2 * entries, vpro_result_);
            mutex.unlock();
            // switch endianess
            for (int j = 0; j < entries; ++j) {
                auto a = vpro_result_[2 * j + 0];
                vpro_result_[2 * j + 0] = vpro_result_[2 * j + 1];
                vpro_result_[2 * j + 1] = a;
            }
            auto *vpro_result = (uint16_t *) (vpro_result_);

            check_correctness(console, vpro_result, reference_result, currentTest, tests_fails, tests_success, printHex, nullptr, nullptr, entries);
            console.printf_normal("\n");
            fflush(stdout);
        } // testlist loop

        console.printf_normal("----------------------------------------------------------------------------------\n");
        console.printf_normal("Runtime: (vertical: time, horizontal: test)\n");
        int RUNTIME_HIST_LINES = 20;
        int max_runtime = 0;
        int64_t total_runtime = 0;
        QMapIterator<TESTS, uint64_t> i(runtime);
        while (i.hasNext()) {
            i.next();
            if (i.value() > max_runtime) max_runtime = i.value();
            total_runtime += i.value();
        }
        console.printf_normal("\tTotal: %li\n", total_runtime);
        for (int k = 0; k < RUNTIME_HIST_LINES; ++k) {
            QMapIterator<TESTS, uint64_t> i(runtime);
            while (i.hasNext()) {
                i.next();
                if (i.value() >= (RUNTIME_HIST_LINES - k) * max_runtime / RUNTIME_HIST_LINES)
                    console.printf_normal("#");
                else
                    console.printf_normal(" ");
            }
            console.printf_normal("\n");
        }
        console.printf_normal("----------------------------------------------------------------------------------\n");


        int tests_count = tests_fails.size() + tests_success.size();
        if (tests_count > 1) {
            console.printf_normal("###### VPRO Test Finished. Summary #####################\n");

            for (auto t : tests_success) {
                console.printf_normal("\t");
                console.printf_success_(testName(t));
                console.printf_normal("\n");
            }
            if (!tests_success.empty())
                console.printf_success(" Success: %i / %i\n", tests_success.size(), tests_count);
            else
                console.printf_error(" Success: %i / %i\n", tests_success.size(), tests_count);

            for (auto t : tests_fails) {
                console.printf_normal("\t");
                console.printf_error_(testName(t));
                console.printf_normal("\n");
            }
            if (!tests_fails.empty())
                console.printf_error(" Fails \xF0\x9F\x98\xA1 : %i / %i\n", tests_fails.size(), tests_count);
            else
                console.printf_normal(" Fails: NONE! \xF0\x9F\x98\x81\n"); //, tests_fails.size(), tests_count);

            console.printf_normal("------ Summary Done --------------------------------------------------------------\n");
        } else {
            console.printf_normal("------ Test Done -----------------------------------------------------------------\n");
        }

        if(silent){ // quit execution and return error | success
            if (tests_fails.empty())
                success = true;
            console.printf_normal("##########################################################################\n");
            console.printf_normal("Autostarted Test Execution Done!\n");
            console.printf_normal("\t ->VPRO is stopping now!\n");
            console.printf_info(success?"\t[Success \xF0\x9F\x98\x81]":"\t[Fail \xF0\x9F\x98\xA1]");
            console.printf_info(" on Test: ");
            console.printf_info_(testName(static_cast<TESTS>(select)) + "\n");
            console.printf_normal("##########################################################################\n");
            *flag = 0;
            mutex.lock();
            bin_array_send(MM_RUNNING_FLAG, 8, (uint8_t *) flag);
            mutex.unlock();

            exit(!tests_fails.empty());
            return;
        }
    } // select new Test loop // vpro running flag

/**
 * Quit vpro loop
 */
    console.printf_normal("\n\t ->VPRO is stopping now!\n\n");
    *flag = 0;
    mutex.lock();
    bin_array_send(MM_RUNNING_FLAG, 8, (uint8_t *) flag);
    mutex.unlock();
}
