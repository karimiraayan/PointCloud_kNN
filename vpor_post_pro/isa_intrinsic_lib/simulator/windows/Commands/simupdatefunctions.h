#ifndef SIMUPDATEFUNCTIONS_H
#define SIMUPDATEFUNCTIONS_H
#include <QVector>
#include <QProgressBar>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QRadioButton>
#include "../../../model/architecture/stats/LaneStat.h"
#include <QDebug>

int updatelanes(QVector<QRadioButton*> radiobuttons,QVector<QProgressBar*> progressbars,QVector<QVector<LaneStat>> alllanestats,long clock,QVector<long> clockvalues,QVector<double> progresstotal); //update for the last x cycles

int updatelanestotal(QVector<QProgressBar*> progressbars,long clock,QVector<LaneStat *> lanestats); //update for all cycles

#endif // SIMUPDATEFUNCTIONS_H
