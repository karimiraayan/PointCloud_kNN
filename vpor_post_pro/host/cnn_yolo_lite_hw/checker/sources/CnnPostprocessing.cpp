//
// Created by gesper on 2/22/21.
//

#include "CnnPostprocessing.h"
#include "ConsolePrinter.h"

#include <chrono>
#include <thread>
#include <QDebug>
#include <CnnPostprocessing.h>

#include <boost/endian/conversion.hpp>
#include <boost/endian/buffers.hpp>  // see Synopsis below

using namespace boost::endian;

struct prediction_t {
    int left{}, top{}, right{}, bottom{};
    double confidence_score{};
    int best_class{};
    bool to_delete = false;
};

double sigmoid(float x) {
    return 1. / (1. + exp(-x));
}

// IoU (Intersection over Union)
float iou(prediction_t &boxA, prediction_t &boxB) {
    // boxA = boxB = [x1,y1,x2,y2]; IoU between two boxes with coordinate [left, top, right, bottom]
    // Determine the coordinates of the intersection rectangle
    int xA = std::max(boxA.left, boxB.left);
    int yA = std::max(boxA.top, boxB.top);
    int xB = std::min(boxA.right, boxB.right);
    int yB = std::min(boxA.bottom, boxB.bottom);

    // Compute the area of intersection
    int intersection_area = (xB - xA + 1) * (yB - yA + 1);

    // Compute the area of both rectangles
    int boxA_area = (boxA.right - boxA.left + 1) * (boxA.bottom - boxA.top + 1);
    int boxB_area = (boxB.right - boxB.left + 1) * (boxB.bottom - boxB.top + 1);

    // Compute the IOU
    return float(intersection_area) / float(boxA_area + boxB_area - intersection_area);
}

void CnnPostprocessing::run() {
    auto t1 = std::chrono::high_resolution_clock::now();
    while (isRunning()) {

        if (isNewCNNResultAvailable()) {
            console.printf_normal(
                    "[Postprocessing] CNN Result completely available. Starting to postprocess & visualize\n");

            cv::Mat results;
            cv::Mat inputs;
            cv::Mat inputs_orig;
            output_result.copyTo(results);
            input_image.copyTo(inputs);
            input_image.copyTo(inputs_orig);
            isNewCNNResultAvailable_flag = false;

            int n_grid_cells = 7; // output size of the last layer (7x7 cells)
            int n_b_boxes = 5;  // 5 object detector for each cell 5x(7x7x25)

            // each detector produces 25 value for each cell
            int n_classes = 20;  // class probabilities of total 20 classes
            int n_b_box_coord = 4;  // for the calculation of bounding box (tx, ty, tw, th)
            int n_b_box_cofidence = 1;  // confidence score of the bounding box

            float score_treshold = 0.5;
            float iou_treshold = 0.3;

            // Names and box colors for each class
            QString classes[] = {"aeroplane", "bicycle", "bird", "boat",
                                 "bottle", "bus", "car", "cat",
                                 "chair", "cow", "table", "dog",
                                 "horse", "motorbike", "person", "pottedplant",
                                 "sheep", "sofa", "train", "tvmonitor"};
            int colors[20][3] = {{254, 254, 254},
                                 {239, 211, 127},
                                 {225, 169, 0},
                                 {211, 127, 254},
                                 {197, 84,  127},
                                 {183, 42,  0},
                                 {169, 0, 254},
                                 {155, -42, 127},
                                 {141, -84, 0},
                                 {127, 254, 254},
                                 {112, 211, 127},
                                 {98,  169, 0},
                                 {84,  127, 254},
                                 {70,  84,  127},
                                 {56,  42,  0},
                                 {42,  0,   254},
                                 {28,  -42, 127},
                                 {14,  -84, 0},
                                 {0,   254, 254},
                                 {-14, 211, 127}};
            // Pre-computed object shapes for 5 detectors (k=5 B-Boxes)
            // anchors =[width_0, height_0, width_1, height_1, .... width_4, height_4]
            float anchors[] = {1.08, 1.19, 3.42, 4.41, 6.63, 11.38, 9.42, 5.11, 16.62, 10.52};

            QList<prediction_t> thresholded_predictions;

            for (int row = 0; row < n_grid_cells; ++row) {
                for (int col = 0; col < n_grid_cells; ++col) {
                    for (int b = 0; b < n_b_boxes; ++b) { // number of bounding box or detector 5
                        float tx = results.at<float>(col, row, (25 * b) + 0);
                        float ty = results.at<float>(col, row, (25 * b) + 1);
                        float tw = results.at<float>(col, row, (25 * b) + 2);
                        float th = results.at<float>(col, row, (25 * b) + 3);
                        float tc = results.at<float>(col, row, (25 * b) + 4);

                        double center_x = (float(col) + sigmoid(tx)) * 32.0;
                        double center_y = (float(row) + sigmoid(ty)) * 32.0;

                        // width and heights of 5 detectors (5 bounding box)
                        double roi_w = exp(tw) * anchors[2 * b + 0] * 32.0;
                        double roi_h = exp(th) * anchors[2 * b + 1] * 32.0;

                        // confidence score of each bounding box (total 5)
                        double final_confidence = sigmoid(tc);

                        // Softmax classification
                        float class_predictions[20]; // 5: last 20 values for classification
                        for (int i = 0; i < 20; ++i) {
                            class_predictions[i] = results.at<float>(col, row, (25 * b) + i + 5);
                        }
                        int best_class = 0;
                        float class_predictions_max = class_predictions[best_class];
                        for (int i = 1; i < 20; ++i) {
                            if (class_predictions[i] > class_predictions_max) {
                                class_predictions_max = class_predictions[i];
                                best_class = i;
                            }
                        }
                        float class_predictions_sum = 0;
                        for (float &class_prediction : class_predictions) {
                            class_prediction = exp(class_prediction - class_predictions_max);
                            class_predictions_sum += class_prediction;
                        }
                        float best_class_score = class_predictions[best_class] / class_predictions_sum;

                        if (final_confidence * best_class_score > score_treshold) {
                            // update thresholded_predictions for further "non_maximal_suppression"
                            prediction_t prediction;
                            // Compute the final coordinates on both axes
                            prediction.left = int(center_x - (roi_w / 2.));
                            prediction.right = int(center_x + (roi_w / 2.));
                            prediction.top = int(center_y - (roi_h / 2.));
                            prediction.bottom = int(center_y + (roi_h / 2.));
                            prediction.confidence_score = final_confidence * best_class_score;
                            prediction.best_class = best_class;
                            thresholded_predictions.append(prediction);
                        }
                    }
                }
            }

            if (!thresholded_predictions.isEmpty()){
                qSort(thresholded_predictions.begin(), thresholded_predictions.end(),
                      [](prediction_t &a, prediction_t &b) { return a.confidence_score > b.confidence_score; });

                // IOU Tresholding
                QList<prediction_t> thresholded_predictions_nms;
                thresholded_predictions_nms.append(thresholded_predictions.front());
                for (auto &prediction : thresholded_predictions) {
                    for (auto &prediction_cmp : thresholded_predictions_nms) {
                        if (&prediction == &prediction_cmp) continue;
                        float iou_value = iou(prediction, prediction_cmp);
                        if (iou_value > iou_treshold) {
                            prediction.to_delete = true;
                        }
                    }
                    if (!prediction.to_delete) {
                        thresholded_predictions_nms.append(prediction);
                    }
                }

                // draw rectangles
                for (auto &prediction : thresholded_predictions_nms) {
                    auto color = cv::Scalar(colors[prediction.best_class][0], colors[prediction.best_class][1],
                                            colors[prediction.best_class][2]);
                    cv::Point pt1(prediction.left, prediction.top);
                    cv::Point pt2(prediction.right, prediction.bottom);
                    cv::rectangle(inputs, pt1, pt2, color*200);

                    cv::putText(inputs, classes[prediction.best_class].toStdString(),
                                cv::Point(prediction.left, prediction.top),
                                cv::FONT_HERSHEY_COMPLEX_SMALL, 1, color*200, 3);
                }
            }

            // concat input original with overlayed one
            cv::Mat win_mat(cv::Size(inputs_orig.rows, inputs_orig.cols * 2), inputs_orig.type());

            inputs_orig.copyTo(win_mat(cv::Rect(  0, 0, inputs_orig.rows, inputs_orig.cols)));
            inputs.copyTo(win_mat(cv::Rect(0, inputs_orig.cols,  inputs_orig.rows, inputs_orig.cols)));

            cv::imshow("YOLO-LITE [result | original]", win_mat);

//            cv::namedWindow("Input Image", cv::WINDOW_AUTOSIZE);
//            cv::imshow("Input Image", inputs);
        }
    }
}

bool CnnPostprocessing::isNewCNNResultAvailable() {
    return isNewCNNResultAvailable_flag;
}

void CnnPostprocessing::setInputFile(uint16_t input_data_array[224*224*3]) {
    console.printf_normal("[Postprocessing] Saving new Input Data\n");

    auto input_data_array_reversed = new uint16_t[224 * 224 * 3];
    for (int i = 0; i < 224 * 224 * 3; ++i) {
        input_data_array_reversed[i] = endian_reverse(input_data_array[i]);
    }
    cv::Mat channelR(224, 224, CV_16UC1, input_data_array_reversed);
    cv::Mat channelG(224, 224, CV_16UC1, input_data_array_reversed + 224 * 224);
    cv::Mat channelB(224, 224, CV_16UC1, input_data_array_reversed + 2 * 224 * 224);
    std::vector<cv::Mat> channels{channelB, channelG, channelR};
    cv::Mat outputMat;
    cv::merge(channels, outputMat);
    outputMat *= 16;    // correct fpf for example image
    outputMat.copyTo(input_image);
    delete[] input_data_array_reversed;

//    cv::namedWindow("input image reveived post", cv::WINDOW_AUTOSIZE);
//    cv::imshow("input image reveived post", input_image);
//    cv::waitKey(0);
    emit doneInputSave();
}

void CnnPostprocessing::arrivalNewCNNResult(int16_t result_data_array[7*7*125]) {
    console.printf_normal("[Postprocessing] Saving new CNN Result\n");

    int height = 7;
    int width = 7;
    int channels = 125;
    int fpf[2] = {5, 9};

    // read result, transfer to float and use fpf
    int sz[] = {height, width, channels};
    output_result = cv::Mat(3, sz, CV_32F);
    for (int i = 0; i < width; ++i) {
        for (int j = 0; j < height; ++j) {
            for (int k = 0; k < channels; ++k) {
                output_result.at<float>(i, j, k) =
                        float(endian_reverse(result_data_array[k * width * height + j * width + i])) / float(1 << fpf[1]);
            }
        }
    }

//    for (int row = 0; row < 4; ++row) {
//        int col = 0;
//        int b = 0;
//        qDebug() << "(Row:"<<row<<") "<< output_result.at<float>(row, col, b) << " == [INT] " << endian_reverse(result_data_array[b * width * height + row * width + col]);
//    }
//
//    for (int col = 0; col < 4; ++col) {
//        int row = 0;
//        int b = 0;
//        qDebug() << "(Col:"<<col<<") "<< output_result.at<float>(row, col, b) << " == [INT] " << endian_reverse(result_data_array[b * width * height + row * width + col]);
//    }
//
//    for (int row = 0; row < 1; ++row) {
//        for (int col = 0; col < 1; ++col) {
//            for (int b = 0; b < 5; ++b) { // number of bounding box or detector 5
//                qDebug() << "(B:"<<b<<") "<<
//                         output_result.at<float>(row, col, (25 * b) + 0) << ", " <<
//                         output_result.at<float>(row, col, (25 * b) + 1) << ", " <<
//                         output_result.at<float>(row, col, (25 * b) + 2) << ", " <<
//                         output_result.at<float>(row, col, (25 * b) + 3) << ", " <<
//                         output_result.at<float>(row, col, (25 * b) + 4);
//            }
//        }
//    }

    emit doneCNNResultProcessed();
    isNewCNNResultAvailable_flag = true;
}
