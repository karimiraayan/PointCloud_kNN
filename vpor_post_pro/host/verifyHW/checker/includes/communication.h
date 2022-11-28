//
// Created by gesper on 20.11.20.
//

#ifndef CNN_YOLO_LITE_COMMUNICATION_H
#define CNN_YOLO_LITE_COMMUNICATION_H

#include <QThread>
#include <QString>
#include <QVector>
#include "AuxDebugFifoReader.h"
#include "../../../vpro/verifyHW/includes/test_framework.h"
#include <ConsolePrinter.h>

class communication : public QThread {
Q_OBJECT

private:
    AuxDebugFifoReader &fiforeader;

    int &MM_TEST_RESULT;
    int &MM_TEST_DATA_1;
    int &MM_TEST_DATA_2;
    int &MM_START_FLAG;
    int &MM_RUNNING_FLAG;
    int &MM_USE_SINGLE_DMA_TRANSFERS;

    void getUserHexChoice(bool &hex);
    void getUserDMAChoice(bool &dma);

    QMap<TESTS, QList<TESTS>> testmap;

    uint64_t *flag;
    uint16_t *opa;
    uint16_t *opb;

    uint16_t *opa_mips;
    uint16_t *opb_mips;

    bool printHex;
    bool dmaSingleTransfers;

    uint64_t *vpro_flag;

    int num_cl;
    int num_vu_per_cl;

    const QString& executable;
    const QString& input_a;
    const QString& input_b;
    const QString& output;

    bool &silent;
    int &silent_testnr;

    QMutex &mutex;
    ConsolePrinter &console;

    const int MAX_TEST_DURATION = 180; // in seconds

public:
    communication(QMutex &mutex, AuxDebugFifoReader &fiforeader, ConsolePrinter &console, int &MM_TEST_RESULT, int &MM_TEST_DATA_1, int &MM_TEST_DATA_2,
                  int &MM_START_FLAG, int &MM_RUNNING_FLAG, int &MM_USE_SINGLE_DMA_TRANSFERS,
                  const QString& executable, const QString& input_a, const QString& input_b, const QString& output,
                  int num_cl, int num_vu_per_cl, bool &silent, int &silent_testnr, QObject *parent = nullptr);

    bool success;
protected:
    void run() override;

};


#endif //CNN_YOLO_LITE_COMMUNICATION_H
