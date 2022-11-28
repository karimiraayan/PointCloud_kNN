//
// Created by gesper on 20.11.20.
//

#ifndef CNN_YOLO_LITE_RISC_COMMUNICATION_H
#define CNN_YOLO_LITE_RISC_COMMUNICATION_H

#include <cstddef>

#include "ConsolePrinter.h"
#include <QMutex>
// enum INDICATORS, enum COMMUNICATION_STATES, ...
#include "../../../vpro/cnn_yolo_lite_hw/configuration_generation/RiscHostCommunication.h"
using namespace RiscHostCommunicationSignals;

class RiscCommunication {
public:
    static const bool DEBUG_RISC_HOST_COMMUNICATION = false;

private:
    int communication_risc{};
    int communication_host{};
    int communication_risc_data{};
    int communication_host_data{};
    QMutex &mutex;
    ConsolePrinter &console;

    bool nokilledflag;
public:
    RiscCommunication(QMutex &mutex, ConsolePrinter &console);

    int getCommunicationRisc() const;
    void setCommunicationRisc(int communicationRisc);
    int getCommunicationHost() const;
    void setCommunicationHost(int communicationHost);
    int getCommunicationRISCData() const;
    void setCommunicationRISCData(int communicationData);
    int getCommunicationHOSTData() const;
    void setCommunicationHOSTData(int communicationData);

    [[maybe_unused]] uint32_t getData(uint32_t address);
    [[maybe_unused]] void setData(uint32_t address, uint32_t data);

    [[maybe_unused]] uint32_t getDataFromRisc(bool &killed);
    [[maybe_unused]] uint32_t getDataFromRisc();
    [[maybe_unused]] void sendDataToRisc(uint32_t d, bool &killed);
    [[maybe_unused]] void sendDataToRisc(uint32_t d);


    void send_data_array(uint32_t address, uint32_t size, uint8_t *data);
    void dump_data_array(uint32_t addr, int num_bytes, uint8_t* array_pnt, bool silent = false);
    void dump_data_array_file(uint32_t addr, int num_bytes, char const* file_name);
};


#endif //CNN_YOLO_LITE_RISC_COMMUNICATION_H
