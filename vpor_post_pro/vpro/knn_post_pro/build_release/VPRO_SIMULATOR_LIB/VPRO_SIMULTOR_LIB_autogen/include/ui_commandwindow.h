/********************************************************************************
** Form generated from reading UI file 'commandwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_COMMANDWINDOW_H
#define UI_COMMANDWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CommandWindow
{
public:
    QWidget *centralWidget;
    QGridLayout *gridLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *cycleButton;
    QPushButton *pauseButton;
    QSpinBox *cycleInput;
    QPushButton *exitButton;
    QPushButton *cycle100button;
    QWidget *widget_2;
    QVBoxLayout *verticalLayout_5;
    QSplitter *splitter_2;
    QTabWidget *tabwidget;
    QWidget *Lanestab;
    QLabel *Labellastcycles;
    QGroupBox *radiobuttongroup;
    QHBoxLayout *radiobuttonlayout;
    QRadioButton *radioButton_4;
    QRadioButton *radioButton_5;
    QRadioButton *radioButton_3;
    QRadioButton *radioButton_2;
    QRadioButton *radioButton;
    QRadioButton *radioButton_6;
    QWidget *tab_statistic;
    QGridLayout *gridLayout_6;
    QScrollArea *tab_statistic_content;
    QWidget *statistic_scrollAreaWidgetContent;
    QVBoxLayout *verticalLayout_3;
    QTextBrowser *statistic_textbrowser;
    QWidget *consoledumptab;
    QGridLayout *gridLayout_4;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QGridLayout *gridLayout_5;
    QGroupBox *rfdump;
    QSpacerItem *verticalSpacer;
    QSpacerItem *horizontalSpacer_2;
    QSpacerItem *verticalSpacer_2;
    QSpacerItem *horizontalSpacer_3;
    QGroupBox *lmdump;
    QWidget *mainmemorydumptab;
    QHBoxLayout *horizontalLayout_4;
    QGroupBox *mainmembuttongroup;
    QWidget *tab_localMemory;
    QGridLayout *gridLayout_3;
    QTabWidget *lmtabs;
    QWidget *settingstab;
    QGridLayout *horizontalLayout_3;
    QHBoxLayout *horizontalLayout_41;
    QGroupBox *settingsbox;
    QVBoxLayout *verticalLayout_4;
    QCheckBox *lanecheck;
    QCheckBox *consoledumpcheck;
    QCheckBox *mainmemorycheck;
    QCheckBox *lmcheck;
    QCheckBox *command3check;
    QSpacerItem *verticalSpacer_4;
    QGroupBox *tablebasesetter;
    QVBoxLayout *verticalLayout_2;
    QRadioButton *tablehex;
    QRadioButton *tableint;
    QRadioButton *tablebit;
    QSpacerItem *verticalSpacer_3;
    QGroupBox *Debugsettings;
    QHBoxLayout *horizontalLayout;
    QScrollArea *scrollArea_3;
    QWidget *scrollAreaWidgetContents_3;
    QVBoxLayout *verticalLayout;
    QScrollArea *scrollArea_2;
    QWidget *scrollAreaWidgetContents_2;
    QGridLayout *gridLayout_2;
    QLabel *cycle_label;
    QLabel *sim_status_label;
    QPushButton *cycle1button;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *CommandWindow)
    {
        if (CommandWindow->objectName().isEmpty())
            CommandWindow->setObjectName(QString::fromUtf8("CommandWindow"));
        CommandWindow->resize(936, 777);
        centralWidget = new QWidget(CommandWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        gridLayout = new QGridLayout(centralWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer, 2, 2, 1, 1);

        cycleButton = new QPushButton(centralWidget);
        cycleButton->setObjectName(QString::fromUtf8("cycleButton"));

        gridLayout->addWidget(cycleButton, 2, 1, 1, 1);

        pauseButton = new QPushButton(centralWidget);
        pauseButton->setObjectName(QString::fromUtf8("pauseButton"));

        gridLayout->addWidget(pauseButton, 3, 3, 1, 1);

        cycleInput = new QSpinBox(centralWidget);
        cycleInput->setObjectName(QString::fromUtf8("cycleInput"));
        cycleInput->setMaximum(10000000);
        cycleInput->setValue(1000);

        gridLayout->addWidget(cycleInput, 2, 0, 1, 1);

        exitButton = new QPushButton(centralWidget);
        exitButton->setObjectName(QString::fromUtf8("exitButton"));

        gridLayout->addWidget(exitButton, 3, 4, 1, 1);

        cycle100button = new QPushButton(centralWidget);
        cycle100button->setObjectName(QString::fromUtf8("cycle100button"));

        gridLayout->addWidget(cycle100button, 3, 1, 1, 1);

        widget_2 = new QWidget(centralWidget);
        widget_2->setObjectName(QString::fromUtf8("widget_2"));
        verticalLayout_5 = new QVBoxLayout(widget_2);
        verticalLayout_5->setSpacing(0);
        verticalLayout_5->setContentsMargins(11, 11, 11, 11);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        verticalLayout_5->setContentsMargins(0, 0, 0, 0);
        splitter_2 = new QSplitter(widget_2);
        splitter_2->setObjectName(QString::fromUtf8("splitter_2"));
        splitter_2->setOrientation(Qt::Vertical);
        tabwidget = new QTabWidget(splitter_2);
        tabwidget->setObjectName(QString::fromUtf8("tabwidget"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(tabwidget->sizePolicy().hasHeightForWidth());
        tabwidget->setSizePolicy(sizePolicy);
        tabwidget->setStyleSheet(QString::fromUtf8("QLabel { border-color: black}"));
        Lanestab = new QWidget();
        Lanestab->setObjectName(QString::fromUtf8("Lanestab"));
        Labellastcycles = new QLabel(Lanestab);
        Labellastcycles->setObjectName(QString::fromUtf8("Labellastcycles"));
        Labellastcycles->setGeometry(QRect(0, 340, 431, 17));
        radiobuttongroup = new QGroupBox(Lanestab);
        radiobuttongroup->setObjectName(QString::fromUtf8("radiobuttongroup"));
        radiobuttongroup->setGeometry(QRect(0, 340, 931, 91));
        radiobuttonlayout = new QHBoxLayout(radiobuttongroup);
        radiobuttonlayout->setSpacing(6);
        radiobuttonlayout->setContentsMargins(11, 11, 11, 11);
        radiobuttonlayout->setObjectName(QString::fromUtf8("radiobuttonlayout"));
        radioButton_4 = new QRadioButton(radiobuttongroup);
        radioButton_4->setObjectName(QString::fromUtf8("radioButton_4"));

        radiobuttonlayout->addWidget(radioButton_4);

        radioButton_5 = new QRadioButton(radiobuttongroup);
        radioButton_5->setObjectName(QString::fromUtf8("radioButton_5"));

        radiobuttonlayout->addWidget(radioButton_5);

        radioButton_3 = new QRadioButton(radiobuttongroup);
        radioButton_3->setObjectName(QString::fromUtf8("radioButton_3"));

        radiobuttonlayout->addWidget(radioButton_3);

        radioButton_2 = new QRadioButton(radiobuttongroup);
        radioButton_2->setObjectName(QString::fromUtf8("radioButton_2"));

        radiobuttonlayout->addWidget(radioButton_2);

        radioButton = new QRadioButton(radiobuttongroup);
        radioButton->setObjectName(QString::fromUtf8("radioButton"));

        radiobuttonlayout->addWidget(radioButton);

        radioButton_6 = new QRadioButton(radiobuttongroup);
        radioButton_6->setObjectName(QString::fromUtf8("radioButton_6"));
        radioButton_6->setChecked(true);

        radiobuttonlayout->addWidget(radioButton_6);

        tabwidget->addTab(Lanestab, QString());
        tab_statistic = new QWidget();
        tab_statistic->setObjectName(QString::fromUtf8("tab_statistic"));
        gridLayout_6 = new QGridLayout(tab_statistic);
        gridLayout_6->setSpacing(6);
        gridLayout_6->setContentsMargins(11, 11, 11, 11);
        gridLayout_6->setObjectName(QString::fromUtf8("gridLayout_6"));
        tab_statistic_content = new QScrollArea(tab_statistic);
        tab_statistic_content->setObjectName(QString::fromUtf8("tab_statistic_content"));
        tab_statistic_content->setWidgetResizable(true);
        statistic_scrollAreaWidgetContent = new QWidget();
        statistic_scrollAreaWidgetContent->setObjectName(QString::fromUtf8("statistic_scrollAreaWidgetContent"));
        statistic_scrollAreaWidgetContent->setGeometry(QRect(0, 0, 894, 498));
        verticalLayout_3 = new QVBoxLayout(statistic_scrollAreaWidgetContent);
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setContentsMargins(11, 11, 11, 11);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        statistic_textbrowser = new QTextBrowser(statistic_scrollAreaWidgetContent);
        statistic_textbrowser->setObjectName(QString::fromUtf8("statistic_textbrowser"));

        verticalLayout_3->addWidget(statistic_textbrowser);

        tab_statistic_content->setWidget(statistic_scrollAreaWidgetContent);

        gridLayout_6->addWidget(tab_statistic_content, 0, 0, 1, 1);

        tabwidget->addTab(tab_statistic, QString());
        consoledumptab = new QWidget();
        consoledumptab->setObjectName(QString::fromUtf8("consoledumptab"));
        gridLayout_4 = new QGridLayout(consoledumptab);
        gridLayout_4->setSpacing(0);
        gridLayout_4->setContentsMargins(11, 11, 11, 11);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        gridLayout_4->setContentsMargins(0, 0, 0, 0);
        scrollArea = new QScrollArea(consoledumptab);
        scrollArea->setObjectName(QString::fromUtf8("scrollArea"));
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QString::fromUtf8("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 912, 516));
        gridLayout_5 = new QGridLayout(scrollAreaWidgetContents);
        gridLayout_5->setSpacing(6);
        gridLayout_5->setContentsMargins(11, 11, 11, 11);
        gridLayout_5->setObjectName(QString::fromUtf8("gridLayout_5"));
        rfdump = new QGroupBox(scrollAreaWidgetContents);
        rfdump->setObjectName(QString::fromUtf8("rfdump"));

        gridLayout_5->addWidget(rfdump, 1, 1, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_5->addItem(verticalSpacer, 0, 1, 1, 2);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_5->addItem(horizontalSpacer_2, 1, 0, 1, 1);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_5->addItem(verticalSpacer_2, 3, 1, 1, 2);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_5->addItem(horizontalSpacer_3, 1, 3, 1, 1);

        lmdump = new QGroupBox(scrollAreaWidgetContents);
        lmdump->setObjectName(QString::fromUtf8("lmdump"));

        gridLayout_5->addWidget(lmdump, 1, 2, 1, 1);

        scrollArea->setWidget(scrollAreaWidgetContents);

        gridLayout_4->addWidget(scrollArea, 0, 0, 1, 3);

        tabwidget->addTab(consoledumptab, QString());
        mainmemorydumptab = new QWidget();
        mainmemorydumptab->setObjectName(QString::fromUtf8("mainmemorydumptab"));
        horizontalLayout_4 = new QHBoxLayout(mainmemorydumptab);
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        mainmembuttongroup = new QGroupBox(mainmemorydumptab);
        mainmembuttongroup->setObjectName(QString::fromUtf8("mainmembuttongroup"));

        horizontalLayout_4->addWidget(mainmembuttongroup);

        tabwidget->addTab(mainmemorydumptab, QString());
        tab_localMemory = new QWidget();
        tab_localMemory->setObjectName(QString::fromUtf8("tab_localMemory"));
        gridLayout_3 = new QGridLayout(tab_localMemory);
        gridLayout_3->setSpacing(0);
        gridLayout_3->setContentsMargins(11, 11, 11, 11);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        gridLayout_3->setContentsMargins(0, 0, 0, 0);
        lmtabs = new QTabWidget(tab_localMemory);
        lmtabs->setObjectName(QString::fromUtf8("lmtabs"));

        gridLayout_3->addWidget(lmtabs, 0, 0, 1, 1);

        tabwidget->addTab(tab_localMemory, QString());
        settingstab = new QWidget();
        settingstab->setObjectName(QString::fromUtf8("settingstab"));
        horizontalLayout_3 = new QGridLayout(settingstab);
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        horizontalLayout_41 = new QHBoxLayout();
        horizontalLayout_41->setSpacing(6);
        horizontalLayout_41->setObjectName(QString::fromUtf8("horizontalLayout_41"));
        settingsbox = new QGroupBox(settingstab);
        settingsbox->setObjectName(QString::fromUtf8("settingsbox"));
        verticalLayout_4 = new QVBoxLayout(settingsbox);
        verticalLayout_4->setSpacing(6);
        verticalLayout_4->setContentsMargins(11, 11, 11, 11);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        verticalLayout_4->setContentsMargins(-1, 0, -1, -1);
        lanecheck = new QCheckBox(settingsbox);
        lanecheck->setObjectName(QString::fromUtf8("lanecheck"));
        lanecheck->setChecked(true);

        verticalLayout_4->addWidget(lanecheck);

        consoledumpcheck = new QCheckBox(settingsbox);
        consoledumpcheck->setObjectName(QString::fromUtf8("consoledumpcheck"));
        consoledumpcheck->setChecked(true);

        verticalLayout_4->addWidget(consoledumpcheck);

        mainmemorycheck = new QCheckBox(settingsbox);
        mainmemorycheck->setObjectName(QString::fromUtf8("mainmemorycheck"));
        mainmemorycheck->setChecked(true);

        verticalLayout_4->addWidget(mainmemorycheck);

        lmcheck = new QCheckBox(settingsbox);
        lmcheck->setObjectName(QString::fromUtf8("lmcheck"));
        lmcheck->setChecked(true);

        verticalLayout_4->addWidget(lmcheck);

        command3check = new QCheckBox(settingsbox);
        command3check->setObjectName(QString::fromUtf8("command3check"));
        command3check->setChecked(true);

        verticalLayout_4->addWidget(command3check);

        verticalSpacer_4 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_4->addItem(verticalSpacer_4);


        horizontalLayout_41->addWidget(settingsbox);

        tablebasesetter = new QGroupBox(settingstab);
        tablebasesetter->setObjectName(QString::fromUtf8("tablebasesetter"));
        verticalLayout_2 = new QVBoxLayout(tablebasesetter);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        tablehex = new QRadioButton(tablebasesetter);
        tablehex->setObjectName(QString::fromUtf8("tablehex"));
        tablehex->setChecked(true);

        verticalLayout_2->addWidget(tablehex);

        tableint = new QRadioButton(tablebasesetter);
        tableint->setObjectName(QString::fromUtf8("tableint"));

        verticalLayout_2->addWidget(tableint);

        tablebit = new QRadioButton(tablebasesetter);
        tablebit->setObjectName(QString::fromUtf8("tablebit"));

        verticalLayout_2->addWidget(tablebit);

        verticalSpacer_3 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer_3);


        horizontalLayout_41->addWidget(tablebasesetter);

        Debugsettings = new QGroupBox(settingstab);
        Debugsettings->setObjectName(QString::fromUtf8("Debugsettings"));
        horizontalLayout = new QHBoxLayout(Debugsettings);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        scrollArea_3 = new QScrollArea(Debugsettings);
        scrollArea_3->setObjectName(QString::fromUtf8("scrollArea_3"));
        scrollArea_3->setWidgetResizable(true);
        scrollAreaWidgetContents_3 = new QWidget();
        scrollAreaWidgetContents_3->setObjectName(QString::fromUtf8("scrollAreaWidgetContents_3"));
        scrollAreaWidgetContents_3->setGeometry(QRect(0, 0, 461, 452));
        verticalLayout = new QVBoxLayout(scrollAreaWidgetContents_3);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
        scrollArea_3->setWidget(scrollAreaWidgetContents_3);

        horizontalLayout->addWidget(scrollArea_3);


        horizontalLayout_41->addWidget(Debugsettings);


        horizontalLayout_3->addLayout(horizontalLayout_41, 0, 0, 1, 1);

        tabwidget->addTab(settingstab, QString());
        splitter_2->addWidget(tabwidget);
        scrollArea_2 = new QScrollArea(splitter_2);
        scrollArea_2->setObjectName(QString::fromUtf8("scrollArea_2"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(scrollArea_2->sizePolicy().hasHeightForWidth());
        scrollArea_2->setSizePolicy(sizePolicy1);
        scrollArea_2->setWidgetResizable(true);
        scrollAreaWidgetContents_2 = new QWidget();
        scrollAreaWidgetContents_2->setObjectName(QString::fromUtf8("scrollAreaWidgetContents_2"));
        scrollAreaWidgetContents_2->setGeometry(QRect(0, 0, 916, 119));
        gridLayout_2 = new QGridLayout(scrollAreaWidgetContents_2);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        cycle_label = new QLabel(scrollAreaWidgetContents_2);
        cycle_label->setObjectName(QString::fromUtf8("cycle_label"));

        gridLayout_2->addWidget(cycle_label, 9, 0, 1, 1);

        sim_status_label = new QLabel(scrollAreaWidgetContents_2);
        sim_status_label->setObjectName(QString::fromUtf8("sim_status_label"));

        gridLayout_2->addWidget(sim_status_label, 12, 0, 1, 1);

        scrollArea_2->setWidget(scrollAreaWidgetContents_2);
        splitter_2->addWidget(scrollArea_2);

        verticalLayout_5->addWidget(splitter_2);


        gridLayout->addWidget(widget_2, 0, 0, 1, 5);

        cycle1button = new QPushButton(centralWidget);
        cycle1button->setObjectName(QString::fromUtf8("cycle1button"));

        gridLayout->addWidget(cycle1button, 3, 0, 1, 1);

        CommandWindow->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(CommandWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        CommandWindow->setStatusBar(statusBar);

        retranslateUi(CommandWindow);
        QObject::connect(exitButton, SIGNAL(clicked()), CommandWindow, SLOT(close()));

        tabwidget->setCurrentIndex(5);
        lmtabs->setCurrentIndex(-1);


        QMetaObject::connectSlotsByName(CommandWindow);
    } // setupUi

    void retranslateUi(QMainWindow *CommandWindow)
    {
        CommandWindow->setWindowTitle(QCoreApplication::translate("CommandWindow", "CommandWindow", nullptr));
        cycleButton->setText(QCoreApplication::translate("CommandWindow", "+...cycles", nullptr));
        pauseButton->setText(QCoreApplication::translate("CommandWindow", "Start Simulation", nullptr));
        exitButton->setText(QCoreApplication::translate("CommandWindow", "Exit Simulation", nullptr));
        cycle100button->setText(QCoreApplication::translate("CommandWindow", "+100 cycles", nullptr));
#if QT_CONFIG(tooltip)
        tabwidget->setToolTip(QString());
#endif // QT_CONFIG(tooltip)
        Labellastcycles->setText(QCoreApplication::translate("CommandWindow", "Value of last x cycles", nullptr));
        radioButton_4->setText(QCoreApplication::translate("CommandWindow", "Last 10000 cycles", nullptr));
        radioButton_5->setText(QCoreApplication::translate("CommandWindow", "Last 20000 cycles", nullptr));
        radioButton_3->setText(QCoreApplication::translate("CommandWindow", "Last 30000 cycles", nullptr));
        radioButton_2->setText(QCoreApplication::translate("CommandWindow", "Last 40000 cycles", nullptr));
        radioButton->setText(QCoreApplication::translate("CommandWindow", "Last 50000 cycles", nullptr));
        radioButton_6->setText(QCoreApplication::translate("CommandWindow", "Total cylces", nullptr));
        tabwidget->setTabText(tabwidget->indexOf(Lanestab), QCoreApplication::translate("CommandWindow", "Lanedetails", nullptr));
        tabwidget->setTabText(tabwidget->indexOf(tab_statistic), QCoreApplication::translate("CommandWindow", "Statistics", nullptr));
        rfdump->setTitle(QCoreApplication::translate("CommandWindow", "Register File Dump (Cluster, Unit, Lane)", nullptr));
        lmdump->setTitle(QCoreApplication::translate("CommandWindow", "Local Memory Dump (Cluster, Unit)", nullptr));
        tabwidget->setTabText(tabwidget->indexOf(consoledumptab), QCoreApplication::translate("CommandWindow", "Console Dump", nullptr));
        mainmembuttongroup->setTitle(QString());
        tabwidget->setTabText(tabwidget->indexOf(mainmemorydumptab), QCoreApplication::translate("CommandWindow", "Main Memory", nullptr));
        tabwidget->setTabText(tabwidget->indexOf(tab_localMemory), QCoreApplication::translate("CommandWindow", "Local Memory", nullptr));
        settingsbox->setTitle(QCoreApplication::translate("CommandWindow", "Tableenables", nullptr));
        lanecheck->setText(QCoreApplication::translate("CommandWindow", "Laneupdate", nullptr));
        consoledumpcheck->setText(QCoreApplication::translate("CommandWindow", "Console dump", nullptr));
        mainmemorycheck->setText(QCoreApplication::translate("CommandWindow", "Mainmemorydump", nullptr));
        lmcheck->setText(QCoreApplication::translate("CommandWindow", "lm-update", nullptr));
        command3check->setText(QCoreApplication::translate("CommandWindow", "Commandupdate 3", nullptr));
        tablebasesetter->setTitle(QCoreApplication::translate("CommandWindow", "Memorytable-base-settings", nullptr));
        tablehex->setText(QCoreApplication::translate("CommandWindow", "hexadecimal (base 16)", nullptr));
        tableint->setText(QCoreApplication::translate("CommandWindow", "decimal (base 10)", nullptr));
        tablebit->setText(QCoreApplication::translate("CommandWindow", " binary (base 2)", nullptr));
        Debugsettings->setTitle(QCoreApplication::translate("CommandWindow", "Debug-Settings", nullptr));
        tabwidget->setTabText(tabwidget->indexOf(settingstab), QCoreApplication::translate("CommandWindow", "Settings", nullptr));
        cycle_label->setText(QCoreApplication::translate("CommandWindow", "Cycle: -", nullptr));
        sim_status_label->setText(QCoreApplication::translate("CommandWindow", "Sim Status: -", nullptr));
        cycle1button->setText(QCoreApplication::translate("CommandWindow", "+1 cycle", nullptr));
    } // retranslateUi

};

namespace Ui {
    class CommandWindow: public Ui_CommandWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_COMMANDWINDOW_H
