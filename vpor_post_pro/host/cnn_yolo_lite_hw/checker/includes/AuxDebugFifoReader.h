//
// Created by gesper on 20.11.20.
//

#ifndef CNN_YOLO_LITE_AUX_READER_H
#define CNN_YOLO_LITE_AUX_READER_H

#include <QMutex>
#include <QThread>
#include "../../common/vpro_host_aux.h"
#include "ConsolePrinter.h"

class AuxDebugFifoReader : public QThread
{
    Q_OBJECT
public:
    AuxDebugFifoReader(QMutex &mutex, ConsolePrinter &console);

protected:
    void run() override;

private:
    QMutex &mutex;
    ConsolePrinter &console;
};



#endif //CNN_YOLO_LITE_AUX_READER_H
