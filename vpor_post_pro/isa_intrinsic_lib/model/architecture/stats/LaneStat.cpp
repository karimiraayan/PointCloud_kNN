//
// Created by gesper on 27.03.19.
//

#include <QTextStream>

#include "LaneStat.h"

LaneStat::LaneStat(){
    name = QString("");
    dst_stall = 0;
    src_stall = 0;
}

void LaneStat::addExecutedCmdQueue(CommandVPRO *cmd){
    typeCount[cmd->type][0]++;
}

void LaneStat::addExecutedCmdTick(CommandVPRO *cmd){
    typeCount[cmd->type][1]++;
}

LaneStat *LaneStat::add(LaneStat other){
    for (auto &it : *other.gettypeCount()) {
        this->typeCount[it.first][0] += it.second[0];
        this->typeCount[it.first][1] += it.second[1];
    }
    dst_stall += other.getDstStall();
    src_stall += other.getSrcStall();
    return this;
}

LaneStat& LaneStat::operator+(LaneStat other){
    for (auto &it : *other.gettypeCount()) {
        this->typeCount[it.first][0] += it.second[0];
        this->typeCount[it.first][1] += it.second[1];
    }
    dst_stall += other.getDstStall();
    src_stall += other.getSrcStall();
    return *this;
}

LaneStat& LaneStat::operator-(LaneStat other){
    for (auto &it : *other.gettypeCount()) {
        this->typeCount[it.first][0] -= it.second[0];
        this->typeCount[it.first][1] -= it.second[1];
    }
    dst_stall -= other.getDstStall();
    src_stall -= other.getSrcStall();
    return *this;
}

void LaneStat::headerToFile(QString name, QString filename, bool truncate){
    QFile file(filename);
    if (truncate){
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
            return;
    }
    else{
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
            return;
    }

    QTextStream out(&file);
    if (truncate)
        out << "\n";
    out << name << "\n";
    out << "Step\t";
    for (int i = 0; i < CommandVPRO::enumTypeEnd; i++) {
        auto eCurrent = (CommandVPRO::TYPE) i;
        out << "\t" << CommandVPRO::getType(eCurrent);
    }
    out << "\t" << "Src-Stalls";
    out << "\t" << "Dst-Stalls";
}

void LaneStat::appendToFile(QString name, QString filename){
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
        return;

    QTextStream out(&file);
    out << "\n" << name;
    for (int i = 0; i < CommandVPRO::enumTypeEnd; i++) {
        auto eCurrent = (CommandVPRO::TYPE) i;
        out << "\t" << typeCount[eCurrent][1]; // clock ticks
    }

    out << "\t" << QString::number(src_stall);
    out << "\t" << QString::number(dst_stall);
}

void LaneStat::print(std::string prefix){
    printf("%sStats: %s\n", prefix.c_str(), name.toStdString().c_str());

    auto sum = new double[2]();
    for (auto &it : typeCount) {
        sum[0] += it.second[0];
        sum[1] += it.second[1];
    }
    printf("%s--Sum               = %10.0lf (%10.0lf clock cycles)\n", prefix.c_str(), sum[0], sum[1]);
    if (typeCount.find( CommandVPRO::NONE ) != typeCount.end()){
        sum[0] -= typeCount[CommandVPRO::NONE][0];
        sum[1] -= typeCount[CommandVPRO::NONE][1];
        printf("%s--Sum (no NONE)     = %10.0lf (%10.0lf clock cycles)\n", prefix.c_str(), sum[0], sum[1]);
    }

    for (auto &it : typeCount) {
        printf("%s \t", prefix.c_str());
        CommandVPRO::printType(it.first);
        printf("   = %10.0lf (%10.0lf clock cycles)\n", it.second[0], it.second[1]);
    }
    printf("%s ", prefix.c_str());
    printf("Stalls due to SRC: (%10.0i clock cycles), ", src_stall);
    printf(" DST: (%10.0i clock cycles)\n", dst_stall);

    delete[] sum;
}

double LaneStat::getCyclesNotNONE(){
    double sum = 0;
    for (auto &it : typeCount) {
        sum += it.second[1];
    }
    sum -= getCyclesNONE();

    return sum;
}

double LaneStat::getCyclesNONE(){
    if (typeCount.find( CommandVPRO::NONE ) == typeCount.end())
        return 0;

    return typeCount[CommandVPRO::NONE][1];
}
