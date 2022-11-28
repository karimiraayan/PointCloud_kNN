//
// Created by gesper on 27.03.19.
//

#ifndef VPRO_CPP_LANESTAT_H
#define VPRO_CPP_LANESTAT_H

#include <map>
#include <string>
#include <QFile>

#include "../../commands/CommandVPRO.h"

class LaneStat {

public:
    LaneStat();

    void addExecutedCmdQueue(CommandVPRO *cmd);
    void addExecutedCmdTick(CommandVPRO *cmd);

    LaneStat *add(LaneStat other);

    void print(std::string prefix = "\t");

    std::map<CommandVPRO::TYPE, double[2]>* gettypeCount(){ return &typeCount; }

    LaneStat& operator/(float divisor) {
        for (auto &it : typeCount) {
            it.second[0] /= divisor;
            it.second[1] /= divisor;
        }
        return *this;
    }

    LaneStat& operator+(LaneStat other);
    LaneStat& operator-(LaneStat other);

    LaneStat& operator*(float factor) {
        for (auto &it : typeCount) {
            it.second[0] *= factor;
            it.second[1] *= factor;
        }
        return *this;
    }

    QString name;

    double getCyclesNotNONE();
    double getCyclesNONE();

    void headerToFile(QString name, QString filename, bool truncate = false);
    void appendToFile(QString name, QString filename);

    int getSrcStall(){ return src_stall; }
    int getDstStall(){ return dst_stall; }

    void incrementSrcStall(){ src_stall++; }
    void incrementDstStall(){ dst_stall++; }
private:
    std::map<CommandVPRO::TYPE, double[2]> typeCount; // queue + clockticks
    int dst_stall, src_stall;

};


#endif //VPRO_CPP_LANESTAT_H
