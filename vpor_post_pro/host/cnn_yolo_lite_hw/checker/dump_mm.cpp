   
// UEMU libraries
#include <dma_common.h>
#include <mcpa.h>

#include <unistd.h>
#include <cstddef>
#include <QDir>
#include <QDebug>
#include <QThread>
#include <QString>
#include <QVector>
#include "../../common/vpro_host_aux.h"
#include "../../../isa_intrinsic_lib/simulator/helper/debugHelper.h"

int main(int argc, char **argv) {

    // Disable buffering on std out
    setbuf(stdout, nullptr);

    printf_info("UEMU init... \n");
    init_connection_via_cfg(0);
//    printf_info("set reset... \n");
//    set_reset();

    printf_info("UEMU init... done \n");
    uint32_t out_mm_base[1];
    uint32_t out_mm_stride[1];
    uint32_t out_x[1];
    uint32_t out_y[1];
    uint32_t out_channels[1];
    out_mm_base[0] = 1342277632;
    out_mm_stride[0] = 0;
    out_x[0] = 224;
    out_y[0] = 224;
    out_channels[0] = 1;

int layer_count = 1;
    // TODO stride
    for (int layer = 0; layer < layer_count; ++layer) {
        printf_info("Dumping Layer %i \n", layer);
        QDir dir("./data/result/layer_" + QString::number(layer) + "/");
        if (!dir.exists()) {
            dir.mkpath(".");
        }
        for (int c = 0; c < out_channels[layer]; ++c) {
            QString filename =
                    "./data/result/layer_" + QString::number(layer) + "/channel_" + QString::number(c) +
                    ".bin";
            bin_file_dump(out_mm_base[layer] + c * out_x[layer] * out_y[layer] * 2,
                          out_x[layer] * out_y[layer] * 2, filename.toStdString().c_str());
            usleep(200);
        }
        if (out_mm_stride[layer] != 0) {
            printf_error("[ERROR] file out created but stride (%i) != 0! @Layer %i\n", out_mm_stride,
                         layer);
        }
    }
    for (int layer = 0; layer < 3; ++layer) {
        QDir dir("./data/result/input/");
        if (!dir.exists()) {
            dir.mkpath(".");
        }
        QString filename = "./data/result/input/channel_" + QString::number(layer) + ".bin";
        bin_file_dump(0x50000000 + 224 * 224 * 2 * layer, 224 * 224 * 2, filename.toStdString().c_str());
        usleep(200);
    }
    
    return 0;
}
