#ifndef SAVEMAIN_H
#define SAVEMAIN_H

#include <QFile>
#include <QFileDevice>
#include <QFileDialog>
#include <QWidget>
#include <QMainWindow>
#include <QMessageBox>
#include <QLineEdit>
#include <QVector>
#include <QObject>
#include <QDebug>
#pragma once
class CommandWindow;
#include "commandwindow.h"

void savemaintofile(int i,CommandWindow *Widget,QVector<QLineEdit*> offsets,QVector<QLineEdit*> sizes,bool *paused);
#endif // SAVEMAIN_H
