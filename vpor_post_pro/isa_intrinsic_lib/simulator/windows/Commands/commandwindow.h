#ifndef COMMANDWINDOW_H
#define COMMANDWINDOW_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <inttypes.h>

#include <QVector>
#include <QMainWindow>
#include "../../../model/commands/CommandVPRO.h"
#include "../../../model/commands/CommandDMA.h"
#include "../../../model/commands/CommandSim.h"
#include <QStandardItemModel>
#include <QProgressBar>
#include <QFile>
#include <QMetaEnum>
#include <QFileDevice>
#include <QFileDialog>
#include <QTableView>
#include <QTableWidget>
#include "lmtableview.h"
#include "rftableview.h"
#include <QMessageBox>
#include "../../../model/architecture/stats/LaneStat.h"
#include "../../../model/architecture/stats/DMAStat.h"
#include "../../../model/architecture/stats/UnitStat.h"
#include "../../../model/architecture/stats/SimStat.h"
#include <QMetaType>
#include <QLineEdit>
#include <QSettings>
#include <QRadioButton>
#include "savesettings.h"
#include "savemain.h"
#include "siminitfunctions.h"
#include <fstream>
#include <QDebug>
#include <tuple>
#include <QComboBox>
#include "simupdatefunctions.h"
#include "QStackedLayout"
#pragma once
#include "simupdatefunctions.h"
#include "savemain.h"
#include "siminitfunctions.h"
#include "dataupdatefunctions.h"
#include "../../../simulator/helper/debugHelper.h"


namespace Ui {
class CommandWindow;
}

class CommandWindow : public QMainWindow
{
    Q_OBJECT

public:
    struct Data{
        QVector<QVector<std::shared_ptr<CommandVPRO>>> command_queue;   // [cluster][unit] list of cmds in queue
        QVector<std::shared_ptr<CommandVPRO>> commands;                 // command in all cluster*units*lanes lane's ALU
        QVector<bool> isLooppush;                                       // [cluster]
        QVector<bool> isLoppparsing;                                    // [cluster]
        QVector<bool> isbusy;                                           // [cluster]
        QVector<bool> isDMAbusy;                                        // [cluster]

        DMAStat summedDMAStat;
        LaneStat *summedLanesStat; // [lane]
        UnitStat summedUnitStat;
        SimStat simStat;

        long clock;
        Data(){
        };
    };

    explicit CommandWindow(int clusters, int units, int lanes = 2, QWidget *parent = nullptr);
    ~CommandWindow();

    QVector<QComboBox*> lmboxes;
    QVector<QVector<QComboBox*>> rfboxes;
    uint8_t *main_memory;
    QScrollArea *clusterhscroll = new QScrollArea;
    QVector<QProgressBar*> progressbars; //vector of progressbars for each lane
    QVector<QVector<LaneStat>> alllanestats;
    QVector<LmTableView *>lmviews;
    QVector<QVector<RfTableView *>>rfviews;
    QVector<long> clockvalues; //saves the clockvalue to the corresponding lanestats saved in the vector alllanestats
    QVector<double> progresstotal;
    QVector<QLineEdit*> offsets;
    QVector<QLineEdit*> sizes;
    QVector<QStackedLayout *> qstacks;
    void RemoveLayout(QWidget*);

public slots:
    void dataUpdate(CommandWindow::Data data);
    void simUpdate(long clock);
    void simIsPaused();
    void simIsResumed();
    void getmainmemory(uint8_t*);
    void on_pauseButton_clicked();
    void setLocalMemory(int unit, uint8_t*,int c);
    void setRegisterMemory(int lane,uint8_t *ref,int c,int u);
    void recieveguiwaitpress(bool*);

private slots:
    void on_cycle100button_clicked();
    void on_cycle1button_clicked();
    void on_cycleButton_clicked();
    void on_exitButton_clicked();
    void on_lanecheck_clicked();
    void on_consoledumpcheck_clicked();
    void on_mainmemorycheck_clicked();
    void on_lmcheck_clicked();
    void on_command3check_clicked();
    void on_tableint_clicked();
    void on_tablehex_clicked();
    void on_tablebit_clicked();

signals:
    void pauseSim();
    void runSteps(int);
    void dumpRegisterFile(int, int, int);
    void dumpLocalMemory(int, int);
    void laneupdate(int);
    void savemainclicked(int);
    void getSimupdate();
    void closeotherwindows();

private:
    Ui::CommandWindow *ui;
    Data data;
    int cluster_cnt, unit_cnt, lane_cnt,startclock;
    bool paused;
    LmTableView *lm_view;
};

Q_DECLARE_METATYPE(CommandWindow::Data)
#endif // COMMANDWINDOW_H
