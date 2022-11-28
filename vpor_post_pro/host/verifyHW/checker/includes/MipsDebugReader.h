//
// Created by gesper on 16.12.20.
//

#ifndef CNN_YOLO_LITE_MIPS_DEBUG_READER_H
#define CNN_YOLO_LITE_MIPS_DEBUG_READER_H

#include <QMutex>
#include <QThread>
#include "../../common/vpro_host_aux.h"
#include "ConsolePrinter.h"

class MipsDebugReader : public QThread
{
    Q_OBJECT
public:
    MipsDebugReader(QMutex &mutex, ConsolePrinter &console);

protected:
    void run() override;

private:
    QMutex &mutex;
    ConsolePrinter &console;
};

#endif //CNN_YOLO_LITE_MIPS_DEBUG_READER_H
