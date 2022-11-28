//
// Created by gesper on 20.11.20.
//

#include "CnnCommunication.h"
#include <stdexcept>
#include <chrono>

#include "opencv2/opencv.hpp"

#include <QDir>
#include <QProcessEnvironment>
#include <QDataStream>
#include "yolo_configuration.h"

#include <boost/endian/conversion.hpp>
#include <boost/endian/buffers.hpp>  // see Synopsis below

using namespace boost::endian;

//#include <stdio.h>
//#include <iostream>
//#include <sys/ioctl.h> //ioctl() and TIOCGWINSZ
//#include <unistd.h> // for STDOUT_FILENO

static const bool DEBUG_VERIFY_DATA_TRANSFER = false;
static const bool USE_FILE_INPUT_IMAGE = false;

QString type2str(int type) {
    QString r;
    uchar depth = type & CV_MAT_DEPTH_MASK;
    uchar chans = 1 + (type >> CV_CN_SHIFT);
    switch ( depth ) {
        case CV_8U:  r = "8U"; break;
        case CV_8S:  r = "8S"; break;
        case CV_16U: r = "16U"; break;
        case CV_16S: r = "16S"; break;
        case CV_32S: r = "32S"; break;
        case CV_32F: r = "32F"; break;
        case CV_64F: r = "64F"; break;
        default:     r = "User"; break;
    }
    r += "C";
    r += (QString::number(chans));
    return r;
}

CnnCommunication::CnnCommunication(RiscCommunication &risc, ConsolePrinter &console, int MM_RUNNING_FLAG, int MM_DATA_AVAI_FLAG,
                                   int layers_size_bytes, int layers_number_total, int segments_size_bytes, int segments_total_number, int weights_size_total) :
                     risc(risc), console(console), postprocess(console, this), QThread() {
    this->layer_binary_filename = "./data/YOLO_config_layers.bin";
    this->segments_binary_filename = "./data/YOLO_config_segments.bin";
    this->weights_binary_filename = "./data/YOLO_config_weights.bin";
    this->MM_RUNNING_FLAG = MM_RUNNING_FLAG;
    this->MM_DATA_AVAI_FLAG = MM_RUNNING_FLAG + 4;
    this->MM_DCACHE_CLEARED_FLAG = MM_DATA_AVAI_FLAG + 4;
    this->layers_number_total = layers_number_total;
    this->segments_total_number = segments_total_number;

    segment_transfers = new transfer[layers_number_total];
    weight_transfers = new transfer[layers_number_total];
    for (int i = 0; i < layers_number_total; ++i) {
        segment_transfers[i].total_bytes = segments_size_bytes;
        weight_transfers[i].total_bytes = weights_size_total;
    }
    layer_transfers = new transfer[1];
    layer_transfers[0].total_bytes = layers_size_bytes;
    input_transfers = new transfer[input_files.size()];
    isThreadKilled = false;
//    postprocess = CnnPostprocessing(console, this);
    connect(this, SIGNAL(arrivalNewCNNResult(int16_t *)), &postprocess, SLOT(arrivalNewCNNResult(int16_t *)));
    connect(this, SIGNAL(setInputFile(uint16_t *)), &postprocess, SLOT(setInputFile(uint16_t *)));
    connect(&postprocess, SIGNAL(doneCNNResultProcessed()), this, SLOT(doneCNNResultProcessed()));
    connect(&postprocess, SIGNAL(doneInputSave()), this, SLOT(doneInputSave()));
    connect(this, SIGNAL(finished()), &postprocess, SLOT(quit()));
    isInputProcessed = true;
    isCNNResultProcessed = true;
    postprocess.start();
}

void CnnCommunication::createInputData(){
    while (!isInputProcessed and !isThreadKilled){
//        console.printf_warning("Waiting for input to be processed... by postprocessing!\n");
        usleep(100);
    }

    console.printf_info("Creating new Image to be sent to VPRO... (+postprocessing)\n");

    cv::Mat img;
    if (USE_FILE_INPUT_IMAGE){
        QString image_path = "./data/image_in.png";
        img = cv::imread(image_path.toStdString(), cv::IMREAD_COLOR);
//        input_files.append({"./data/input0_revEnd.bin", "./data/input1_revEnd.bin", "./data/input2_revEnd.bin"});
    } else {
        cv::Mat imgtmp;
        cap >> imgtmp;
        int dim = std::min(imgtmp.rows, imgtmp.cols);
        int x = (imgtmp.cols - dim)/2;
        int y = (imgtmp.rows - dim)/2;
        img = imgtmp(cv::Rect(x,y, dim, dim));  // cut max dim from center of cam image
    }
    cv::Mat img_int;
    img.convertTo(img_int, CV_32FC3);
    cv::cvtColor(img_int, img_int, cv::COLOR_BGR2RGB);
    cv::resize(img_int, img_int, cv::Size(224, 224), 0, 0, cv::INTER_CUBIC);
    // scale to range [0; 1]
    double min, max;
    cv::minMaxIdx(img_int, &min, &max);
    img_int -= min;
    cv::minMaxIdx(img_int, &min, &max);
    img_int /= max;
    // scale values to range [0; 2^13]
//    cv::normalize(img_int, img_int, 0, pow(2,12), cv::NORM_MINMAX, CV_16UC3);
    img_int *= (1<<12);
    img_int.convertTo(img_int, CV_16UC3);
//    console.printf_info_(type2str(img_int.type()) + "\n");
//    cv::minMaxIdx(img_int, &min, &max);
//    console.printf_info_("Min: "+QString::number(min) + ", Max: " + QString::number(max) + "\n");

    for (int i = 0; i < 224; ++i) {
        for (int j = 0; j < 224; ++j) {
            for (int k = 0; k < 3; ++k) {
                int16_t cvval = endian_reverse(img_int.at<cv::Vec<uint16_t, 3> >(i, j)[k]);
//                int16_t qtval = input_data[k*224*224+i*224+j];
//                if (cvval != qtval){
////                    console.printf_info("Error %i != %i \n", cvval, qtval);   // TODO: why is this different?!...
//                }
                input_data[k*224*224+i*224+j] = cvval; // endian_reverse(img_int.at<cv::Vec3i>(i, j)[k]);
            }
        }
    }
    console.printf_info("Emitting postprocessing input rdy\n");

    emit setInputFile(&(input_data[0]));
}

void CnnCommunication::ctrl_c_handler(int s){
    if (s == 2){
        console.printf_warning("Caught Ctrl-C [signal %d]\n",s);
        console.printf_warning("\tSystem gonna shut down\n");
        risc.setData(MM_RUNNING_FLAG, 0);
        cv::destroyAllWindows();
        this->isThreadKilled = true;
        this->kill();
        this->quit();
    }
}

void CnnCommunication::run() {
    console.printf_warning("[CNN COMMUNICATION]\tThread started\n");
    risc.sendDataToRisc(1, isThreadKilled); // receive of initial parameters (sizes) done, continue @ MIPS

    // open the default camera, use something different from 0 otherwise;
    // Check VideoCapture documentation.
    if(!cap.open(0)){
        console.printf_error_("OpenCV Video Capture could not be opened!\n");
        throw std::runtime_error("Error: OpenCV Video Capture could not be opened!");
    }

//    // for 3 seconds, show the image from the webcam
//    auto start = std::chrono::steady_clock::now();
//    while (start + std::chrono::seconds{3} > std::chrono::steady_clock::now())
//    {
//        cv::Mat frame;
//        cap >> frame;
//        if( frame.empty() ) break; // end of video stream
//        cv::imshow("[Preview] this is you, smile! :)", frame);
//        if( cv::waitKey(10) == 27 ) break; // stop capturing by pressing ESC
//    }
//    cv::destroyWindow("[Preview] this is you, smile! :)");

    console.printf_normal("##############################\n");
    console.printf_normal("Transfer Addresses are fetched from VPRO\n");
    getTransferAdresses(1, layer_transfers);
    console.printf_normal("Layer [Done]\n");
    getTransferAdresses(layer_count, segment_transfers);
    console.printf_normal("Segments [Done]\n");
    getTransferAdresses(layer_count, weight_transfers);
    console.printf_normal("Weights [Done]\n");

    console.printf_normal("Cache is cleared...\n");
    if (QProcessEnvironment::systemEnvironment().value("UEMU_CONN_MODE", "NONE") == "SIM_IO"){
        console.printf_normal("Gonna wait additional 3s due to sim speed is slow :-)\n");
        sleep(3);
    }
    msleep(10);

    console.printf_normal("Layer List Array Transfer...\n");
    transfer_layer_data();
    console.printf_normal("Segment List Array Transfer...\n");
    transfer_segment_data();
    console.printf_normal("Weights Transfer...\n");
    transfer_weights();
    console.printf_normal("MIPS done receiving MM-Datas\n");
    waitFor(INDICATOR_VPRO_READY);
    console.printf_normal("##############################\n");

    console.printf_normal("Cache is cleared...\n");
    if (QProcessEnvironment::systemEnvironment().value("UEMU_CONN_MODE", "NONE") == "SIM_IO"){
        console.printf_normal("Gonna wait additional 3s due to sim speed is slow :-)\n");
        sleep(3);
    }
    msleep(10);

    console.printf_normal("Transfering First Input Data... \n");
    // first input data
    createInputData();
    transfer_input_data();
    waitFor(INDICATOR_VPRO_INPUT_DATA_RECEIVED);

    console.printf_normal("Checking MM-Data via UEMU Read + Compare by Host\n");
    check_layer_data();
    check_segment_data();
    check_weights_data();
    check_input_data();

    console.printf_normal("Fetching addresses of Result MM-Data \n");
    // address of result array
    uint32_t out_mm_base = risc.getDataFromRisc(isThreadKilled);
    uint32_t out_x = risc.getDataFromRisc(isThreadKilled);
    uint32_t out_y = risc.getDataFromRisc(isThreadKilled);
    uint32_t out_channels = risc.getDataFromRisc(isThreadKilled);

    waitFor(INDICATOR_VPRO_READY);
    console.printf_normal("All Preparation Steps done \n");

    int frame_count = 0;
    int results_fetched = 0;
    while (risc.getDataFromRisc(isThreadKilled) == INDICATOR_VPRO_START && !isThreadKilled) {
        auto start_time = std::chrono::high_resolution_clock::now();
        console.printf_normal("##############################\n");
        console.printf_warning("VPRO started CNN Calculation!\n");

        uint32_t data = risc.getDataFromRisc(isThreadKilled);
        int layer = 0;
        uint32_t cycles_mips_total = 0;
        uint32_t cycles_vpro_total = 0;
        uint32_t data2 = 0;
        while (data != INDICATOR_VPRO_INFERENCE_FINISHED && !isThreadKilled) {
            if (data == INDICATOR_VPRO_READY){
                console.printf_normal("\tVPRO Layer [%i] finished!\n", layer);
                data = risc.getDataFromRisc(isThreadKilled);
                data2 = risc.getDataFromRisc(isThreadKilled);
                console.printf_normal("\tRuntime: @ [VPRO] Total Cycle %i (this Layer took %i [MIPS] Cycles)\n", data, data2);
                cycles_mips_total += data2;
                cycles_vpro_total = data;
                layer++;
                if (layer == 2){
                    if (frame_count > 0){
                        while (!isCNNResultProcessed && !isThreadKilled){
                            sleep(1);
                            console.printf_warning("[Waiting for Postprocessing] Host Postprocessing too slow!");
                        }
                        console.printf_warning("Fetching Result Data!\n");
                        risc.dump_data_array(out_mm_base, out_x * out_y * 2 * out_channels, (uint8_t *)result_data);
                        emit arrivalNewCNNResult(result_data);
                    }
                } else if (layer == 4){
                    console.printf_warning("Creating new Input Data!\n");
                    createInputData();
                } else if (layer == 5){   // after layer 1, there is no dependecy on input data.. [YOLO-LITE] => overwrite it ;-)
                    console.printf_warning("Transfering Input Data!\n");
                    transfer_input_data();
                }
            } else {
                console.printf_normal("\t(VPRO message during exec|awaiting READY|INFERENCE_FINISHED): %i [0x%x]\n", data, data);
            }
            data = risc.getDataFromRisc(isThreadKilled);
        }

        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>( end_time - start_time ).count();

        console.printf_error_(QString("Runtime: ")+QString::number(duration)+"us | FPS: "+QString::number(1000000.0/float(duration), 'G', 4)+"\n");

        console.printf_normal("##############################\n");
        console.printf_info("\tRuntime: %i [VPRO] Cycles / %i [MIPS no stall] Cycles\n", cycles_vpro_total, cycles_mips_total);
        float mips = 100;
//        float tvpro = float(cycles_vpro_total)/cycles_mips_total*mips;
//        console.printf_normal("\tMIPS @100 MHz -> VPRO @%.0f MHz\n", tvpro); // Theoretical VPRO Freq (if this is real MIPS cycle count/no stall)
        float vpro = 1/5*1000; // to get MHz
        float realmips = float(cycles_vpro_total)/(vpro/mips); // calc mips cycles (including stall) by re trans the vpro cycles
        console.printf_info("\tMIPS @100 MHz, VPRO @%.0f MHz -> MIPS stalled %.2f %%\n", vpro, 1-float(cycles_mips_total)/realmips);
        // [%.0f real MIPS cycles] realmips

        frame_count++;

//        transfer_result_data("./data/results/"); // includes: sendDataToRisc(1);

        risc.sendDataToRisc(1); // continue vpro cnn execution loop
    } // vpro rdy loop

/**
 * Quit vpro loop
 */
    console.printf_normal("\t ->VPRO is stopping now!\n\n");
    risc.setData(MM_RUNNING_FLAG, 0);

    for (int i = 5; i > 0; --i) {
        console.printf_warning("\033[FGonna shutdown in %is\n", i);
        // \033[F => move cursor to the beginning of the previous line
        // \033[A =>  move cursor up one line
        sleep(1); // 1s
        if (isThreadKilled) break;
    }
    console.printf_warning("\n");

    cv::destroyAllWindows();
}

void CnnCommunication::transfer_layer_data() {
    QFile infile(layer_binary_filename);
    if (infile.open(QFile::ReadOnly)) {
        QByteArray blob = infile.readAll();
        uint32_t size = infile.size();
        if (size != layer_transfers[0].total_bytes) {
            console.printf_error("[ERROR!] Layers Data size is on HOST %u Bytes, but on MIPS %u Bytes!\n",
                         size, layer_transfers[0].total_bytes);
        }
        infile.close();
        console.printf_normal_("File Size: " + QString::number(size) + "\n");
        for (int i = 0; i < 1; ++i) {
//            while (risc.getData(MM_DCACHE_CLEARED_FLAG) == 0){ } // wait until dcache is cleared
//            console.printf_info("Layer [%s], address: 0x%x [Size: %i]\n", layer_binary_filename.toStdString().c_str(),
//                        yolo_layer_address, yolo_layer_size);
            risc.send_data_array(layer_transfers[i].address, layer_transfers[i].bytes, (uint8_t *) &(blob.data()[layer_transfers[i].offset_file]));
//            console.printf_success("Layer transmission done!\n");
            size -= layer_transfers[i].bytes;
//            risc.sendDataToRisc(1); // done
        }
        if (size > 0)
            console.printf_error("Error, the layer file read by HOST still have some byte which weren't transmitted!\n\n");
    } else {
        console.printf_error("Error opening infile file! \n\n");
        console.printf_error_(infile.errorString());
    }
}
void CnnCommunication::transfer_segment_data() {
    QFile infile(segments_binary_filename);

    QByteArray blob;
    if (infile.open(QFile::ReadOnly)) {
        blob = infile.readAll();
        uint32_t size = infile.size();
        if (size != segments_total_number * segment_transfers[0].total_bytes) {
            console.printf_error("[ERROR!] Segments Data size is on HOST %u Bytes, but on MIPS %u Bytes!\n",
                         size, segments_total_number * segment_transfers[0].total_bytes);
        }
        infile.close();
        console.printf_normal_("File Size: " + QString::number(size) + "\n");
        for (int i = 0; i < layer_count; ++i) {
//            while (risc.getData(MM_DCACHE_CLEARED_FLAG) == 0){ } // wait until dcache is cleared
//            console.printf_info("Segment %i [%s], address: 0x%x [Size: %i]\n", i,
//                        segments_binary_filename.toStdString().c_str(), yolo_segment_address, yolo_segment_size);
            risc.send_data_array(segment_transfers[i].address, segment_transfers[i].bytes, (uint8_t *) &(blob.data()[segment_transfers[i].offset_file]));
//            console.printf_success("Segment transmission done!\n");
            size -= segment_transfers[i].bytes;
//            risc.sendDataToRisc(1); // done
        }
        if (size > 0)
            console.printf_error("Error, the segment file read by HOST still have some byte which weren't transmitted!\n\n");
    } else {
        console.printf_error("Error opening infile file! \n\n");
        console.printf_error_(infile.errorString());
    }
}
void CnnCommunication::transfer_weights(){
    QFile infile(weights_binary_filename);
    if (infile.open(QFile::ReadOnly)) {
        QByteArray blob = infile.readAll();
        uint32_t size = infile.size();
        if (size != weight_transfers[0].total_bytes) {
            console.printf_error("[ERROR!] Weights Data size is on HOST %u Bytes, but on MIPS %u Bytes!\n",
                         size, weight_transfers[0].total_bytes);
        }
        infile.close();
        console.printf_normal_("File Size: " + QString::number(size) + "\n");
        for (int i = 0; i < layer_count; ++i) {
//            while (risc.getData(MM_DCACHE_CLEARED_FLAG) == 0){ } // wait until dcache is cleared
//            console.printf_info("Layer/Weights [%s], address: 0x%x [Size: %i]\n", weights_binary_filename.toStdString().c_str(),
//                        layer_address, layer_size);
            risc.send_data_array(weight_transfers[i].address, weight_transfers[i].bytes, (uint8_t *) &(blob.data()[weight_transfers[i].offset_file]));
//            console.printf_success("Weights transmission done!\n");
            size -= weight_transfers[i].bytes;
//            risc.sendDataToRisc(1); // done
        }
        if (size > 0)
            console.printf_error("Error, the weights file read by HOST still have some byte which weren't transmitted!\n\n");
    } else {
        console.printf_error("Error opening infile file! \n\n");
        console.printf_error_(infile.errorString());
    }
}
void CnnCommunication::transfer_input_data() {
    console.printf_info("Transferring input data...\n");
    uint32_t offset = 0;
    for (int i = 0; i < 3; ++i){
        input_transfers[i].offset_file = 0;
        input_transfers[i].bytes = 224*224*2;
        input_transfers[i].total_bytes += 224*224*2;
        input_transfers[i].address = input_mm_base + offset;
        risc.send_data_array(input_transfers[i].address, input_transfers[i].bytes, (uint8_t *) &(input_data[0]));
        offset += 224*224*2;
    }
    console.printf_info("Transferring input data... done!\n");
}
void CnnCommunication::transfer_result_data(QString path) {
    for (int layer = 0; layer < layer_count; ++layer) {
        uint32_t out_mm_base = risc.getDataFromRisc(isThreadKilled);
        uint32_t out_mm_stride = risc.getDataFromRisc(isThreadKilled);
        uint32_t out_x = risc.getDataFromRisc(isThreadKilled);
        uint32_t out_y = risc.getDataFromRisc(isThreadKilled);
        uint32_t out_channels = risc.getDataFromRisc(isThreadKilled);

        console.printf_info("Storing Result Layer %i [MM Base: %x (stride %i), Dim %i x %i, Channels: %i]\n",
                            layer, out_mm_base, out_mm_stride, out_x, out_y, out_channels);
        QDir dir(path+"layer_" + QString::number(layer) + "/");
        if (!dir.exists()) {
            dir.mkpath(".");
        }
        for (int c = 0; c < out_channels; ++c) {
            QString filename =
                    path+"layer_" + QString::number(layer) + "/channel_" + QString::number(c) +
                    ".bin";
            risc.dump_data_array_file(out_mm_base + c * (out_x+out_mm_stride) * (out_y + out_mm_stride) * 2,
                                      (out_x + out_mm_stride) * (out_y + out_mm_stride) * 2, filename.toStdString().c_str());
            usleep(500);
        }
        if (out_mm_stride != 0) {
            console.printf_error("[ERROR] file out created but stride (%i) != 0! @Layer %i\n\n", out_mm_stride,
                                 layer);
        }
    }
    uint32_t out_mm_base = risc.getDataFromRisc(isThreadKilled);
    uint32_t out_mm_stride = risc.getDataFromRisc(isThreadKilled);
    uint32_t out_x = risc.getDataFromRisc(isThreadKilled);
    uint32_t out_y = risc.getDataFromRisc(isThreadKilled);
    uint32_t out_channels = risc.getDataFromRisc(isThreadKilled);
    for (int layer = 0; layer < out_channels; ++layer) {
        QDir dir(path+"input/");
        if (!dir.exists()) {
            dir.mkpath(".");
        }
        QString filename = path+"input/channel_" + QString::number(layer) + ".bin";
        risc.dump_data_array_file(out_mm_base + (out_x+out_mm_stride) * out_y * 2 * layer, (out_x+out_mm_stride) * out_y * 2, filename.toStdString().c_str());
        usleep(200);
    }
    usleep(10000);
    risc.sendDataToRisc(1, isThreadKilled); // clear flag vpro_rdy
}

void CnnCommunication::check_layer_data(){
    QFile infile(layer_binary_filename);
    if (infile.open(QFile::ReadOnly)) {
        QByteArray blob = infile.readAll();
        uint32_t size = infile.size();
        infile.close();
        console.printf_normal_("Verification on " + layer_binary_filename + ". File Size: "  + QString::number(size) + ". Checking "+QString::number(layer_transfers[0].bytes)+" Bytes\n");
        for (int i = 0; i < 1; ++i) {
            verify(blob, layer_transfers[i], i);
        }
    } else {
        console.printf_error("[ERROR] check on layer data failed. Error opening file!\n\n");
    }
}
void CnnCommunication::check_segment_data(){
    QFile infile(segments_binary_filename);
    if (infile.open(QFile::ReadOnly)) {
        QByteArray blob = infile.readAll();
        uint32_t size = infile.size();
        infile.close();
        int bytes = 0;
        for (int i = 0; i < layer_count; ++i){
            bytes += segment_transfers[i].bytes;
        }
        console.printf_normal_("Verification on " + segments_binary_filename + ". File Size: "  + QString::number(size) + ". Checking "+QString::number(bytes)+" Bytes\n");
        for (int i = 0; i < layer_count; ++i) {
            verify(blob, segment_transfers[i], i);
        }
    } else {
        console.printf_error("[ERROR] check on segment data failed. Error opening file!\n\n");
    }
}
void CnnCommunication::check_weights_data(){
    QFile infile(weights_binary_filename);
    if (infile.open(QFile::ReadOnly)) {
        QByteArray blob = infile.readAll();
        uint32_t size = infile.size();
        infile.close();
        int bytes = 0;
        for (int i = 0; i < layer_count; ++i){
            bytes += weight_transfers[i].bytes;
        }
        console.printf_normal_("Verification on " + weights_binary_filename + ". File Size: "  + QString::number(size) + ". Checking "+QString::number(bytes)+" Bytes\n");
        for (int i = 0; i < layer_count; ++i) {
            verify(blob, weight_transfers[i], i);
        }
    } else {
        console.printf_error("[ERROR] check on weights data failed. Error opening file!\n\n");
    }
}
void CnnCommunication::check_input_data(){
    console.printf_normal_("Verification on Input Data [3]. Checking "+QString::number(input_transfers[0].bytes)+" Bytes\n");
    for (int i = 0; i < 3; ++i) {
        QByteArray blob;
        blob.setRawData((char *)(&(input_data[0])), 224*224*2);
        verify(blob, input_transfers[i], i);
    }
}

void CnnCommunication::verify(QByteArray &blob, transfer &trans, int lay){
    // check data correctness
    auto vpro_data = new uint8_t[trans.bytes];
    risc.dump_data_array(trans.address, trans.bytes, vpro_data, !DEBUG_VERIFY_DATA_TRANSFER);
    int errors = 0;
    for (int verify_i = 0; verify_i < trans.bytes; ++verify_i) {
        if (vpro_data[verify_i] != (uint8_t) blob.at( trans.offset_file + verify_i)) {
            console.printf_error("[ERROR] [%i] Verify of byte index %i/0x%x: Data is not equal! (VPRO) %i != (Host) %i\n",
                         lay, verify_i, verify_i, vpro_data[verify_i], (uint8_t) blob.at(trans.offset_file + verify_i));
            errors++;
        }
        if (errors >= 100) {
            console.printf_error("[ERROR] more than 100 Errors! (skipping check of remaining...)\n");
            break;
        }
    }
}
void CnnCommunication::waitFor(uint32_t flag) {
//    console.printf_normal("\twaiting for MIPS Flag: %i\n", flag);
    while (!isThreadKilled) {
        int tmp = risc.getDataFromRisc(isThreadKilled);
        if (tmp == flag) {
            break;
        }
        console.printf_normal("\t(VPRO feedback): %i [0x%x], during wait for %i\n", tmp, tmp, flag);
//        usleep(5000);
    }
}

void CnnCommunication::getTransferAdresses(int count, transfer *trans){
    int offset = 0;
    for (int i = 0; i < count; ++i) {
        trans[i].address = risc.getDataFromRisc(isThreadKilled);
        trans[i].bytes = risc.getDataFromRisc(isThreadKilled);
        trans[i].offset_file = offset;
        offset += trans[i].bytes;
    }
    waitFor(INDICATOR_VPRO_READY);
}
