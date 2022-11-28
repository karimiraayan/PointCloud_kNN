// ############################################################
// # VPRO instruction/operation verification framework        #
// # ---------------------------------------------------------#
// # HOST application to send executable, OpA data and OpB    #
// # to the processor and to read-back results when receiving #
// # a specific flag via the debug FIFO.                      #
// #                                                          #
// # Stephan Nolting, IMS, Uni Hannover, September 2018       #
// ############################################################

// C std libraries
#include <unistd.h>
#include <cctype>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>

#include <chrono>
#include <thread>

// UEMU libraries
#include <dma_common.h>
#include <mcpa.h>

// QT
#include <QMutex>
#include <QtCore>
#include <QApplication>

// CORE communication
#include "AuxDebugFifoReader.h"
#include "communication.h"
#include "MipsDebugReader.h"
#include <ConsolePrinter.h>

// Memory areas (VPRO)
int MM_TEST_DATA_1;
int MM_TEST_DATA_2;
int MM_TEST_RESULT;
int MM_START_FLAG;
int MM_RUNNING_FLAG;
int MM_USE_SINGLE_DMA_TRANSFERS;


int main(int argc, char **argv) {
    auto *a = new QCoreApplication(argc, argv);
    ConsolePrinter console = ConsolePrinter();
    console.start();

    QCoreApplication::setApplicationVersion("1.0");
    QCoreApplication::setApplicationName("verifyHW");

    QMutex mutex;

    QCommandLineParser parser;
    parser.setApplicationDescription("Hardware Verification App for VPRO Functions");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("executable", QCoreApplication::translate("main", "Binary VPRO Executable"));
    parser.addPositionalArgument("file_a", QCoreApplication::translate("main", "Input data A"));
    parser.addPositionalArgument("file_b", QCoreApplication::translate("main", "Input data B"));
    parser.addPositionalArgument("output", QCoreApplication::translate("main", "Output data"));

    const QCommandLineOption silentOption({"s", "silent"},
                                          QCoreApplication::translate("main",
                                                                      "Silence (Autorun) execution of testnr. If test successfull -> return 0 | else -> return errorcode"),
                                          QCoreApplication::translate("main", "testnr"));
    const QCommandLineOption unitOptions({"u", "units"},
                                          QCoreApplication::translate("main", "Unit count per Cluster"),
                                          QCoreApplication::translate("main", "units"),
                                          "2");
    const QCommandLineOption clusterOption({"c", "clusters"},
                                          QCoreApplication::translate("main", "Cluster count"),
                                          QCoreApplication::translate("main", "clusters"),
                                          "2");
    parser.addOption(silentOption);
    parser.addOption(unitOptions);
    parser.addOption(clusterOption);
    parser.process(*a);

    const QStringList args = parser.positionalArguments();
    if (args.size() < 4){
        console.printf_error("Not enough input arguments!\n\n");
        parser.showHelp();
    }
    QString executable = args[0];
    QString input_a = args[1];
    QString input_b = args[2];
    QString output = args[3];

    bool silent = parser.isSet(silentOption);
    int testnr = parser.value(silentOption).toInt();

    int num_cl = parser.value(clusterOption).toInt();
    int num_vu_per_cl = parser.value(unitOptions).toInt();

/**
 * Initialize
 */

    // Disable buffering on std out
    setbuf(stdout, nullptr);

    console.printf_info("UEMU init [Port: %s]... \n", QProcessEnvironment::systemEnvironment().value("UEMU_PORT_START", "NONE").toStdString().c_str());
    init_connection_via_cfg(0);
    console.printf_info("set reset... \n");
    set_reset();
    console.printf_info("sending executable [%s]... \n", argv[1]);
    send_executable(argv[1]);
    console.printf_info("releasing reset...\n");
    release_reset();

    console.printf_normal("\nExecuting '%s' on hardware (1. parameter)...\n\n", argv[1]);

    console.printf_normal("1. Receiving Addresses for Result Data Array...\n");
    MM_TEST_RESULT = get_debug_fifo_entry();
    printf("\tMM_TEST_RESULT: %i [0x%x]\n", MM_TEST_RESULT, MM_TEST_RESULT);

    console.printf_normal("2. Receiving Addresses for Input 1 Data Array...\n");
    MM_TEST_DATA_1 = get_debug_fifo_entry();
    console.printf_normal("\tMM_TEST_DATA_1: %i [0x%x]\n", MM_TEST_DATA_1, MM_TEST_DATA_1);

    console.printf_normal("3. Receiving Addresses for Input 2 Data Array...\n");
    MM_TEST_DATA_2 = get_debug_fifo_entry();
    console.printf_normal("\tMM_TEST_DATA_2: %i [0x%x]\n", MM_TEST_DATA_2, MM_TEST_DATA_2);

    console.printf_normal("4a. Receiving Addresses for Start Flag...\n");
    MM_START_FLAG = get_debug_fifo_entry();
    console.printf_normal("\tMM_START_FLAG: %i [0x%x]\n", MM_START_FLAG, MM_START_FLAG);

    console.printf_normal("4b. Receiving Addresses for Running Flag...\n");
    MM_RUNNING_FLAG = get_debug_fifo_entry();
    console.printf_normal("\tMM_RUNNING_FLAG: %i [0x%x]\n", MM_RUNNING_FLAG, MM_RUNNING_FLAG);

    console.printf_normal("4c. Receiving Addresses for DMA singe step Flag...\n");
    MM_USE_SINGLE_DMA_TRANSFERS = get_debug_fifo_entry();
    console.printf_normal("\tMM_USE_SINGLE_DMA_TRANSFERS: %i [0x%x]\n", MM_USE_SINGLE_DMA_TRANSFERS, MM_USE_SINGLE_DMA_TRANSFERS);

    qInstallMessageHandler(ConsolePrinter::customHandler);

    auto fifoReader = AuxDebugFifoReader(mutex, console);
    // no start. started if fifo test selected -> 911

    auto mipsDebug = new MipsDebugReader(mutex, console);
    mipsDebug->start();

    auto cnn = new communication(mutex, fifoReader, console, MM_TEST_RESULT, MM_TEST_DATA_1, MM_TEST_DATA_2, MM_START_FLAG,
                                 MM_RUNNING_FLAG, MM_USE_SINGLE_DMA_TRANSFERS,
                                 executable, input_a, input_b, output, num_cl, num_vu_per_cl, silent, testnr, a);

    // finished -> quit ?
    QCoreApplication::connect(cnn, SIGNAL(finished()), a, SLOT(quit()));
    QCoreApplication::connect(cnn, SIGNAL(finished()), mipsDebug, SLOT(quit()));
    QCoreApplication::connect(cnn, SIGNAL(finished()), &fifoReader, SLOT(quit()));
    QCoreApplication::connect(cnn, SIGNAL(finished()), &console, SLOT(quit()));

    cnn->start();

    int app_return = QCoreApplication::exec();
    cnn->deleteLater();
    if (!cnn->success){
        app_return = 1;
        console.printf_info("ret: %i\n", app_return);
    }
    console.printf_info("ret (success): %i [shutting down ...]\n", app_return);
    cnn->quit();
    mipsDebug->quit();
    fifoReader.quit();
    console.quit();
    cnn->wait(10000);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    return app_return;
}
