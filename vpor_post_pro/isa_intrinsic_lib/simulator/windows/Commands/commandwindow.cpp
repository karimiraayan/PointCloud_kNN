#include "commandwindow.h"
#include "ui_commandwindow.h"
#include "lmtableview.h"
#include <math.h>


CommandWindow::CommandWindow(int clusters, int units, int lanes, QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::CommandWindow) {
    ui->setupUi(this);

//    QLocale german(QLocale::German);
    QLocale::setDefault(QLocale(QLocale::English, QLocale::UnitedStates));

    paused = true;
    cluster_cnt = clusters;
    unit_cnt = units;
    lane_cnt = lanes;


    progresstotal.resize(cluster_cnt * unit_cnt * lane_cnt);

    RemoveLayout(ui->rfdump);
    RemoveLayout(ui->lmdump);
    RemoveLayout(ui->mainmembuttongroup);
    RemoveLayout(ui->Lanestab);
    RemoveLayout(ui->lmtabs);

    clusterhscroll->setWidgetResizable(true);
    ui->tabwidget->insertTab(4, clusterhscroll, "Commands (Lane+Unit's Q)"); //creates tab with commands

    ui->Lanestab->setLayout(createlaneprogressbars(cluster_cnt, unit_cnt, lane_cnt, ui->Labellastcycles,
                                                   ui->radiobuttongroup)); //setlayout for lanestab creating the progressbars for each lane
    QList<QProgressBar *> progressbarslist = ui->Lanestab->findChildren<QProgressBar *>(); //finds progressbars of lanes
    progressbars = progressbarslist.toVector();

    simUpdate(0);

    QList<QRadioButton *> radiobuttons = ui->radiobuttongroup->findChildren<QRadioButton *>();
    for (int i = 0; i <
                    radiobuttons.size(); i++) { //connects the radiobuttons of laneupdate with the signal getsimupdate so data is get updated instantly when another button is clicked
        connect(radiobuttons[i], &QRadioButton::clicked, [this]() { this->emit getSimupdate(); });
    }
    rfboxes.resize(cluster_cnt * unit_cnt);
    for (int c = 0; c < cluster_cnt; c++) {  //creates the tables for local memory and registerfiles
        ui->lmtabs->addTab(creatememorytable(unit_cnt, lane_cnt), "Cluster " + QString::number(c));
    }
    QList<LmTableView *> lmlist = ui->lmtabs->findChildren<LmTableView *>();//finds the localmemory tables
    QList<QComboBox *> lmboxeslist = ui->lmtabs->findChildren<QComboBox *>();
    lmboxes = lmboxeslist.toVector();
    lmviews = lmlist.toVector();
    bool first = true;
    for (auto unit : ui->lmtabs->findChildren<QStackedWidget *>()) {
        if (first == true) {//skips first qstackedwidget because its the tab itself
            first = false;
            continue;
        }
        QList<RfTableView *> rmlist = unit->findChildren<RfTableView *>();//finds the registerfilememorys
        QList<QComboBox *> rfboxeslist = unit->findChildren<QComboBox *>();// finds the registerfilecomboboxes
        rfboxes.append(rfboxeslist.toVector());
        rfviews.append(rmlist.toVector());
    }
    for (int i = 0; i < log(float(DebugOptions::end)) /
                        log(2.); i++) { //loops trough debugoption creating corresponding checkboxes and connection to the debugoptions enum
        auto option = DebugOptions(1 << i);
        QString option_text = debugToText(option);
        if (option_text != "end") {
            QCheckBox *optionbox = new QCheckBox;
            optionbox->setText(option_text);
            connect(optionbox, QOverload<bool>::of(&QCheckBox::clicked), [optionbox, option](bool checked) {
                if (checked) {
                    debug = debug | option;
                } else if (!checked) {
                    debug = debug & ~option;
                }
            });
            ui->scrollAreaWidgetContents_3->layout()->addWidget(optionbox);
        }
    }
//    QWidget *scrolleareawidget = new QWidget;
//    createdebugoptions(ui->scrollAreaWidgetContents_3);
//    ui->scrollArea_3->setWidget(scrolleareawidget);
    QGridLayout *mainbuttons = new QGridLayout;
    bool *pausedptr = &paused;//creates pointer to the paused variable
    std::tie(offsets, sizes) = createmainmemorydumpbuttons(pausedptr, offsets, sizes, this,
                                                           mainbuttons); //creates mainmemorydumpbuttons and links them to offsets and sizes vectors
    ui->mainmembuttongroup->setLayout(mainbuttons);

    ui->rfdump->setLayout(createregisterdumpbuttons(this, cluster_cnt, unit_cnt, lane_cnt));
    ui->lmdump->setLayout(createlmdumpbuttons(this, cluster_cnt, unit_cnt));

    QList<QCheckBox *> checkboxes = ui->settingstab->findChildren<QCheckBox *>(); //findes checkboxes in settingstab to restoresettings after init
    restoresettings(offsets, sizes, checkboxes);
    QList<QCheckBox *> tabcheckboxes = ui->settingsbox->findChildren<QCheckBox *>(); //finds checkboxes in tabsettingsbox to enable tabs by looping trough the list
    for (int i = 0; i < tabcheckboxes.size(); i++) {
        ui->tabwidget->setTabEnabled(i, tabcheckboxes[i]->isChecked());
    }
    for (auto debugbox : ui->scrollAreaWidgetContents_3->findChildren<QCheckBox *>()) {//loops trough all checked debugoptionboxes one time to emit signal if checked to activate this option
        if (debugbox->isChecked()) {
            emit debugbox->clicked(true);
        }
    }
}

Q_DECLARE_METATYPE(CommandBase*);

void CommandWindow::dataUpdate(Data data) {
    if (ui->command3check->isChecked()) {
        qDebug() << "Update of Command List Tab is on TODO-List!";


        //    overallDMA = overallDMA / (float) cluster_count;
        //    for (auto &stat : overalllane) {
        //        stat = stat / (float) (cluster_count * vector_unit_count);
        //    }
        //    overallUnit = overallUnit / (cluster_count * vector_unit_count);


//        if (data.commands.size() > 0) {
//            QString text;
//            int cmdcount = 0;
//            int *cmd = &cmdcount;
//            int unitqueue = 0;
//            clusterhscroll->setWidget(createcommandcluster(cluster_cnt, data.isDMAbusy,
//                                                           data.isbusy)); //creates commandcluster in commandtab with label scrollarea etc
//            QList<QScrollArea *> clusterscrollslist = ui->tabwidget->widget(
//                    4)->findChildren<QScrollArea *>();//finds scrollareas of each cluster in the commandtabwidget and adds them to a list
//            QVector<QScrollArea *> clusterscrollareas = clusterscrollslist.toVector();
//            for (int c = 0; c < cluster_cnt; c++) {
//                QWidget *clusterscrollwidget = new QWidget;
//                QVBoxLayout *clustervlayout = new QVBoxLayout;
//
//                for (int u = 0; u < unit_cnt; u++) {
//                    clustervlayout->addWidget(
//                            createunitwidget(this, unitqueue, u, this->lane_cnt, data.commands, data.command_queue,
//                                             paused, cmd, data.isLoppparsing, data.isLooppush,
//                                             c)); //creates units for each cluster with command and commandqueue
//                    unitqueue++;
//                    if (unitqueue > data.command_queue.size()) {
//                        break;
//                    }
//                }
//                clusterscrollwidget->setLayout(clustervlayout);
//                clusterscrollareas[c]->setWidget(clusterscrollwidget);
//            }
//        }
    }
    QString statistic_text;
    data.simStat.print(data.clock, "\t", &statistic_text);
    ui->statistic_textbrowser->setText(statistic_text);
}

void CommandWindow::simUpdate(long clock) {
    if (ui->lmtabs->count() > 0 && ui->tabwidget->currentWidget() == ui->tab_localMemory) {
        LmTableView *current = ui->lmtabs->currentWidget()->findChild<LmTableView *>();//finds lmtableview in the current activated tab
        current->model()->dataChanged(QModelIndex(), QModelIndex());//updates data of lm tableview
        QStackedWidget *stack = ui->lmtabs->currentWidget()->findChild<QStackedWidget *>();//finds Qstackedwidget in the current activated tab
        RfTableView *rf = stack->currentWidget()->findChild<RfTableView *>();//finds rftableview on the current activated stack of the qstackedwidget
        rf->model()->dataChanged(QModelIndex(), QModelIndex());//updates data of rf tableview
    }
    ui->cycle_label->setText("Cycle: " + QLocale().toString(qlonglong(clock)));

//    clockvalues.append(clock);//stores clock in a vector
    if (ui->lanecheck->isChecked()) { //shows percentual usage of lane, relation of NONE commands to total commands
        //qDebug() << "Update of Lane Stats Tab is on TODO-List!";
    }
}

void CommandWindow::RemoveLayout(QWidget *widget) {
    QLayout *layout = widget->layout();
    if (layout != nullptr) {
        QLayoutItem *item;
        while ((item = layout->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
        delete layout;
    }
}

void CommandWindow::simIsPaused() {
    ui->sim_status_label->setText("Paused");
    ui->pauseButton->setText("Resume Simulation");

}

void CommandWindow::simIsResumed() {
    ui->sim_status_label->setText("Running");
    ui->pauseButton->setText("Pause Simulation");
}

CommandWindow::~CommandWindow() {

    delete ui;
}

void CommandWindow::on_cycle100button_clicked() {
    emit runSteps(100);
}

void CommandWindow::on_cycle1button_clicked() {
    emit runSteps(1);
}

void CommandWindow::on_pauseButton_clicked() {
    if (!paused) {
        paused = true;
    } else {
        paused = false;
    }
    emit pauseSim();
}

void CommandWindow::on_cycleButton_clicked() {
    emit runSteps(ui->cycleInput->value());
}

void CommandWindow::getmainmemory(uint8_t *adress) {
    main_memory = adress;
}

void CommandWindow::setLocalMemory(int unit, uint8_t *ref, int c) {
    lmviews[cluster_cnt - c - 1]->setLocalMemory(unit, ref);
    lmviews[cluster_cnt - c - 1]->selectLocalMemory(0);
}

void CommandWindow::setRegisterMemory(int lane, uint8_t *ref, int c, int u) {
    rfviews[cluster_cnt - c - 1][unit_cnt - u - 1]->setRegisterFile(lane, ref);
    rfviews[cluster_cnt - c - 1][unit_cnt - u - 1]->selectRegisterFile(0);
}

void CommandWindow::on_exitButton_clicked() {
    QList<QCheckBox *> checkedtorestore = ui->settingstab->findChildren<QCheckBox *>();
    savesettings(offsets, sizes, checkedtorestore);
}

void CommandWindow::on_lanecheck_clicked() {
    ui->tabwidget->setTabEnabled(0, ui->lanecheck->isChecked());
}

void CommandWindow::on_consoledumpcheck_clicked() {
    ui->tabwidget->setTabEnabled(1, ui->consoledumpcheck->isChecked());
}

void CommandWindow::on_mainmemorycheck_clicked() {
    ui->tabwidget->setTabEnabled(2, ui->mainmemorycheck->isChecked());
}

void CommandWindow::on_lmcheck_clicked() {
    ui->tabwidget->setTabEnabled(3, ui->lmcheck->isChecked());
}

void CommandWindow::on_command3check_clicked() {
    ui->tabwidget->setTabEnabled(4, ui->command3check->isChecked());
}

void CommandWindow::on_tableint_clicked() {
    for (int i = 0; i < lmviews.size(); i++) {//loop which changes lmviewdata to int and sets columnwidth
        lmviews[i]->modelData->basevalue = 10;
        for (int j = 0; j < lmviews[i]->modelData->COLS; j++) {
            lmviews[i]->setColumnWidth(j, 100);
        }
    }
    for (int j = 0; j < rfviews.size(); j++) {//loop which changes rfviewdata to int and sets columnwidth
        for (int k = 0; k < rfviews[j].size(); k++) {
            rfviews[j][k]->modelData->basevalue = 10;
            rfviews[j][k]->resizeColumnsToContents();
            for (int t = 0; t < rfviews[j][k]->modelData->COLS; t++) {
                rfviews[j][k]->setColumnWidth(t, 100);
            }
        }
    }
}

void CommandWindow::on_tablehex_clicked() {
    for (int i = 0; i < lmviews.size(); i++) {//loop which changes lmviewdata to hex and sets columnwidth
        lmviews[i]->modelData->basevalue = 16;
        lmviews[i]->resizeColumnsToContents();
    }
    for (int j = 0; j < rfviews.size(); j++) {//loop which changes rfviewdata to hex and sets columnwidth
        for (int k = 0; k < rfviews[j].size(); k++) {
            rfviews[j][k]->modelData->basevalue = 16;
            rfviews[j][k]->resizeColumnsToContents();
        }
    }
}

void CommandWindow::on_tablebit_clicked() {
    for (int i = 0; i < lmviews.size(); i++) {//loop which changes lmviewdata to bit and sets columnwidth
        lmviews[i]->modelData->basevalue = 2;
        lmviews[i]->resizeColumnsToContents();
    }
    for (int j = 0; j < rfviews.size(); j++) {//loop which changes rfviewdata to bit and sets columnwidth
        for (int k = 0; k < rfviews[j].size(); k++) {
            rfviews[j][k]->modelData->basevalue = 2;
            rfviews[j][k]->resizeColumnsToContents();
        }
    }
}

void CommandWindow::recieveguiwaitpress(bool *is_pressed) {
    emit pauseSim();
    //QMessageBox msgBox;
    //msgBox.setText("Sim_wait_step: press resume Simulation to continue");
    //msgBox.exec();
    auto conn = std::make_shared<QMetaObject::Connection>();
    *conn = connect(ui->pauseButton, &QPushButton::clicked, [is_pressed, conn]() {
        *is_pressed = true;
        disconnect(*conn);
    }); //connect pausebutton to is_pressed variable until clicked once for resuming
}
