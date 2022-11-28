
#ifndef SIM_SETTINGS
#define SIM_SETTINGS

/**
 * whether to copy the file (created by cmake) globals.section to mm start (address = 0)
 */
constexpr bool copyGlobalSectionToMMStart = false;

/**
 * whether to wait for a keypress when the debughelper has printed a printf_error message
 */
constexpr bool PAUSE_ON_ERROR_PRINT = false;
constexpr bool PAUSE_ON_WARNING_PRINT = false;

/**
 * In the pipeline WAR conflict can occur
 * if after a Write command with small vector length (e.g. ADD writes to RF [0]: x_end and y_end build vector lenght 1)
 * a reading command uses the register written before. The Write Command is still inside the ALU pipeline.
 * It would read a wrong data word... [ERROR]
 *
 * - if activated, simulation speed drops by ~30%
 */
constexpr bool checkForWARConflict = false;

/**
 * whether to check if x/y is large enough to fill the whole pipeline
 * if not, following instructions may use wrong input data, cause the rf hasnt got the results yet
 */
constexpr bool checkVectorPipelineLength = false;

/**
 * enables resuming of execution trough gui via button
 */
constexpr bool GUI_WAITKEY = true;

/**
 * defines register size for loop values
 */
constexpr int MAX_LOOP_CASCADES = 2;
constexpr int MAX_LOOP_INSTRUCTIONS = 12;

/**
 * defines size of each units command queue
 */
constexpr int UNITS_COMMAND_QUEUE_MAX_SIZE = 32;

/**
 * print number of simulated cycles once per second (e.g.: "Simulated Cycles in the last 989ms: 102205 [103341 Cycles/s]")
 * influenced massively by simulated architecture size!
 * 1C 1U simulates with >300.000 Cycles/s, -> 8C 1U simulates with ~10.000 Cycles/s
 */
constexpr bool simulationSpeedMeasurement = false;

/**
 * Whether to seperate Cluster, Units or Lanes into threads...
 * more sense on high content tick functions (cluster...)
 *
 * constexpr bool THREAD_CLUSTER = true;   // if #cluster  >= 2 && units   >= 4    (more means more tick time)
 * constexpr bool THREAD_UNIT = false;     // if #units    >= 2 && lanes   >= 3?   (more means more tick time)
 * constexpr bool THREAD_LANE = false;     // if #lanes    >= 2 && tick    per lane complex enough... (maybe with cordic?)
 *
 * no big influence on simulated cycles / s !??? => maybe some bug somewhere? should speed it up...
 */
//#define THREAD_CLUSTER
//#define THREAD_UNIT
//#define THREAD_LANE

/**
 * Log files for CMD history (
 */
constexpr bool CREATE_CMD_HISTORY_FILE = true;
constexpr char CMD_HISTORY_FILE_NAME[] = "../data/sim_cmd_history.log";

class QFile;
extern QFile* CMD_HISTORY_FILE;
class QTextStream;
extern QTextStream *CMD_HISTORY_FILE_STREAM;

#endif
