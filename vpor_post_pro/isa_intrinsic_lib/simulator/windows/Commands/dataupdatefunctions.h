#ifndef DATAUPDATEFUNCTIONS_H
#define DATAUPDATEFUNCTIONS_H
#include <QWidget>
#include <QObject>
#include <QLabel>
#include <QLayout>
#include <QScrollArea>
#include <QPushButton>
#pragma once
#include "commandwindow.h"
#include "commandstablewidget.h"



QWidget *createunitwidget(CommandWindow *mainwindow,int unitqueue,int u,int lane_cnt,QVector<std::shared_ptr<CommandVPRO>> commands,QVector<QVector<std::shared_ptr<CommandVPRO>>> command_queue,bool paused,int *cmd,QVector<bool> isLoopparsing,QVector<bool> isLopppush,int c);

void createtablewindow(CommandWindow *mainwindow,std::shared_ptr<CommandVPRO> cmd);

QScrollArea *createcommandqueue(QVector<QVector<std::shared_ptr<CommandVPRO>>> command_queue,CommandWindow *mainwindow,int unitqueue);

QGridLayout *createlaneslayout(int lane_cnt,int *cmd,QVector<std::shared_ptr<CommandVPRO>> commands,CommandWindow *mainwindow);

void printcommandqueue(QVector<std::shared_ptr<CommandVPRO>> command_queue,int c,int u);

QWidget* createcommandcluster (int cluster_cnt,QVector<bool> isDMAbusy,QVector<bool> isbusy); //creates a widget for each cluster, in which are later the unitwidgets of the dataupdate are added
#endif // DATAUPDATEFUNCTIONS_H

