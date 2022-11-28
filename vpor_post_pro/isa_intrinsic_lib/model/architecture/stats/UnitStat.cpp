//
// Created by gesper on 19.07.19.
//

#include <QDebug>
#include <QTextStream>
#include <stdio.h>

#include "UnitStat.h"

UnitStat::UnitStat() :
        waitingVPROCount(0),
        waitingDMACount(0)
{

}

void UnitStat::tick(int busy){
    busyLaneCount[busy] += 1;
}

void UnitStat::dmaBusyTick(){
    waitingDMACount++;
}

void UnitStat::vproBusyTick(){
    waitingVPROCount++;
}

UnitStat *UnitStat::add(UnitStat other){
    for (auto &it : *other.getBusyCount()) {
        busyLaneCount[it.first] += it.second;
    }
    waitingVPROCount += other.getWaitingVPROCount();
    waitingDMACount  += other.getWaitingDMACount();
    return this;
}

void UnitStat::print(QString prefix){
    QTextStream stream(stdout);
    stream.setRealNumberNotation(QTextStream::FixedNotation);
    stream.setRealNumberPrecision(0);

    double clock = 0;
    for (auto &it : busyLaneCount) {
        clock += it.second;
    }
    stream << prefix.toStdString().c_str() << "Unit Stats:\n";
    stream << prefix.toStdString().c_str() << "    Total Clock Cycles: " << clock << "\n";
    stream << prefix.toStdString().c_str() << "    Lanes usage: " << "\n";

    for (auto &it : busyLaneCount) {
        stream.setFieldWidth(0);
        stream << prefix.toStdString().c_str();
        stream.setFieldWidth(10);
        stream << it.first << " Lanes Busy: " << it.second;
        stream.setFieldWidth(5);
        stream.setRealNumberPrecision(2);
        stream << " [ " << (it.second / clock * 100);
        stream << "% ]\n";
        stream.setRealNumberPrecision(0);
    }

    stream.setFieldWidth(0);
    stream.setRealNumberPrecision(0);
    stream << prefix.toStdString().c_str() << "DMA/VPRO wait Cycles: " << "\n";
    stream << prefix.toStdString().c_str() << "    waiting for DMA:   ";
    stream.setFieldWidth(10);
    stream << waitingDMACount << "\n";
    stream.setFieldWidth(0);
    stream << prefix.toStdString().c_str() << "    waiting for VPRO:  ";
    stream.setFieldWidth(10);
    stream<< waitingVPROCount << "\n";
    stream.setFieldWidth(0);

    //printf(" = %9.1lf (%9.1lf clock cycles)\n", it.second[0], it.second[1]);
}
