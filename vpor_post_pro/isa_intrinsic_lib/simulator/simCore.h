// ########################################################
// # VPRO instruction & system simulation library         #
// # Sven Gesper, IMS, Uni Hannover, 2019                 #
// ########################################################
// # Main entry for simulation.                           #
// # -> Init                                              #
// # -> call commands for vpro                            #
// # -> stop                                              #
// ########################################################


#ifndef sim_core_class
#define sim_core_class

// C std libraries
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <inttypes.h>
#include <unistd.h>
//#include <vector>

#include <QThread>
#include <QMutex>
#include <QTimer>
#include <QTime>
#include <QDebug>
#include <QCoreApplication>

#include "../model/core.h"
#include "../model/commands/CommandVPRO.h"
#include "../model/commands/CommandDMA.h"
#include "../model/commands/CommandBase.h"
#include "../model/commands/CommandSim.h"
#include "../model/architecture/Cluster.h"
#include "../model/architecture/stats/SimStat.h"
#include "windows/Commands/commandwindow.h"


class SimCore : public QThread, Core {

    Q_OBJECT // to be able to receive signals from the windows (qt based widgets) & to send signals to them (update of visualized data)

public:
    SimCore();
    // *** General simulator environment control ***
    int sim_init(int (*main_fkt)(int, char**), int &argc, char* argv[]) override;
    int sim_init(int (*main_fkt)(int, char**), int &argc, char* argv[], uint32_t main_memory_size, uint32_t local_memory_size, uint32_t register_file_size, int cluster_count, int vector_unit_count, int vector_lane_count, int pipeline_depth);

    // constants from main.c (#defines are deprecated) -> given in sim_init
    uint32_t main_memory_size, local_memory_size, register_file_size;
    int cluster_count, vector_unit_count, vector_lane_count, pipeline_depth;

    /**
     * VPRO VectorUnits/Lanes / SIMULATOR functions:
     */
    void __load_shift_left(uint32_t id, uint32_t lm_immediate, uint32_t offset, uint32_t alpha, uint32_t beta, uint32_t shift_factor, uint32_t x_end, uint32_t y_end);
    void __load_shift_right(uint32_t id, uint32_t lm_immediate, uint32_t offset, uint32_t alpha, uint32_t beta, uint32_t shift_factor, uint32_t x_end, uint32_t y_end);
    void __load_reverse(uint32_t lm_immediate, uint32_t offset, int32_t alpha, int32_t beta, uint32_t x_end, uint32_t y_end);
    void __store_reverse(uint32_t lm_immediate, uint32_t offset, int32_t alpha, int32_t beta, uint32_t x_end, uint32_t y_end, bool delayed_chain = false);

    void run_vpro_instruction(std::shared_ptr<CommandVPRO> command);
    void run_dma_instruction(std::shared_ptr<CommandDMA> command);
    void __builtin_vpro_instruction_word(uint32_t id, uint32_t blocking, uint32_t is_chain, uint32_t fu_sel, uint32_t func, uint32_t flag_update, uint32_t dst, uint32_t src1_sel, uint32_t src1, uint32_t src2_sel, uint32_t src2, uint32_t x_end, uint32_t y_end) override;
    void vpro_wait_busy(uint32_t cluster, uint32_t unit_mask) override;
    void vpro_set_idmask(uint32_t idmask) override{vpro_set_unit_mask(idmask);}
    void vpro_set_unit_mask(uint32_t idmask) override;
    uint32_t get_unit_mask(){ return unit_mask_global;}

    void vpro_set_cluster_mask(uint32_t idmask) override;

    void vpro_pipeline_wait() override;

    void vpro_loop_start(uint32_t start, uint32_t end, uint32_t increment_step) override;
    void vpro_loop_end() override;
    void vpro_loop_mask(uint32_t src1_mask, uint32_t src2_mask, uint32_t dst_mask) override;

    void vpro_mac_h_bit_shift(uint32_t shift) override;
    void vpro_mul_h_bit_shift(uint32_t shift) override;

    // *** VPRO DMA *** //
    void dma_ext1D_to_loc1D(uint32_t cluster, uint64_t ext_base, uint32_t loc_base, uint32_t word_count, uint32_t x_stride = 0, uint32_t x_size = 0, uint32_t y_size = 0, bool is_broadcast_no_checking = false) override;

    /**
     * dies ist auch eine allg. beschr.
     * @param cluster dies ist ein vector cluster
     * @param ext_base
     * @param loc_base
     * @param x_stride
     * @param x_size
     * @param y_size
     */
    void dma_ext2D_to_loc1D(uint32_t cluster, uint64_t ext_base, uint32_t loc_base, int32_t x_stride, uint32_t x_size, uint32_t y_size, bool pad_flags[4] = default_flags, bool is_broadcast_no_checking = false) override;
    void dma_loc1D_to_ext1D(uint32_t cluster, uint64_t ext_base, uint32_t loc_base, uint32_t word_count) override;
    void dma_loc1D_to_ext2D(uint32_t cluster, uint64_t ext_base, uint32_t loc_base, int32_t x_stride, uint32_t x_size, uint32_t y_size) override;
    void dma_wait_to_finish(uint32_t cluster) override;

    void dma_set_pad_widths(uint32_t top, uint32_t right, uint32_t bottom, uint32_t left) override;
    void dma_set_pad_value(int16_t value) override;

    // *** System *** //
    void aux_print_debugfifo(uint32_t data) override;
    //uint32_t aux_dev_null(uint32_t data) override;
    void aux_dev_null(uint32_t data) override;
    void aux_flush_dcache() override;

    void aux_clr_sys_time() override;
    void aux_clr_cycle_cnt() override;
    uint32_t aux_get_cycle_cnt() override;
    void aux_send_system_runtime() override;
    uint32_t aux_get_sys_time_lo() override;
    uint32_t aux_get_sys_time_hi() override;
    int aux_cycle_counter, aux_sys_time;

    /**
     * NOT in  VPRO architecture implemented functions / SIMULATOR functions:
     */
    // *** Main Memory *** //
    int bin_file_send(uint32_t addr, int num_bytes, char const* file_name) override;

    int bin_file_dump(uint32_t addr, int num_bytes, char const* file_name) override;
    //int SimCore::bin_file_dump(uint32_t addr, int num_bytes, char const* file_name)

    uint8_t* bin_file_return(uint32_t addr, int num_bytes) override;
    uint8_t *getMainMemory(){return main_memory;}
    uint16_t *get_main_memory(uint32_t addr, unsigned int num_bytes);

    uint8_t* bin_lm_return(uint32_t cluster, uint32_t unit) override;
    uint8_t* bin_rf_return(uint32_t cluster, uint32_t unit, uint32_t lane) override;

    void aux_memset(uint32_t base_addr, uint8_t value, uint32_t num_bytes) override;

    // *** Simulator debugging funtions ***
    void sim_dump_local_memory(uint32_t cluster, uint32_t unit) override;
    void sim_dump_queue(uint32_t cluster, uint32_t unit);
    void sim_dump_register_file(uint32_t cluster, uint32_t unit, uint32_t lane) override;
    void sim_wait_step(bool finish = false, char const* msg = "") override;

    void sim_printf(const char *format){
        printf("#SIM_PRINTF: ");
        printf("%s", format);
        printf("\n");
    }

    template<typename... Args> void sim_printf(const char *format, Args... args){
        printf("#SIM_PRINTF: ");
        printf(format, args...);
        printf("\n");
    };
    // to check whether simulation only program error message has already been printed
    bool SIM_ONLY_WARNING_PRINTED_ONCE_XEND_YEND;
    bool SIM_ONLY_WARNING_PRINTED_ONCE_SHIFT_LL;
    void check_vpro_instruction_length(std::shared_ptr<CommandVPRO> command);

    std::vector<Cluster *> &getClusters(){
        return clusters;
    };
    const long& getClock() const {
        return clock;
    }
    SimStat& getSimStats(){
        return simstat;
    }

    // when closing application, interpret exit script/config ...
    void sim_exit();

signals:
    // indicates end of sim
    void simFinish();
    // transfers all commands (sim and vpro queue) to gui
    void dataUpdate(CommandWindow::Data);
    // basic data about arch
    void simInit(int cluster_cnt, int unit_cnt, int lane_cnt);
    // current clock cycle and issued instructions
    void simUpdate(long clock);

    // status of sim pause/running
    void simIsPaused();
    void simIsResumed();
    // give pointer to qt
    void sendmainmemory(uint8_t*);
    void sendRegister(uint8_t*,QVector <bool>);
    void sendLocalmemory(uint8_t*);
    void sendGuiwaitkeypress(bool*);
    void runMain();

    void performanceMeasurement_stop();
    void performanceMeasurement_start(int);

public slots:
    //after sim. print all usages, cycles,...
    void printSimResults(bool toFile = false);
    // print the results from simStat
    void printSimStats();

    // pause or resume simulation loop
    void pauseSim();
    // exit signal
    void quitSim();
    // run a specified amount of cycles. then pause
    void runSteps(int steps);
    /**
     * basis function of simulation
     * start work on command queue until empty
     * after finish emits simFinish to print SimResults.
     * is called in separate thread...
     */
    void run() override;
    void runUntilReadyForCmd();
    void runUntilMIPSReadyForCmd();

    // creates a copy of all cmds in list and emits the update to visualization.
    void visualizeDataUpdate();
    // dumps LM and RF
    void dumpLM(int c, int u);
    void dumpRF(int c, int u, int l);
    void dumpQ(int c, int u);

    // send current cycle/commands in lanes to gui
    void sendSimUpdate();

    void sim_stop();

    void performanceMeasureTimeout();
private:
    bool windowThread;
    bool isCompletelyInitialized;
    bool isPrintResultDone;

    // flag if gui should be opened
    bool windowless;
    // flag if simulation is running/paused
    bool sim_running;
    // flag to show finish/quit of sim
    bool sim_finished;

    // gui
    CommandWindow *w;
    QCoreApplication *a; // QScopedPointer<>

    // calls updates for all architecture parts / clock
    void clk_tick();

    // name of output configuration file
    QString outputcfg = "../exit/output.cfg";
    QString inputcfg = "../init/input.cfg";
    QString initscript = "../init/init.sh";
    QString exitscript = "../exit/exit.sh";
    QString statoutfilenmae = "../data/statistic_out.csv";

    SimStat simstat;

    // name of executable (to be exchanged in output files "<EXE>" string) (e.g. if output.cfg contains <EXE>.bin -> sim.bin)
    QString ExeName;
    // external symbols (global const variables)
    // address, (size, name -- only one entry in this map)
    QMap<uint64_t , QMap<int, QString>> external_variables;

    // global clock
    long clock;
    // timer for clock cycles stepping (ui)
    long goal_clock;

    // internal functions to control the simulation process, called from gui (e.g.)
    void simPause(){
        if (simulationSpeedMeasurement) {
            if (performanceMeasurement->isActive())
                    emit performanceMeasurement_stop();
            performanceMeasureTimeout();
        }
        sim_running = false;
        emit simIsPaused();
    }
    void simResume(){
        if (simulationSpeedMeasurement){
            emit performanceMeasurement_start(1000);
            performanceMeasurementStart = QTime::currentTime();
        }
        sim_running = true;
        emit simIsResumed();
    }
    void simToggle(){
        if (sim_running)
            simPause();
        else
            simResume();
    }
    bool isSimRunning(){
        return sim_running;
    }

    // architecture. top level are clusters
    std::vector<Cluster *> clusters;

    // global register
    uint32_t cluster_mask_global;
    uint32_t unit_mask_global;

    uint32_t loop_mask_src1;
    uint32_t loop_mask_src2;
    uint32_t loop_mask_dst;

    /**
     * The Result after Multiply is inside a Accu Register (48-bit, FPGA).
     * The Result to write into RF is 24-bit.
     * So different regions of accu are taken. (High or Low region)
     * high region is defined here (24+HIGH_BIT_SHIFT downto HIGH_BIT_SHIFT)
     * 18 due to restriction of OPB to 18-bit on DSP slice on FPGA...
     */
    uint32_t ACCU_MAC_HIGH_BIT_SHIFT;
    uint32_t ACCU_MUL_HIGH_BIT_SHIFT;

    uint32_t dma_pad_top;
    uint32_t dma_pad_right;
    uint32_t dma_pad_bottom;
    uint32_t dma_pad_left;

    uint32_t dma_pad_value;

    // main memory (referenced from all clusters dma's)
    uint8_t* main_memory;

    QTime performanceMeasurementStart;
    QTimer *performanceMeasurement;
    uint64_t performance_clock_last_second;

};


#endif
