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
#include "helper.hpp"

// UEMU libraries
#include <dma_common.h>
#include <mcpa.h>

// QT
#include <QMutex>
#include <QtCore>
#include <QApplication>

// CORE communication
#include "AuxDebugFifoReader.h"
#include "CnnCommunication.h"
#include "MipsDebugReader.h"
#include "RiscCommunication.h"
#include "ConsolePrinter.h"

#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

CnnCommunication * cnn;
void signalHandleWrapper(int s)
{
    cnn->ctrl_c_handler(s);
}

int main(int argc, char **argv) {
    auto *a = new QApplication(argc, argv);
    ConsolePrinter console = ConsolePrinter();
    console.start();

    QCoreApplication::setApplicationVersion("1.0");
    QCoreApplication::setApplicationName("yololite");

    QMutex mutex;

    QCommandLineParser parser;
    parser.setApplicationDescription("CNN App for VPRO Execution of YOLO-LITE");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("executable", QCoreApplication::translate("main", "Binary VPRO Executable"));

    const QCommandLineOption liveOption({"l", "live"},
                                        QCoreApplication::translate("main",
                                                                    "Live Capture of Webcam Videostream to be sent to VPRO [TODO]"),
                                        QCoreApplication::translate("main", "live"));
    const QCommandLineOption unitOptions({"u", "units"},
                                         QCoreApplication::translate("main", "Unit count per Cluster"),
                                         QCoreApplication::translate("main", "units"),
                                         "2");
    const QCommandLineOption clusterOption({"c", "clusters"},
                                           QCoreApplication::translate("main", "Cluster count"),
                                           QCoreApplication::translate("main", "clusters"),
                                           "2");
    parser.addOption(liveOption);
    parser.addOption(unitOptions);
    parser.addOption(clusterOption);
    parser.process(*a);

    const QStringList args = parser.positionalArguments();
    if (args.size() < 1) {
        console.printf_error("Not enough input arguments!\n\n");
        parser.showHelp();
    }
    QString executable = args[0];

    bool live = parser.isSet(liveOption);
    int num_cl = parser.value(clusterOption).toInt();
    int num_vu_per_cl = parser.value(unitOptions).toInt();


/**
 * Initialize
 */
    // Disable buffering on std out
    setbuf(stdout, nullptr);

    console.printf_info("UEMU init [Port: %s]... \n", QProcessEnvironment::systemEnvironment().value("UEMU_PORT_START",
                                                                                                     "NONE").toStdString().c_str());
    init_connection_via_cfg(0);
    console.printf_info("set reset... \n");
    set_reset();
    console.printf_info("sending executable [%s]... \n", argv[1]);
    send_executable(argv[1]);
    console.printf_info("releasing reset...\n");
    release_reset();

    console.printf_normal("\nExecuting '%s' on hardware (1. parameter)...\n\n", argv[1]);

    int layers_size_bytes = get_debug_fifo_entry();
    int layers_number_total = get_debug_fifo_entry();
    int segments_size_bytes = get_debug_fifo_entry();
    int segments_total_number = get_debug_fifo_entry();
    int weights_size_total = get_debug_fifo_entry();
    console.printf_normal(
            "0. RISC Array dimensions: \n\tLayer[%u] takes %u Bytes, Segments[%u] takes %u Bytes, Weights take %u Bytes\n\n",
            layers_number_total, layers_size_bytes,
            segments_total_number, segments_total_number * segments_size_bytes, weights_size_total);

    auto risc = new RiscCommunication(mutex, console);

    console.printf_normal("1. Receiving Addresses for communication_risc\n");
    risc->setCommunicationRisc(get_debug_fifo_entry());
    console.printf_normal("\tcommunication_risc: %i [0x%x]\n", risc->getCommunicationRisc(),
                          risc->getCommunicationRisc());
    risc->setCommunicationHost(get_debug_fifo_entry());
    console.printf_normal("\tcommunication_host: %i [0x%x]\n", risc->getCommunicationHost(),
                          risc->getCommunicationHost());
    risc->setCommunicationRISCData(get_debug_fifo_entry());
    console.printf_normal("\tcommunication_risc_data: %i [0x%x]\n", risc->getCommunicationRISCData(),
                          risc->getCommunicationRISCData());
    risc->setCommunicationHOSTData(get_debug_fifo_entry());
    console.printf_normal("\tcommunication_host_data: %i [0x%x]\n", risc->getCommunicationHOSTData(),
                          risc->getCommunicationHOSTData());
    int MM_RUNNING_FLAG = get_debug_fifo_entry();
    int MM_DATA_AVAI_FLAG = MM_RUNNING_FLAG + 4;
    int MM_DCACHE_CLEARED_FLAG = MM_DATA_AVAI_FLAG + 4;
    console.printf_normal("\tMM_RUNNING_FLAG: %i [0x%x]\n", MM_RUNNING_FLAG, MM_RUNNING_FLAG);
    console.printf_normal("\tMM_DATA_AVAI_FLAG: %i [0x%x]\n", MM_DATA_AVAI_FLAG, MM_DATA_AVAI_FLAG);
    console.printf_normal("\tMM_DATA_AVAI_FLAG: %i [0x%x]\n", MM_DCACHE_CLEARED_FLAG, MM_DCACHE_CLEARED_FLAG);

    // PRINT all Flags
    console.printf_normal("\n2. Reading Addresses data content ( containing first data to be transmitted already ) \n");
    console.printf_success("    communication_risc: %lx\n", risc->getData(risc->getCommunicationRisc()));
    console.printf_success("    communication_host: %lx\n", risc->getData(risc->getCommunicationHost()));
    console.printf_success("             risc data: %lx\n", risc->getData(risc->getCommunicationRISCData()));
    console.printf_success("             host data: %lx\n", risc->getData(risc->getCommunicationHOSTData()));
    console.printf_success("       MM_RUNNING_FLAG: %lx\n", risc->getData(MM_RUNNING_FLAG));
    console.printf_success("     MM_DATA_AVAI_FLAG: %lx\n", risc->getData(MM_DATA_AVAI_FLAG));
    console.printf_success("MM_DCACHE_CLEARED_FLAG: %lx\n", risc->getData(MM_DCACHE_CLEARED_FLAG));

    auto fifoReader = new AuxDebugFifoReader(mutex, console);
    auto mipsDebug = new MipsDebugReader(mutex, console);
    mipsDebug->start();
    fifoReader->start();

    cnn = new CnnCommunication(*risc, console, MM_RUNNING_FLAG, MM_DATA_AVAI_FLAG, layers_size_bytes,
                                    layers_number_total, segments_size_bytes, segments_total_number,
                                    weights_size_total);
    QCoreApplication::connect(cnn, SIGNAL(finished()), a, SLOT(quit()));
    QCoreApplication::connect(cnn, SIGNAL(finished()), mipsDebug, SLOT(quit()));
    QCoreApplication::connect(cnn, SIGNAL(finished()), &console, SLOT(quit()));

    // Catch Ctrl-C
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = signalHandleWrapper;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);

    cnn->start();

    int app_return = QCoreApplication::exec();
    cnn->deleteLater();
    cnn->quit();
    console.printf_info("\nret (success): %i [shutting down ...]\n", app_return);
    mutex.unlock();
    cnn->wait(1000);
    mipsDebug->quit();
    console.quit();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return app_return;
}
