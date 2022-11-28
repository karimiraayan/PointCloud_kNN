//
// Created by gesper on 27.03.19.
//

#include "DMAStat.h"

DMAStat::DMAStat() //:
//        totalUnitCount(0)
{

}

void DMAStat::addExecutedCmdQueue(CommandDMA *cmd){
    typeCount[cmd->type][0]++;
    totalUnitCount[cmd->type] += cmd->unit.size();
}

void DMAStat::addExecutedCmdTick(CommandDMA *cmd){
    typeCount[cmd->type][1]++;
}

DMAStat *DMAStat::add(DMAStat other){
    for (auto &it : *other.gettypeCount()) {
        this->typeCount[it.first][0] += it.second[0];
        this->typeCount[it.first][1] += it.second[1];
    }
    for (auto &it : *other.getUnitCount()) {
        this->totalUnitCount[it.first] += it.second;
    }
    return this;
}

void DMAStat::print(std::string prefix){
    for (auto &it : typeCount) {
        printf("%s ", prefix.c_str());
        CommandDMA::printType(it.first);
        printf("  = %10.0lf (%10.0lf clock cycles)\n", it.second[0], it.second[1]);
    }
}


double DMAStat::getCyclesNotNONE(){
    double sum = 0;
    for (auto &it : typeCount) {
        sum += it.second[1];
    }
    sum -= getCyclesNONE();

    return sum;
}

double DMAStat::getCyclesNONE(){
    if (typeCount.find( CommandDMA::NONE ) == typeCount.end())
        return 0;

    return typeCount[CommandDMA::NONE][1];
}