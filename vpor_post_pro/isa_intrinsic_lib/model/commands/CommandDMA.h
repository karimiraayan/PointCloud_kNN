// ########################################################
// # VPRO instruction & system simulation library         #
// # Sven Gesper, IMS, Uni Hannover, 2019                 #
// ########################################################
// # a class for commands to the DMA                      #
// ########################################################

#ifndef VPRO_CPP_COMMANDDMA_H
#define VPRO_CPP_COMMANDDMA_H

#include "CommandBase.h"
#include <QList>
#include <QString>

class CommandDMA : public CommandBase {
public:
    CommandDMA();
    CommandDMA(CommandDMA *ref);
    CommandDMA(CommandDMA &ref);

    enum TYPE{
        NONE,
        EXT_1D_TO_LOC_1D,
        EXT_2D_TO_LOC_1D,
        LOC_1D_TO_EXT_2D,
        LOC_1D_TO_EXT_1D,
        WAIT_FINISH,
        enumTypeEnd
    } type;

    uint32_t cluster;
    QList<uint32_t> unit;

    uint64_t ext_base;
    uint32_t loc_base;

    int x, y;

    uint32_t x_size;
    int32_t x_stride;
    uint32_t y_size;

    enum PAD{
        TOP = 0,
        RIGHT = 1,
        BOTTOM = 2,
        LEFT = 3
    };
    bool pad[4];

    bool is_done() override;
    void print(FILE * out = stdout);
    
    // String returning functions
    QString get_type() override{
        switch (type){
            case NONE:
                return QString("NONE             ");
            case EXT_1D_TO_LOC_1D:
                return QString("EXT_1D_TO_LOC_1D ");
            case EXT_2D_TO_LOC_1D:
                return QString("EXT_2D_TO_LOC_1D ");
            case LOC_1D_TO_EXT_2D:
                return QString("LOC_1D_TO_EXT_2D ");
            case LOC_1D_TO_EXT_1D:
                return QString("LOC_1D_TO_EXT_1D ");
            case WAIT_FINISH:
                return QString("WAIT_FINISH      ");
            default:
                return QString("Unknown          ");
        }
    }

    static QString getType(CommandDMA::TYPE t);

    QString get_string() override;

    static void printType(CommandDMA::TYPE t, FILE * out = stdout){
        switch (t){
            case NONE:
                fprintf(out, "NONE             "); break;
            case EXT_1D_TO_LOC_1D:
                fprintf(out, "EXT_1D_TO_LOC_1D "); break;
            case EXT_2D_TO_LOC_1D:
                fprintf(out, "EXT_2D_TO_LOC_1D "); break;
            case LOC_1D_TO_EXT_2D:
                fprintf(out, "LOC_1D_TO_EXT_2D "); break;
            case LOC_1D_TO_EXT_1D:
                fprintf(out, "LOC_1D_TO_EXT_1D "); break;
            case WAIT_FINISH:
                fprintf(out, "WAIT_FINISH      "); break;
            default:
                fprintf(out, "Unknown          ");
        }
    }

    bool done;
};


#endif //VPRO_CPP_COMMANDDMA_H
