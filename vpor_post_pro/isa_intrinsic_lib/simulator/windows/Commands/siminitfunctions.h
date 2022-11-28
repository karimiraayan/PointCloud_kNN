#ifndef SIMINITFUNCTIONS_H
#define SIMINITFUNCTIONS_H
#include <QWidget>
#include <QProgressBar>
#include <QTabWidget>
#include <QLabel>
#include <QLayout>
#include <QVector>
#include <QGroupBox>
#include <QLineEdit>
#include <QPushButton>
#include <QStackedWidget>
#include <QObject>
#include <tuple>
#include<QComboBox>
#include <math.h>
#include "../../../simulator/helper/debugHelper.h"
#pragma once
class CommandWindow;
#include "commandwindow.h"
#include "savemain.h"



QVBoxLayout* createlaneprogressbars(int cluster_cnt,int unit_cnt,int lane_cnt,QLabel *label,QGroupBox *radiobuttongroup);

QVBoxLayout* createlmdumpbuttons(CommandWindow* Window, int cluster_cnt, int unit_cnt);

QVBoxLayout* createregisterdumpbuttons(CommandWindow* Window, int cluster_cnt, int unit_cnt,int lane_cnt);

QWidget* creatememorytable(int unit_cnt,int lane_cnt); //creates lm table which also contains register table

std::tuple <QVector<QLineEdit*>,QVector<QLineEdit*>> createmainmemorydumpbuttons(bool *paused, QVector<QLineEdit*> offsets,QVector<QLineEdit*> sizes,CommandWindow* Window,QGridLayout *mainbuttons);

void createdebugoptions(QWidget* parent);

QWidget* createregistertable(int lane_cnt);


#endif // SIMINITFUNCTIONS_H
