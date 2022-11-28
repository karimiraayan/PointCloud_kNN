//
// Created by gesper on 27.03.19.
//

#ifndef VPRO_CPP_DMASTAT_H
#define VPRO_CPP_DMASTAT_H

#include <map>
#include <string>

#include "../../commands/CommandDMA.h"

class DMAStat {

public:
    DMAStat();

    void addExecutedCmdQueue(CommandDMA *cmd);
    void addExecutedCmdTick(CommandDMA *cmd);

    DMAStat *add(DMAStat other);

    void print(std::string prefix = "\t");

    std::map<CommandDMA::TYPE, double[2]>* gettypeCount(){ return &typeCount; }
    std::map<CommandDMA::TYPE, double>* getUnitCount(){ return &totalUnitCount; }

    DMAStat& operator/(float divisor) {
        for (auto &it : typeCount) {
            it.second[0] /= divisor;
            it.second[1] /= divisor;
        }
        for (auto &it : totalUnitCount) {
            it.second /= divisor;
        }
        return *this;
    }

    DMAStat& operator*(float factor) {
        for (auto &it : typeCount) {
            it.second[0] *= factor;
            it.second[1] *= factor;
        }
        for (auto &it : totalUnitCount) {
            it.second *= factor;
        }
        return *this;
    }

    double getCyclesNotNONE();
    double getCyclesNONE();

    void printAverageUnitUse(std::string prefix = "\t"){
        for (auto &it : typeCount) {
            printf("%s", prefix.c_str());
            CommandDMA::printType(it.first);
            printf(" Avg. Units: %lf\n", it.second[1] / totalUnitCount[it.first]);
        }
    }

    double getAverageUnitUse(){
        double res = 0;
        for (auto &it : totalUnitCount) {
            res += it.second;
        }
        res /= totalUnitCount.size();
        return res;
    }
private:
    std::map<CommandDMA::TYPE, double[2]> typeCount; // queue + clockticks

    std::map<CommandDMA::TYPE, double> totalUnitCount;

};


#endif //VPRO_CPP_DMASTAT_H
