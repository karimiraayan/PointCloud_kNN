//
// Created by gesper on 04.06.20.
//

#include "SimStat.h"
#include "../../../simulator/simCore.h"
#include "../../commands/CommandVPRO.h"

SimStat::SimStat(uint32_t cluster_count, uint32_t vector_unit_count) :
        cluster_count(cluster_count), vector_unit_count(vector_unit_count),
        old_m(0.0), new_m(0.0),
        old_s(0.0), new_s(0.0),
        instruction_count(0) {

    for (int busy_clusters = 0; busy_clusters < cluster_count; ++busy_clusters) {
        clock_counts[QString::number(busy_clusters) + "BusyClustersCycles"] = 0;
    }
    for (int busy_dmas = 0; busy_dmas < cluster_count; ++busy_dmas) {
        clock_counts[QString::number(busy_dmas) + "BusyDMAsCycles"] = 0;
    }
    for (int busy_units = 0; busy_units < vector_unit_count; ++busy_units) {
        clock_counts[QString::number(busy_units) + "BusyUnitsCycles"] = 0;
    }

    clock_counts["TotalBusyUnitCycles"] = 0;
    clock_counts["TotalBusyDMACycles"] = 0;
    clock_counts["TotalBusyClusterCycles"] = 0;
    clock_counts["TotalBusyUnitAndDMACycles"] = 0;
}

void SimStat::clockTick(SimCore &core){
    int busy_units = 0, busy_clusters = 0, busy_dmas = 0;
#ifndef SKIP_SIMSTAT_CORE
    for (auto c : core.getClusters()){
        if (c->dma->isBusy())
            busy_dmas++;
        if (c->isBusy())
            busy_clusters++;
    }
    for (auto u: core.getClusters()[0]->getUnits()){
        if (u->isBusy())
            busy_units++;
    }
#endif

    clock_counts[QString::number(busy_clusters)+"BusyClustersCycles"]++;
    clock_counts[QString::number(busy_dmas)+"BusyDMAsCycles"]++;
    clock_counts[QString::number(busy_units)+"BusyUnitsCycles"]++;

    if (busy_units > 0)
        clock_counts["TotalBusyUnitCycles"]++;
    if (busy_dmas > 0)
        clock_counts["TotalBusyDMACycles"]++;
    if (busy_clusters > 0)
        clock_counts["TotalBusyClusterCycles"]++;
    if ((busy_dmas > 0) && (busy_units > 0))
        clock_counts["TotalBusyUnitAndDMACycles"]++;
}

void SimStat::newCommandTick(CommandVPRO *cmd){
    if (cmd->type == CommandVPRO::NONE)
        return;

    double length = (cmd->x_end + 1) * (cmd->y_end + 1);

//    printf("STAT COMMAND: ");
//    cmd->print_type();
//    printf(" Length: %f \n ", length);
    instruction_count++;
    if (instruction_count == 1){
        old_m = length;
        new_m = length;
        old_s = 0;
    } else {
        new_m = old_m + (length - old_m) / double(instruction_count);
        new_s = old_s + (length - old_m) * (length - new_m);

        old_m = new_m;
        old_s = new_s;
    }
}

void SimStat::finishGroup(int number){

    lengthStatistic group{};
    group.instruction_count = instruction_count;
    group.mean = mean();
    group.variance = variance();
    group.standard_deviation = standard_deviation();

    instruction_count = 0;
    old_m = 0;
    old_s = 0;
    new_m = 0;
    new_s = 0;

    groupStats[number] = group;
}

void SimStat::print(double clock, QString prefix, QString *result){
    QString string;
    QTextStream stream(&string);
    QString finalstring;
    QTextStream output(&finalstring);
    stream.setLocale(QLocale());
    stream.setRealNumberNotation(QTextStream::FixedNotation);
    stream.setRealNumberPrecision(0);

    auto tmp = prefix.toStdString();
    const char *pref = tmp.c_str();
    stream << pref << "#####################################" << "\n";
    stream << pref << "SimStats: " << "\n";

    stream << pref << "Cycles for DMAs:" << "\n";
    for(int cluster=0; cluster <= cluster_count; cluster++){
        stream << prefix << "\tDMA     active ["<< cluster <<" active] " << clock_counts[QString::number(cluster)+"BusyDMAsCycles"];
        output << string << "\n";
        string.clear();
    }
    stream << pref << "Cycles for Clusters:" << "\n";
    for(int cluster=0; cluster <= cluster_count; cluster++){
        stream << prefix << "\tCluster active ["<< cluster <<" active] " << clock_counts[QString::number(cluster)+"BusyClustersCycles"];
        output << string << "\n";
        string.clear();
    }
    stream << pref << "Cycles for Units (Mean over all clusters [expected no difference in clusters]):" << "\n";
    for(int unit=0; unit <= vector_unit_count; unit++){
        stream << prefix << "\tUnit    active ["<< unit <<" active] " << clock_counts[QString::number(unit)+"BusyUnitsCycles"];
        output << string << "\n";
        string.clear();
    }
    stream << pref << "Active Cycles in Total:" << "\n";

    stream.setRealNumberPrecision(2);
    stream << prefix << "\tUnit (VPRO):       "<< clock_counts["TotalBusyUnitCycles"] << " [" << (100*double(clock_counts["TotalBusyUnitCycles"])/clock) << " %]";
    output << string << "\n";
    string.clear();

    stream << prefix << "\tDMA:               "<< clock_counts["TotalBusyDMACycles"] << " [" << (100*double(clock_counts["TotalBusyDMACycles"])/clock) << " %]";
    output << string << "\n";
    string.clear();

    stream << prefix << "\tCluster (Any):     "<< clock_counts["TotalBusyClusterCycles"] << " [" << (100*double(clock_counts["TotalBusyClusterCycles"])/clock) << " %]";
    output << string << "\n";
    string.clear();

    stream << prefix << "\tUnit + DMA (both): "<< clock_counts["TotalBusyUnitAndDMACycles"] << " [" << (100*double(clock_counts["TotalBusyUnitAndDMACycles"])/clock) << " %]";
    output << string << "\n";
    string.clear();

    stream.setRealNumberPrecision(0);
    stream << pref << "Clock Cycles: " << clock;
    output << string << "\n";
    string.clear();

    stream.setRealNumberPrecision(2);
    if (groupStats.empty()){
        stream << pref << "Vector-Length of all "<< instruction_count <<" commands cycles: " <<
            "Mean: " << mean() << ", " <<
            "Std-Derivation: " << standard_deviation() << ", " <<
            "Variance: " << variance();
        output << string << "\n";
    } else {
        for (auto item: groupStats.keys()){
            stream << pref << "[Group "<< item <<"] Vector-Length of "<< groupStats[item].instruction_count <<" commands cycles: " <<
                   "Mean: " << groupStats[item].mean << ", " <<
                   "Std-Derivation: " << groupStats[item].standard_deviation << ", " <<
                   "Variance: " << groupStats[item].variance;
            output << string << "\n";
            string.clear();
        }
    }
    string.clear();

    if (result != nullptr)
        result->append(finalstring);
    else
        qDebug().noquote() << finalstring;
}
