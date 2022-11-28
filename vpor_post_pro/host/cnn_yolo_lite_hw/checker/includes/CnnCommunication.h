//
// Created by gesper on 20.11.20.
//

#ifndef CNN_YOLO_LITE_CNN_COMMUNICATION_H
#define CNN_YOLO_LITE_CNN_COMMUNICATION_H

#include <QThread>
#include <QString>
#include <QVector>
#include "RiscCommunication.h"
#include "ConsolePrinter.h"
#include "CnnPostprocessing.h"

class CnnCommunication : public QThread
{
    Q_OBJECT

public:
    void ctrl_c_handler(int s);

private:
    QString layer_binary_filename;
    QString segments_binary_filename;
    QString weights_binary_filename;

    QVector<QString> input_files;

    int MM_RUNNING_FLAG;
    int MM_DATA_AVAI_FLAG;
    int MM_DCACHE_CLEARED_FLAG;

    int layers_number_total;
    int segments_total_number;

    struct transfer{
        explicit transfer(uint32_t total_bytes = 0) :
            address(0), bytes(0), offset_file(0), total_bytes(total_bytes)
        {

        };
        uint32_t address;
        uint32_t bytes;
        uint32_t offset_file;
        uint32_t total_bytes;
    };
    transfer *layer_transfers;
    transfer *segment_transfers;
    transfer *weight_transfers;
    transfer *input_transfers;

    RiscCommunication &risc;
    ConsolePrinter &console;

    void transfer_layer_data();
    void transfer_segment_data();
    void transfer_input_data();

    void getTransferAdresses(int count, transfer *trans);

    void verify(QByteArray &blob, transfer &trans, int i = 0);
    void check_layer_data();
    void check_segment_data();
    void check_weights_data();
    void check_input_data();

    void transfer_result_data(QString path = "./data/result/");

    void waitFor(uint32_t flag);

    CnnPostprocessing postprocess;
    bool isInputProcessed;
    bool isCNNResultProcessed;

    uint16_t input_data[224*224*3];
    int16_t result_data[7*7*125];

    cv::VideoCapture cap;
    bool isThreadKilled;
public:
    CnnCommunication(RiscCommunication &risc, ConsolePrinter &console, int MM_RUNNING_FLAG, int MM_DATA_AVAI_FLAG,
                     int layers_size_bytes, int layers_number_total, int segments_size_bytes, int segments_total_number, int weights_size_total);

public slots:
    void doneCNNResultProcessed(){
        isCNNResultProcessed = true;
    }
    void doneInputSave(){
        isInputProcessed = true;
    }
    void kill(){
        isThreadKilled = true;
    }

signals:
    void arrivalNewCNNResult(int16_t *result_data_array);
    void setInputFile(uint16_t *input_data_array);

protected:

    void run() override;

    void createInputData();
    void transfer_weights();
};


#endif //CNN_YOLO_LITE_CNN_COMMUNICATION_H
