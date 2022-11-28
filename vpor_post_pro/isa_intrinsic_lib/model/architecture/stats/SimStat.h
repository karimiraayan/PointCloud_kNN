//
// Created by gesper on 04.06.20.
//

#ifndef CNN_MOBILENET_DCT_SIMSTAT_H
#define CNN_MOBILENET_DCT_SIMSTAT_H

#include <math.h>
#include <QString>
#include <QDebug>

class SimCore;
class CommandVPRO;

class SimStat {

public:
    SimStat(uint32_t cluster_count = 0, uint32_t vector_unit_count = 0);

    /**
     * each counter gets updated. called from sims clock function
     */
    void clockTick(SimCore &core);

    void print(double clock, QString prefix = "\t", QString *result = nullptr);

    const std::map<QString, double> &getStats() const{
        return clock_counts;
    };

    void newCommandTick(CommandVPRO *cmd);

    void finishGroup(int number);
private:
    std::map<QString, double> clock_counts;

    uint32_t cluster_count;
    uint32_t vector_unit_count;

    double old_m, new_m;
    double old_s, new_s;
    long instruction_count;

    double mean(){
        return new_m;
    };
    double variance(){
        if (instruction_count > 1)
            return new_s / double(instruction_count - 1);
        else
            return 0.0;
    };
    double standard_deviation(){
        return sqrt(variance());
    };

    struct lengthStatistic{
        double mean;
        double variance;
        double standard_deviation;
        long instruction_count;
    };
    QMap<int, lengthStatistic> groupStats;

};


#endif //CNN_MOBILENET_DCT_SIMSTAT_H
