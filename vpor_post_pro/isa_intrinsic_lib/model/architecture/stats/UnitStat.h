//
// Created by gesper on 19.07.19.
//

#ifndef VPRO_TUTORIAL_CNN_UNITSTAT_H
#define VPRO_TUTORIAL_CNN_UNITSTAT_H

#include <map>
#include <string>
#include <QString>

class UnitStat {

public:
    UnitStat();

    void tick(int busy);

    void dmaBusyTick();
    void vproBusyTick();

    void print(QString prefix = QString("#\t"));

    std::map<int, double>* getBusyCount(){ return &busyLaneCount; }

    UnitStat *add(UnitStat other);

    UnitStat& operator/(float divisor) {
        for (auto &it : busyLaneCount) {
            it.second /= divisor;
        }
        waitingDMACount /= divisor;
        waitingVPROCount /= divisor;
        return *this;
    }

    UnitStat& operator*(float factor) {
        for (auto &it : busyLaneCount) {
            it.second *= factor;
        }
        waitingDMACount *= factor;
        waitingVPROCount *= factor;
        return *this;
    }

    double getWaitingDMACount(){ return waitingDMACount; }
    double getWaitingVPROCount(){ return waitingVPROCount; }
private:
    /**
     * 0 / 1 / 2 lanes are busy + clockticks (++ each units clock tick)
     */
    std::map<int, double> busyLaneCount;

    /**
     * no lanes are busy && cluster waiting for DMA active
     * clockticks (++ each units clock tick)
     */
    double waitingDMACount;

    /**
     * no lanes are busy && cluster waiting for VPRO active
     * (should be 0 for even distributed cmds to all units/clusters)
     * clockticks (++ each units clock tick)
     */
    double waitingVPROCount;
};


#endif //VPRO_TUTORIAL_CNN_UNITSTAT_H
