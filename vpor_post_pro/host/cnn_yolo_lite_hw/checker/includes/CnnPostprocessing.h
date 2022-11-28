//
// Created by gesper on 2/22/21.
//

#ifndef CNN_YOLO_LITE_CNNPOSTPROCESSING_H
#define CNN_YOLO_LITE_CNNPOSTPROCESSING_H

#include <QThread>
#include <QString>
#include "ConsolePrinter.h"
#include "opencv2/opencv.hpp"

class CnnPostprocessing : public QThread {
    Q_OBJECT
public:
    CnnPostprocessing(ConsolePrinter &console, QThread *parent = nullptr) : console(console), QThread(parent) {
        isNewCNNResultAvailable_flag = false;
    };

protected:
    void run() override;

public slots:
    void arrivalNewCNNResult(int16_t *result_data_array);
    void setInputFile(uint16_t *input_data_array);

signals:
    void doneCNNResultProcessed();
    void doneInputSave();

private:
    bool isNewCNNResultAvailable_flag;
    bool isNewCNNResultAvailable();

    cv::Mat input_image, output_result;

    ConsolePrinter &console;
};


#endif //CNN_YOLO_LITE_CNNPOSTPROCESSING_H
