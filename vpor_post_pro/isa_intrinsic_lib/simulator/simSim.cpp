// ########################################################
// # VPRO instruction & system simulation library         #
// # Sven Gesper, IMS, Uni Hannover, 2019                 #
// ########################################################
// # simulator related functions (start/stop/...)         #
// ########################################################

#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <limits.h>
#include <QDebug>
#include <QThread>
#include <QString>
#include <QFile>
#include <QApplication>
#include <QtCore/QFuture>
#include <QCoreApplication>
#include <QtConcurrent/QtConcurrent>
#include <QThread>

#include "simCore.h"
#include "helper/debugHelper.h"
#include "VectorMain.h"


QFile *CMD_HISTORY_FILE;
QTextStream *CMD_HISTORY_FILE_STREAM;

SimCore::SimCore() :
        main_memory_size(1024*1024*1024),
        local_memory_size(8192),
        register_file_size(1024),
        cluster_count(1),
        vector_unit_count(1),
        vector_lane_count(2),
        pipeline_depth(10),
        simstat(1,1), // overwritten in simInit
        dma_pad_top(0),
        dma_pad_right(0),
        dma_pad_bottom(0),
        dma_pad_left(0),
        dma_pad_value(0)
{
    sim_finished = false;
    sim_running = false;
    windowless = false;

    unit_mask_global = 0xffffffff;
    cluster_mask_global = 0xffffffff;
    clock = 0;
    goal_clock = 0;
    performance_clock_last_second = 0;

    aux_cycle_counter = 0;
    aux_sys_time = 0;

    windowThread = true;

    isCompletelyInitialized = false;
    isPrintResultDone = false;

    performanceMeasurement = new QTimer();
    performanceMeasurementStart = QTime::currentTime();

    SIM_ONLY_WARNING_PRINTED_ONCE_XEND_YEND = true;
    SIM_ONLY_WARNING_PRINTED_ONCE_SHIFT_LL = true;

    simPause();
}

// ###############################################################################################################################
// Simulator Environment
// ###############################################################################################################################

// ---------------------------------------------------------------------------------
// Initialize simulation environment
// ---------------------------------------------------------------------------------
int SimCore::sim_init(int (*main_fkt)(int, char **), int &argc, char *argv[]) {
    return sim_init(main_fkt, argc, argv, main_memory_size, local_memory_size, register_file_size, cluster_count,
                    vector_unit_count, vector_lane_count, pipeline_depth);
}

int
SimCore::sim_init(int (*main_fkt)(int, char **), int &argc, char *argv[], const uint32_t main_memory_size, const uint32_t local_memory_size,
                  const uint32_t register_file_size, const int cluster_count, const int vector_unit_count, const int vector_lane_count,
                  const int pipeline_depth) {

    if (windowThread){
        for (int i = 1; i < argc; ++i){
            if (!qstrcmp(argv[i], "--windowless") || !qstrcmp(argv[i], "--silent")){
                printf_warning("Gonna run silent, without windows [--windowless]\n");
                windowless = true;
                simResume();
            }
        }
    }

    if (!windowThread) {
        // second call skip - parallel thread - simulate program now

        this->main_memory_size = main_memory_size;
        this->local_memory_size = local_memory_size;
        this->register_file_size = register_file_size;
        this->cluster_count = cluster_count;
        this->vector_unit_count = vector_unit_count;
        this->vector_lane_count = vector_lane_count;
        this->pipeline_depth = pipeline_depth;

        // Disable buffering on std out
        setbuf(stdout, NULL);

        printf("# ---------------------------------------------------------------------------------\n");
        //printf("# VPRO instruction set & system simulator \n");
        //printf("# ---------------------------------------------------------------------------------\n");
        //printf("#  Clusters:  %3d, Units (per Cl): %3d, Processing Lanes (per VU): %3d", cluster_count, vector_unit_count, vector_lane_count);
        //printf("#   Total VUs: %3d, Total Processing Lanes:    %3d \n",
         //       vector_unit_count * cluster_count,
          //      cluster_count * vector_unit_count * vector_lane_count);
        //printf("#\n");
        //printf("#  Main Memory:    8-bit x %6d\n", main_memory_size);
        //printf("#  Local Memory:  16-bit x %6d\n", local_memory_size);
        //printf("#  Register File: 24-bit x %6d\n", register_file_size);
        //printf("# ---------------------------------------------------------------------------------\n\n");

        //printf("#SIM: Allocating memory... (Size: %d Bytes)\n", main_memory_size);
        main_memory = new uint8_t[main_memory_size]();

        auto *main_adress = &main_memory;
        emit sendmainmemory(main_memory);

        //printf("#SIM: Calling initialization Script %s ... ", initscript.toStdString().c_str());
        std::ifstream init_script(initscript.toStdString().c_str());
        if (!init_script) {
            printf_warning("[File '%s' not found!]\n", initscript.toStdString().c_str());
        } else {
            int status = system(QString("bash ").append(initscript).toStdString().c_str());
            //printf("[executed, returned %i]\n", status);
        }

        // ########################################################################
        // Read input to MM (.cfg)
        // ########################################################################
        if (argc > 2 && windowless) {
            inputcfg = QString(argv[2]);
        } else if (argc >= 2 && !windowless) {
            inputcfg = QString(argv[1]);
        }
        if (argc >= 4 && windowless) {
            outputcfg = QString(argv[3]);
        } else if (argc >= 3) {
            outputcfg = QString(argv[2]);
        }

        //printf_info("#SIM: Input settings from %s\n", inputcfg.toStdString().c_str());
        QFile configfile(inputcfg);
        configfile.open(QIODevice::ReadOnly);
        while (!configfile.atEnd()) {
            QString line = configfile.readLine();
            line = line.simplified();
            if (line.startsWith(";") or line.startsWith("#"))
                continue;
            auto input_items = line.split(" ");
            if (input_items.size() < 2){
                printf_warning("Input file not containing enough items [Required: File, Addres]\n");
                continue;
            }
            QString input_file = input_items[0];
            int address = input_items[1].toInt();
            QFile input(input_file);
            if (!input.open(QIODevice::ReadOnly)){
                printf_warning("Input could not be opened! [File: %s]\n", input_file.toStdString().c_str());
                continue;
            }
            QByteArray input_data = input.readAll();
            for(int i = 0; i<input_data.size(); i++){
                main_memory[address + i] = uint8_t(input_data.at(i));
//                if (i < 10)
//                printf_info("%i: %i / %x\n", i, main_memory[address + i], main_memory[address + i]);
            }
            //printf_info("\tFile %s was read to MM [%i (Dez) / 0x%x (Hex)]\t Size: %i\n", input_file.toStdString().c_str(), address, address, input_data.size());
        }

        // ########################################################################
        // check global variables . their addresses have to be outside MM
        // ########################################################################
        size_t length = 0;
        if (copyGlobalSectionToMMStart) {
            std::ifstream globals("globals.section");
            if (!globals) {
                printf_warning("#SIM: file globals.section not found! Won't copy content to MM start.\n");
            } else {
                globals.seekg(0, globals.end); //get length of file
                length = globals.tellg();
                globals.seekg(0, globals.beg);
                char *buffer = new char[length]; // allocate buffer
                globals.read(buffer, length); //read file
                for (int i = 0; i < length; i++) {
                    main_memory[i] = (uint8_t) buffer[i];
                }
                printf("#SIM: Loaded all Global Variables to main_memory start [address: 0] (Size: %i)\n",
                       (int) length); // (__attribute__ ((section ("glob"))))
            }
        }
        if (debug & DEBUG_GLOBAL_VARIABLE_CHECK) {
            printf("#SIM: For Simulation the global Variables are accessed by their address. Checking...:\n");

            FILE *fp;
            char path[1035];
            std::string buf("nm --print-size ");
            buf.append(argv[0]);
            buf.append(
                    " | grep '[0-9a-f]* r _ZL'"); // nm prints all symbols + addresses + size, filter for r _ZL (global variables beginn with this with gcc...)
            fp = popen(buf.c_str(), "r");
            if (fp == NULL) {
                printf_error("Failed to run \"nm\" command\n");
                //exit(1);
            }
            char delimiter[] = " \n";
            char *ptr; /* Read the output a line at a time - output it. */

            while (fgets(path, sizeof(path) - 1, fp) != NULL) {
                char *name;
                int address, size;
                ptr = strtok(path, delimiter);  // split with delimiter
                if (ptr != NULL) {
                    address = (int) strtol(ptr, NULL, 16);
                    ptr = strtok(NULL, delimiter);
                    size = (int) strtol(ptr, NULL, 16);
                    ptr = strtok(NULL, delimiter);
                    ptr = strtok(NULL, delimiter);
                    name = ptr;
                }
                if (address < main_memory_size) {
                    printf_error(
                            "\tObject Failed: external Object inside main memory [0 ... %li] (not addressable) -- name: %s @%i [size: %i]\n",
                            main_memory_size, name, address, size);
                } else if (address >= main_memory_size && (debug & DEBUG_GLOBAL_VARIABLE_CHECK)) {
                    QMap<int, QString> object;
                    object[size] = name;
                    external_variables[address + (uint64_t) (&main_memory)] = object;
                    printf_info(
                            "\tObject Passed: external Object outside main memory [0 ... %li] (addressable)     -- name: %s @%i [size: %i]\n",
                            main_memory_size, name, address, size);
                }
            }
            pclose(fp);
        }

        // ########################################################################
        // Create Cluster
        // ########################################################################
        //printf("\n");
        for (int i = 0; i < cluster_count; i++) {
            clusters.push_back(
                    new Cluster(this, i, unit_mask_global, loop_mask_src1, loop_mask_src2, loop_mask_dst,
                                ACCU_MAC_HIGH_BIT_SHIFT, ACCU_MUL_HIGH_BIT_SHIFT,
                                dma_pad_top, dma_pad_right, dma_pad_bottom, dma_pad_left, dma_pad_value,
                                main_memory, clock,
                                main_memory_size, vector_unit_count,
                                local_memory_size, vector_lane_count, register_file_size, pipeline_depth));
            clusters.back()->dma->setExternalVariableInfo(&external_variables);
        }
        if (!windowless) {
            int c=0;
            for (auto cluster : clusters) {
                int u=0;
                for (auto unit : cluster->getUnits()) {
                    w->setLocalMemory(unit->vector_unit_id, unit->getlocalmemory(),c);
                    for(auto lane: unit->getLanes()){
                        if(lane->vector_lane_id != log2(int(LS)))
                            w->setRegisterMemory(lane->vector_lane_id,lane->getregister(),c,u);
                    }
                u++;
                }
             c++;
            }
        }
        simstat = SimStat(cluster_count, vector_unit_count);
        //printf("\n");

#ifdef THREAD_CLUSTER
//        busythreads.release(cluster_count);
        if(cluster_count >= 2 && vector_unit_count >= 2) {
            for (Cluster *cluster : clusters) {
                cluster->tick_en.lock();
                cluster->tick_done.lock();
                cluster->start();
            }
        }
#endif
        if (CREATE_CMD_HISTORY_FILE) {
            CMD_HISTORY_FILE = new QFile(CMD_HISTORY_FILE_NAME);

//            if (QFileInfo::exists(CMD_HISTORY_FILE_NAME)) {
//                if (!CMD_HISTORY_FILE->open(QIODevice::WriteOnly | QIODevice::Append)) {
//                    printf_error("CMD History file could not be opened! [FILE : %s]\n", CMD_HISTORY_FILE);
//                }
//            } else {
            if (!CMD_HISTORY_FILE->open(QIODevice::WriteOnly | QIODevice::Truncate)) {
                printf_error("CMD History file could not be opened! [FILE : %s]\n", CMD_HISTORY_FILE);
            }
            if (CMD_HISTORY_FILE->isWritable()) {
                CMD_HISTORY_FILE_STREAM = new QTextStream(CMD_HISTORY_FILE);
            } else {
                printf_error("CMD History File is not writeable! [FILE : %s]\n", CMD_HISTORY_FILE);
            }
        }

        //printf("#SIM: Simulator environment initialization done.\n");
        //printf("# ---------------------------------------------------------------------------------\n");
        isCompletelyInitialized = true;
        // return to main and simulate program in this thread
        return 0;
    }
    //GUI Thread (Sim-Init, qApp + Window)
    windowThread = false;

    if (!windowless){
        a = new QApplication(argc, argv); //createApplication(argc, argv);
    } else {
        a = new QCoreApplication(argc, argv);
    }

    qRegisterMetaType<CommandWindow::Data>("CommandWindow::Data");
    qRegisterMetaType<QVector<int >>("QVectorint");
    qRegisterMetaType<QVector<LaneStat >>("QVector<LaneStat>");

    // start main again (as simulator thread)
    VectorMain *simulatorThreadProgram = new VectorMain(main_fkt, argc, argv);
    QThread simulatorThread;
    simulatorThreadProgram->moveToThread(&simulatorThread);
    connect(this, &SimCore::runMain, simulatorThreadProgram, &VectorMain::doWork); // change to vector_main()
    simulatorThread.start();

    // this is the try to close all threads upon gui/this thread ends
    connect(this, &QThread::finished, this, &QObject::deleteLater);
    connect(this, SIGNAL(finished()), &simulatorThread, SLOT(quit()));
    connect(this, SIGNAL(finished()), &simulatorThread, SLOT(deleteLater()));

    // regular finish of simulated program
//    connect(this, &SimCore::simFinish, this, &SimCore::printSimResults);
//    connect(this, &SimCore::simFinish, this, &SimCore::sendSimUpdate);
//    connect(this, SIGNAL(simFinish()), &simulatorThread, SLOT(quit()));

    // status signals of simulation to send update to GUI
    connect(this, &SimCore::simIsPaused, this, &SimCore::sendSimUpdate);
    connect(this, &SimCore::simIsResumed, this, &SimCore::sendSimUpdate);

    // connect 1sec timer to measure simulator speed each second
    connect(performanceMeasurement, SIGNAL(timeout()), this, SLOT(performanceMeasureTimeout()), Qt::DirectConnection);
    connect(this, SIGNAL(performanceMeasurement_stop()), performanceMeasurement, SLOT(stop()), Qt::DirectConnection);
    connect(this, SIGNAL(performanceMeasurement_start(int)), performanceMeasurement, SLOT(start(int)),Qt::DirectConnection);
    connect(this, SIGNAL(finished()), performanceMeasurement, SLOT(deleteLater()));

    if (!windowless) {
        w = new CommandWindow(cluster_count, vector_unit_count, 2);
        connect(this, SIGNAL(dataUpdate(CommandWindow::Data)), w, SLOT(dataUpdate(CommandWindow::Data)));
        connect(this, SIGNAL(simUpdate(long)), w,SLOT(simUpdate(long)));
        connect(this, SIGNAL(simIsPaused()), w, SLOT(simIsPaused()));
        connect(this, SIGNAL(simIsResumed()), w, SLOT(simIsResumed()));
        connect(w, SIGNAL(runSteps(int)), this, SLOT(runSteps(int)), Qt::DirectConnection);
        connect(w, SIGNAL(pauseSim()), this, SLOT(pauseSim()), Qt::DirectConnection);
        connect(w, SIGNAL(destroyed()), this, SLOT(quitSim()), Qt::DirectConnection);
        connect(w, SIGNAL(dumpLocalMemory(int, int)), this, SLOT(dumpLM(int, int)), Qt::DirectConnection);
//        connect(w, SIGNAL(dumpQueue(int, int)), this, SLOT(dumpQ(int, int)), Qt::DirectConnection);
        connect(w, SIGNAL(dumpRegisterFile(int, int, int)), this, SLOT(dumpRF(int, int, int)), Qt::DirectConnection);
        connect(this,SIGNAL(sendGuiwaitkeypress(bool*)),w,SLOT(recieveguiwaitpress(bool*)));
        connect(this, SIGNAL(sendmainmemory(uint8_t * )), w, SLOT(getmainmemory(uint8_t * )));
        connect(w, SIGNAL(getSimupdate()), this, SLOT(sendSimUpdate()), Qt::DirectConnection);
        //connect(this, SIGNAL(sendLocalmemory(uint8_t * )), w, SLOT(saveLocalmemory(uint8_t * )));

        w->show();

        emit simIsPaused();
        emit simUpdate(clock);
//        if (!windowless) {
//            int c=0;
//            for (auto cluster : clusters) {
//                for (auto unit : *cluster->getUnits()) {
//                    w->setLocalMemory(unit->vector_unit_id, unit->getlocalmemory(),c);
//                }
//            c++;
//            }
//        }

    } else {
//        qDebug() << "windowless!";
    }

    emit runMain(); // run main/vpro in other thread
    a->exec(); // GUI Loop

    emit performanceMeasurement_stop();
    sim_finished = true;
    simulatorThread.quit();
    simulatorThread.deleteLater();

    delete performanceMeasurement;
    performanceMeasurement = NULL;

//  performed from other thread in run()...; printSimResults(); sim_exit();
    while (!isPrintResultDone) {
        msleep(50);
    }

    CMD_HISTORY_FILE->close();
    free(main_memory);
    //printf("#SIM: MainMemory released.\n");
    //printf("#SIM: Done.\n");

    // needs exit or main() will continue...
    exit(EXIT_SUCCESS); // will continue?  with main!?
    std::_Exit(EXIT_SUCCESS);
//    std::terminate(); // no exception warning. is killing the app
//    throw std::exception(); // not tested
}

void SimCore::sim_stop() {
    while (!sim_finished) {
        bool cluster_busy = false;
        for (auto cluster : clusters) {
            cluster_busy |= cluster->isBusy();
        }
        if (cluster_busy){
            run();
            continue;
        }
        break;
    }
    simPause();
    printSimResults(true);
    sim_exit();
    sim_finished = true;
    isPrintResultDone = true; // let other main exit

    //printf("\tSim_Stop(); ~Program Finished Successful!");


    if (windowless || true) {
        a->quit();
    }

//    exit(EXIT_SUCCESS);
//    std::_Exit(EXIT_SUCCESS);
}

void SimCore::performanceMeasureTimeout() {

    // called once per second or on pause
    // if sim is running
    // get time and # of processed clock cycles
    // print on qt window status bar?

    auto now = QTime::currentTime();
    int time = performanceMeasurementStart.msecsTo(now);
    auto cycles = clock - performance_clock_last_second;

    if (time > 0 && cycles > 0) {
        printf("Cycles in last %i msec: %li \n", time, cycles);
        printf("Cycles / sec: %f \n", ((float) cycles / (float) time * 1000.0));
    }

    performance_clock_last_second = clock;
    performanceMeasurementStart = now;
}

// ---------------------------------------------------------------------------------
// Run simulation environment for one cycle (until finished)
// ---------------------------------------------------------------------------------
void SimCore::run() {
    if (!isCompletelyInitialized)
        return;

    // clock loop
    if (!sim_finished) {
        while (!isSimRunning()) {
            msleep(1);
        }

        if (!windowless) { // update GUI?
            if (goal_clock > 0 && goal_clock <= clock) { // running until goal_clock done
                goal_clock = -1;
                simPause(); // set pause mode
                sendSimUpdate();
                run();
                return;
            }
            if (((cluster_count*vector_unit_count) < 10 && clock % 50000 == 0) ||
                                                           clock % 10000 == 0) { // update gui clock every x cycles
                sendSimUpdate();
            }
        }

        // clock tick
        clk_tick();
        return;
    }

    // finished button from GUI . cleanup?
    printf("#SIM: Run-Loop exit. GUI got destroyed while sim running. Simulation exit (clock-cycles = %li)\n", clock);
    printf_warning("Results haven't been written into results.csv!");
    printSimResults();
    printSimResults(true);
    sim_exit();
    isPrintResultDone = true; // let other main exit

    exit(EXIT_SUCCESS);
    std::_Exit(EXIT_SUCCESS);
}

void SimCore::runUntilReadyForCmd() {
    bool clusterClocking = false;
    while (true) {

        run();

        // check if any cluster is rdy for a cmd (queue not full, no dma|vpro wait_busy, loop not blocking)
        // TODO: maybe wrong behaviour if different cluster process different commands (complex)
        clusterClocking = false;
        for (auto c: clusters) {
            clusterClocking |= !c->isReadyForCommand();
        }

        // if no cluster ready or clock not rdy for mips cmd => another tick
        if(clock % 4 != 0 || clusterClocking)
            continue;
        else {
            break;
        }
    }
}

void SimCore::runUntilMIPSReadyForCmd() {
    while (true) {
        run();
        if (clock % 4 != 0)
            continue;
        else
            break;
    }
}


// ---------------------------------------------------------------------------------
// Run simulation environment (one clock tick)
// ---------------------------------------------------------------------------------
void SimCore::clk_tick() {

    clock++;
    aux_sys_time++;
    aux_cycle_counter++;

    simstat.clockTick(*this);
//    if (clock > 111) sim_stop();

    if (debug & DEBUG_GLOBAL_TICK)
        printf("Ticking clock: %li\n", clock);

#ifdef THREAD_CLUSTER
    if(cluster_count >= 2 && vector_unit_count >= 2){
        for (auto cluster: clusters) {
            cluster->tick_en.unlock();
        }
        for (auto cluster: clusters) {
            cluster->tick_done.lock();
        }
    } else {
        for (auto cluster: clusters) {
            cluster->tick();
        }
    }
#else
    for (auto cluster: clusters) {
            cluster->tick();
        }
#endif
}

void SimCore::pauseSim() {
//    printf("Sim received pause click and will %s\n", (isSimRunning() ? "pause" : "continue"));
    simToggle();
    sendSimUpdate();
}

void SimCore::sendSimUpdate() {
    if (!isCompletelyInitialized)
        return;
    visualizeDataUpdate();
    emit simUpdate(clock);
}

void SimCore::runSteps(int steps) {
    goal_clock = clock + steps;
    printf("gonna run %i cycles...", steps);
    printf("from %li to %li\n", clock, goal_clock);
    simResume();
}

void SimCore::dumpLM(int c, int u) {
    this->clusters[c]->dumpLocalMemory(u);
    int cc = 0, cu = 0;
    for (auto cluster : clusters) {
        cu = 0;
        for (auto unit : cluster->getUnits()) {
            if (cc == c && cu == u) {

                uint8_t *memory = unit->getlocalmemory();

                emit sendLocalmemory(memory);
                break;
            }
            cu++;
        }
        cc++;
    }
}


void SimCore::dumpQ(int c, int u){
    this->clusters[c]->dumpQueue(u);
}


void SimCore::dumpRF(int c, int u, int l) {
    this->clusters[c]->dumpRegisterFile(u, l);
    QVector<bool> flag(2);
    int cc = 0, cu = 0, cl = 0;
    for (auto cluster : clusters) {
        cu = 0;
        for (auto unit : cluster->getUnits()) {
            cl = 0;
            for (auto lane : unit->getLanes()) {
                if (cl == l && cc == c && cu == u) {

                    uint8_t *regist = lane->getregister();

                    flag[0] = lane->getzeroflag();
                    flag[1] = lane->getnegativeflag();

                    emit sendRegister(regist, flag);
                    break;
                }
                cl++;
            }
            cu++;
        }
        cc++;
    }

}

void SimCore::quitSim() {
    simPause();
    sim_finished = true;
}

void SimCore::visualizeDataUpdate() {
    if (!isCompletelyInitialized)
        return;

    // update visualization data
    CommandWindow::Data data;
    //data.commands = QVector<std::shared_ptr<CommandVPRO>>;
    //data.command_queue =  QVector<QVector<std::shared_ptr<CommandVPRO>>>;

//    // current commands in lanes;
    for (auto cluster : clusters) {
        for (auto unit: cluster->getUnits()) {
            for (auto lane : unit->getLanes()) {
                data.commands.push_back(std::make_shared<CommandVPRO>(lane->getCmd().get()));
            }
        }
    }

    // commands in unit command queues;
    for (auto cluster : clusters) {
        data.isDMAbusy.push_back(cluster->isWaitDMABusy());
        data.isbusy.push_back(cluster->isWaitBusy());
        for (auto unit: cluster->getUnits()) {
            data.isLooppush.push_back(unit->isLoopPushing());
            data.isLoppparsing.push_back(unit->isLoopParsing());
            data.command_queue.push_back(unit->getCopyOfCommandQueue());
        }
    }

    // gather Stat sum;
    for (auto cluster : clusters) {
        data.summedDMAStat.add(cluster->dma->stat);
    }
    data.summedLanesStat = new LaneStat[vector_lane_count + 1]; // sum all stats from all Lanes
    for (auto cluster : clusters) {
        for (auto unit : cluster->getUnits()) {
            int laneId = 0;
            for (auto lane : unit->getLanes()) {
                data.summedLanesStat[laneId].name = QString("Lane ") + QString::number(lane->vector_lane_id);
                data.summedLanesStat[laneId].add(lane->stat);
                laneId++;
            }
        }
    }
    for (auto cluster : clusters) {
        for (auto unit : cluster->getUnits()) {
            data.summedUnitStat.add(unit->stat);
        }
    }
    data.simStat = simstat;
    data.clock = getClock();

    emit dataUpdate(data);
}

void SimCore::printSimStats(){
    simstat.print(getClock());
}

void SimCore::printSimResults(bool toFile) {

    if (!isCompletelyInitialized)
        return;

    if (debug & DEBUG_INSTR_STATISTICS) {
        printf("# ---------------------------------------------------------------------------------\n");
        printf("# Clock Cycles: %li \n", clock);
        printf("# ---------------------------------------------------------------------------------\n");
        printf("# DMA STATISTICS:\n");
    }

    DMAStat overallDMA; // sum all stats from all DMAs
    for (auto cluster : clusters) {
        if (debug & DEBUG_INSTR_STATISTICS_ALL) {
            printf("#\t  DMA-Stats (Cluster %i): \n", cluster->cluster_id);
            cluster->dma->stat.print("# \t\t");
        }
        overallDMA.add(cluster->dma->stat);
    }

    overallDMA = overallDMA / (float) cluster_count;
    if (debug & DEBUG_INSTR_STATISTICS) {
        printf("#  DMA-Stats (Mean, %i DMAs total):\n", cluster_count);
        overallDMA.print("#\t");
        overallDMA.printAverageUnitUse("#\t");
        printf("# ---------------------------------------------------------------------------------\n");
        printf("# LANE STATISTICS:\n");
    }

    LaneStat overalllane[vector_lane_count + 1]; // sum all stats from all Lanes
    for (auto cluster : clusters) {
        if (debug & DEBUG_INSTR_STATISTICS_ALL)
            printf("# Cluster %i\n", cluster->cluster_id);
        for (auto unit : cluster->getUnits()) {
            if (debug & DEBUG_INSTR_STATISTICS_ALL)
                printf("# \tUnit %i\n", unit->vector_unit_id);
            int laneId = 0;
            for (auto lane : unit->getLanes()) {
                if (debug & DEBUG_INSTR_STATISTICS_ALL) {
                    printf("# \t\tLane %i: ", lane->vector_lane_id);
                    lane->stat.print("#\t\t\t");
                }
                overalllane[laneId].name = QString("Lane ") + QString::number(lane->vector_lane_id);
                overalllane[laneId].add(lane->stat);
                laneId++;
            }
        }
    }

    for (auto &stat : overalllane) {
        stat = stat / (float) (cluster_count * vector_unit_count);
    }

    if (debug & DEBUG_INSTR_STATISTICS) {
        printf("# Lane-Stats (Mean, %i cluster, %i units):\n", cluster_count, vector_unit_count);
        int i = 0;
        for (auto stat : overalllane) {
            stat.print("#\t");
            i++;
        }
        printf("# ---------------------------------------------------------------------------------\n");
        printf("# UNIT STATISTICS:\n");
    }
    UnitStat overallUnit;
    for (auto cluster : clusters) {
        if (debug & DEBUG_INSTR_STATISTICS_ALL)
            printf("# Cluster %i\n", cluster->cluster_id);
        for (auto unit : cluster->getUnits()) {
            if (debug & DEBUG_INSTR_STATISTICS_ALL)
                unit->stat.print("\t");
            overallUnit.add(unit->stat);
        }
    }
    overallUnit = overallUnit / (cluster_count * vector_unit_count);
    if (debug & DEBUG_INSTR_STATISTICS)
        overallUnit.print();

    if (toFile) {
        QFile f(statoutfilenmae);
        if (QFileInfo::exists(statoutfilenmae)) {
            if (!f.open(QIODevice::WriteOnly | QIODevice::Append)) {
                printf_error("Failed to write to statics out file!\n");
                return;
            }
        } else {
            if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
                printf_error("Failed to write to statics out file!\n");
                return;
            }
            QTextStream out(&f);
//            out
//                    << "Application\tcomment\t#Clusters\t#Units\t#Lanes\tCycles\tLane Cycles (mean of all Lanes)\tDMA not NONE Cycles (mean of all DMAs)\t"
//                       "DMA Wait Cycles (all Lanes NONE mean over all units)\tVPRO Wait Cycles (all lanes none mean over all units)\tCycles (0 Lanes Busy)\t"
//                       "Cycles (1 Lanes Busy)\tCycles (2 Lanes Busy)\tCycles (3 Lanes Busy)\tDMA Broadcast mean Unit Count\t";
//
//            for (int i = 0; i < CommandVPRO::enumTypeEnd; i++) {
//                auto eCurrent = (CommandVPRO::TYPE) i;
//                out << "TotalClk[" << CommandVPRO::getType(eCurrent) << "]\t";
//            }
//            for (int i = 0; i < CommandDMA::enumTypeEnd; i++) {
//                auto eCurrent = (CommandDMA::TYPE) i;
//                out << "TotalClk[" << CommandDMA::getType(eCurrent) << "]\t";
//            }
//            out << "\n";
            out << "Application\tcomment\t#Clusters\t#Units\t#Lanes\tCycles\tTotalBusyUnitCycles\tTotalBusyDMACycles\tTotalBusyClusterCycles\tTotalBusyUnitAndDMACycles\n";
        }

        QTextStream out(&f);



        out << ExeName << "\t";
#ifndef STAT_COMMENT
        out << "NO_COMMENT" << "\t";
#else
        out << STAT_COMMENT << "\t";
#endif
        out << cluster_count << "\t";
        out << vector_unit_count << "\t";
        out << vector_lane_count << "\t";

        const auto clock_counts = simstat.getStats();
        out << clock << "\t";
        out << clock_counts.at("TotalBusyUnitCycles") << "\t";
        out << clock_counts.at("TotalBusyDMACycles") << "\t";
        out << clock_counts.at("TotalBusyClusterCycles") << "\t";
        out << clock_counts.at("TotalBusyUnitAndDMACycles") << "\t";

//        std::map<int, double> &busyCount = *(overallUnit.getBusyCount());
//        out << clock
//            << "\t" << overalllane->getCyclesNotNONE()
//            << "\t" << overallDMA.getCyclesNotNONE()
//            << "\t" << overallUnit.getWaitingDMACount()
//            << "\t" << overallUnit.getWaitingVPROCount()
//            << "\t" << busyCount[0]
//            << "\t" << busyCount[1]
//            << "\t" << busyCount[2]
//            << "\t" << busyCount[3]
//            << "\t" << overallDMA.getAverageUnitUse();
//
//        LaneStat allLanes;
//        for (int i = 0; i < vector_lane_count + 1; i++) {
//            allLanes.add(overalllane[i]);
//        }
//        // undo mean calc
//        allLanes = allLanes * (float) (cluster_count * vector_unit_count);
//
//        auto alllanestats = allLanes.gettypeCount();
//        for (int i = 0; i < CommandVPRO::enumTypeEnd; i++) {
//            auto eCurrent = (CommandVPRO::TYPE) i;
//            out << "\t" << (*alllanestats)[eCurrent][1]; // clock ticks
//        }
//
//        // undo mean calc
//        overallDMA = overallDMA * (float) cluster_count;
//        auto alldmastats = overallDMA.gettypeCount();
//        for (int i = 0; i < CommandDMA::enumTypeEnd; i++) {
//            auto eCurrent = (CommandDMA::TYPE) i;
//            out << "\t" << (*alldmastats)[eCurrent][1]; // clock ticks
//        }

        out << "\n";

        f.close();
    }

    if (debug & DEBUG_INSTR_STATISTICS) {
        simstat.print(getClock(), "#\t");
        printf("# ---------------------------------------------------------------------------------\n");
        printf("# Statistic END\n");
        printf("# ---------------------------------------------------------------------------------\n");
    }
}

void SimCore::sim_exit() {
    //printf_info("CALLING Sim exit\n\n");
    isCompletelyInitialized = false;

    QFile file(outputcfg);
    if (!file.open(QIODevice::ReadOnly)) {
        printf_error("Error opening %s: %s", outputcfg.toStdString().c_str(), file.errorString().toStdString().c_str());
    }
    QTextStream in(&file);

    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.startsWith('#') || line.startsWith(';')) // skip commented lines
            continue;
        QString output;
        int offset, size;
        QStringList fields = line.split(" ");
        if (fields.size() < 3) {
            printf_warning("Invalid Line for output; %s", line.toStdString().c_str());
            continue;
        }
        output = fields[0].replace("<EXE>", ExeName);
        offset = fields[1].toInt();
        size = fields[2].toInt();
        //printf_info("\tSaving Data to file: %s (Offset: %d, Bytes: %i)\n", output.toStdString().c_str(), offset, size);

        QFile outputfile(output);
        if (outputfile.open(QFile::WriteOnly | QFile::Truncate)) {
            for (int i = 0; i < size; i += 2) {
                char buffer[2];
                buffer[0] = main_memory[offset + i + 0];  // convert endianess
                buffer[1] = main_memory[offset + i + 1];
                outputfile.write(buffer, 2);
            }
            outputfile.close();
        } else {
            printf_error("Error opening output file!");
            QTextStream(stdout) << outputfile.errorString();
        }
    }

    file.close();

    //printf("#SIM: Calling exitialization Script '%s' ... ", exitscript.toStdString().c_str());
    std::ifstream exit_script(exitscript.toStdString().c_str());
    if (!exit_script) {
        printf_warning("[File '%s' not found!]\n", exitscript.toStdString().c_str());
    } else {
        int status = system(QString("bash ").append(exitscript).toStdString().c_str());
        //printf("[executed, returned %i]\n", status);
    }
    exit_script.close();
}
