//
// Created by gesper on 20.11.20.
//

#include "RiscCommunication.h"
#include "../../common/vpro_host_aux.h"

#include <boost/endian/conversion.hpp>
#include <boost/static_assert.hpp>
using namespace boost::endian;

RiscCommunication::RiscCommunication(QMutex &mutex, ConsolePrinter &console) : mutex(mutex), console(console) {
    nokilledflag = false;
}

[[maybe_unused]] uint32_t RiscCommunication::getData(uint32_t address) {
    auto *vpro_flag = new uint32_t;
    mutex.lock();
    bin_array_dump(address, 4, (uint8_t *) vpro_flag);
    mutex.unlock();
//    usleep(100);
    return boost::endian::endian_reverse(*vpro_flag);
}

[[maybe_unused]] void RiscCommunication::setData(uint32_t address, uint32_t data) {
    uint32_t vpro_flag = boost::endian::endian_reverse(uint32_t(data));
    mutex.lock();
    bin_array_send(address, 4, (uint8_t *) &vpro_flag);
    mutex.unlock();
//    usleep(100);
}

[[maybe_unused]] uint32_t RiscCommunication::getDataFromRisc() {
    return getDataFromRisc(nokilledflag);
}
[[maybe_unused]] uint32_t RiscCommunication::getDataFromRisc(bool &killed) {
    if (DEBUG_RISC_HOST_COMMUNICATION)
        printf("[Get Data from Risc] -> start, RISC: %i, HOST: %i\n", getData(communication_risc),
               getData(communication_host));
    setData(communication_host, READ_RISC);
    if (DEBUG_RISC_HOST_COMMUNICATION)
        printf("[Get Data from Risc] ---> read risc set, RISC: %i, HOST: %i\n", getData(communication_risc),
               getData(communication_host));
    while (getData(communication_risc) != READ_RISC && !killed) {
        // wait for RISC to send
    }
    if (DEBUG_RISC_HOST_COMMUNICATION)
        printf("[Get Data from Risc] -----> risc set to read risc, RISC: %i, HOST: %i\n", getData(communication_risc),
               getData(communication_host));
    int32_t d = getData(communication_risc_data);

    if (DEBUG_RISC_HOST_COMMUNICATION)
        printf("[Get Data from Risc] -------> got data: 0x%x\n", d);
    setData(communication_host, READ_RISC_ACK);
    if (DEBUG_RISC_HOST_COMMUNICATION)
        printf("[Get Data from Risc] ---------> setting READ_RISC_ACK, RISC: %i, HOST: %i\n", getData(communication_risc),
               getData(communication_host));
    while (getData(communication_risc) == READ_RISC && !killed) {
        // wait for RISC
    }
    if (DEBUG_RISC_HOST_COMMUNICATION)
        printf("[Get Data from Risc] -----------> risc acked, RISC: %i, HOST: %i\n", getData(communication_risc),
               getData(communication_host));
    setData(communication_host, IDLE);
    if (DEBUG_RISC_HOST_COMMUNICATION)
        printf("[Get Data from Risc] -------------> return, RISC: %i, HOST: %i\n", getData(communication_risc),
               getData(communication_host));
    return d;
}

[[maybe_unused]] void RiscCommunication::sendDataToRisc(uint32_t d) {
    sendDataToRisc(d, nokilledflag);
}
[[maybe_unused]] void RiscCommunication::sendDataToRisc(uint32_t d, bool &killed) {
    if (DEBUG_RISC_HOST_COMMUNICATION)
        printf("[Send Data to Risc] -> start, RISC: %i, HOST: %i\n", getData(communication_risc),
               getData(communication_host));
    while (getData(communication_risc) != IDLE && getData(communication_risc) != READ_HOST && getData(communication_risc) != ACK_ACK && !killed) {
        // wait for RISC to start listen
    }
    if (DEBUG_RISC_HOST_COMMUNICATION)
        printf("[Send Data to Risc] ---> send data = %x, RISC: %i, HOST: %i\n", d, getData(communication_risc),
               getData(communication_host));
    setData(communication_host_data, d);
    setData(communication_host, READ_HOST);
    if (DEBUG_RISC_HOST_COMMUNICATION)
        printf("[Send Data to Risc] -----> sent, RISC: %i, HOST: %i\n", getData(communication_risc),
               getData(communication_host));
    while (getData(communication_risc) != READ_HOST_ACK && !killed) {
        // wait for RISC to finish listen
    }
    if (DEBUG_RISC_HOST_COMMUNICATION)
        printf("[Send Data to Risc] -------> risc acked, RISC: %i, HOST: %i\n", getData(communication_risc),
               getData(communication_host));
    setData(communication_host, IDLE);
    if (DEBUG_RISC_HOST_COMMUNICATION)
        printf("[Send Data to Risc] ---------> return, RISC: %i, HOST: %i\n", getData(communication_risc),
               getData(communication_host));
}

int RiscCommunication::getCommunicationRisc() const {
    return communication_risc;
}

void RiscCommunication::setCommunicationRisc(int communicationRisc) {
    communication_risc = communicationRisc;
}

int RiscCommunication::getCommunicationHost() const {
    return communication_host;
}

void RiscCommunication::setCommunicationHost(int communicationHost) {
    communication_host = communicationHost;
}

int RiscCommunication::getCommunicationRISCData() const {
    return communication_risc_data;
}
int RiscCommunication::getCommunicationHOSTData() const {
    return communication_host_data;
}

void RiscCommunication::setCommunicationRISCData(int communicationData) {
    communication_risc_data = communicationData;
}
void RiscCommunication::setCommunicationHOSTData(int communicationData) {
    communication_host_data = communicationData;
}

void RiscCommunication::send_data_array(uint32_t address, uint32_t size, uint8_t *data) {
    if (size == 0) return;
    mutex.lock();
    console.printf_info("\t[Transfer] to MM@%i|0x%x, Size: %i\n", address, address, size);
    bin_array_send(address, size, data);
    usleep(10000);
    mutex.unlock();
}

void RiscCommunication::dump_data_array(uint32_t addr, int num_bytes, uint8_t *array_pnt, bool silent) {
    if (num_bytes == 0) return;
    mutex.lock();
    if (!silent)
        console.printf_info("\t[Transfer] from MM@%i|0x%x, Size: %i\n", addr, addr, num_bytes);
    bin_array_dump(addr, num_bytes, array_pnt);
    usleep(10000);
    mutex.unlock();
}

void RiscCommunication::dump_data_array_file(uint32_t addr, int num_bytes, const char *file_name) {
    if (num_bytes == 0) return;
    mutex.lock();
//    printf_info("\t[Transfer] from MM@%i|0x%x, Size: %i To: %s\n", addr, addr, num_bytes, file_name);
    bin_file_dump(addr, num_bytes, file_name);
//    usleep(10000);
    mutex.unlock();
}

