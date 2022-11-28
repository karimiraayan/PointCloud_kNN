#include "siminitfunctions.h"

QVBoxLayout* createlaneprogressbars(int cluster_cnt,int unit_cnt,int lane_cnt,QLabel *label,QGroupBox *radiobuttongroup){
    QTabWidget *Laneutilizationgrouptab = new QTabWidget;
    QVBoxLayout *lanetablayout= new QVBoxLayout;
    for (int c = 0 ; c < cluster_cnt ; c++){
        QVBoxLayout *Laneutilizationgroup = new QVBoxLayout;
        auto *w = new QWidget;
        QScrollArea *lanescrollarea = new QScrollArea;
        lanescrollarea->setWidgetResizable(true);
        for (int u = 0 ; u < unit_cnt ; u++){
            for (int l = 0 ; l < lane_cnt ; l++){
                    QProgressBar *lane = new QProgressBar;
                    QLabel *lanelabel = new QLabel;
                    lanelabel->setText("Unit "+QString::number(u)+" Lane "+QString::number(l));
                    Laneutilizationgroup->addWidget(lanelabel);
                    Laneutilizationgroup->addWidget(lane);
             }
            w->setLayout(Laneutilizationgroup);

            lanescrollarea->setWidget(w);
            Laneutilizationgrouptab->addTab(lanescrollarea,"Cluster "+QString::number(c));
            }
        }
    lanetablayout->addWidget(Laneutilizationgrouptab);
    lanetablayout->addWidget(label);
    lanetablayout->addWidget(radiobuttongroup);
    return lanetablayout;
}

std::tuple <QVector<QLineEdit*>,QVector<QLineEdit*>> createmainmemorydumpbuttons(bool *paused, QVector<QLineEdit*> offsets,QVector<QLineEdit*> sizes,CommandWindow* Window,QGridLayout *mainbuttons){
    for(int i=0; i<4; i++){
        QLabel *offsetlabel = new QLabel;
        offsetlabel->setText("Offset");
        QLineEdit *offsetlineedit = new QLineEdit;
        offsets.append(offsetlineedit);
        QLabel *sizelabel= new QLabel;
        sizelabel->setText("Size");
        QLineEdit *sizelineedit = new QLineEdit;
        sizelabel->setAlignment(Qt::AlignRight);
        sizes.append(sizelineedit);
        QPushButton *savemainbutton =new QPushButton;
        savemainbutton->setText("Save Mainmemory");
        QObject::connect(savemainbutton,&QPushButton::clicked,[Window,i,offsets,sizes,paused](){savemaintofile(i, Window, offsets, sizes,paused);});
        mainbuttons->addWidget(offsetlabel,i,0);
        mainbuttons->addWidget(offsetlineedit,i,1);
        mainbuttons->addWidget(sizelabel,i,2);
        mainbuttons->addWidget(sizelineedit,i,3);
        mainbuttons->addWidget(savemainbutton,i,4);
    }

    return std::make_tuple(offsets,sizes);
}

QVBoxLayout* createlmdumpbuttons(CommandWindow* Window, int cluster_cnt, int unit_cnt){
    QVBoxLayout *lmLayout = new QVBoxLayout;
    for (int c = 0 ; c < cluster_cnt ; c++){
        for (int u = 0 ; u < unit_cnt ; u++){
            QPushButton *lm = new QPushButton("Print LM Content ("+QString::number(c)+", "+QString::number(u)+")");
            lmLayout->addWidget(lm);
            QObject::connect(lm, &QPushButton::clicked, [Window, c, u]() {
                    emit Window->dumpLocalMemory(c, u);
                });
            if ( u+1 < unit_cnt || c+1 < cluster_cnt ){
                auto line = new QFrame();
                line->setFrameShape(QFrame::HLine);
                line->setFrameShadow(QFrame::Sunken);
                lmLayout->addWidget(line);

            }
        }
    }
    return lmLayout;
}

QVBoxLayout* createregisterdumpbuttons(CommandWindow* Window, int cluster_cnt, int unit_cnt,int lane_cnt){
    QVBoxLayout *rfLayout = new QVBoxLayout;
    for (int c = 0 ; c < cluster_cnt ; c++){
        for (int u = 0 ; u < unit_cnt ; u++){
            for (int l = 0 ; l < lane_cnt ; l++){
                QPushButton *rf = new QPushButton("Print Reg Content ("+QString::number(c)+", "+QString::number(u)+", "+QString::number(l)+")");
                rfLayout->addWidget(rf);
                QObject::connect(rf, &QPushButton::clicked, [Window, c, u, l]() {
                        emit Window->dumpRegisterFile(c, u, l);
                    });
            }
            if ( u+1 < unit_cnt || c+1 < cluster_cnt ){
                auto line2 = new QFrame();
                line2->setFrameShape(QFrame::HLine);
                line2->setFrameShadow(QFrame::Sunken);
                rfLayout->addWidget(line2);
            }
        }
    }
    return rfLayout;
}



QWidget* creatememorytable(int unit_cnt,int lane_cnt){
    QScrollArea *lmscrollarea = new QScrollArea;
    QWidget *lm_widget_contents = new QWidget;
    QComboBox *lmselect = new QComboBox;
    QStackedWidget *rmlayout = new QStackedWidget;
    for (int i = 0; i < unit_cnt; i++) {
        lmselect->addItem("Unit " + QString::number(i));
        rmlayout->addWidget(createregistertable(lane_cnt));
    }
    LmTableView *lmview = new LmTableView;
    QObject::connect(lmselect, QOverload<int>::of(&QComboBox::activated),
            [lmview](int index){lmview->selectLocalMemory(index); });
    QObject::connect(lmselect, QOverload<int>::of(&QComboBox::activated),
            [rmlayout](int index){rmlayout->setCurrentIndex(index); });
    QHBoxLayout *lmlayout = new QHBoxLayout;
    lmlayout->addWidget(lmselect);
    lmscrollarea->setWidget(lmview);
    lmscrollarea->setWidgetResizable(true);
    lmlayout->addWidget(lmscrollarea);
    lmlayout->addWidget(rmlayout);
    lm_widget_contents->setLayout(lmlayout);
    return lm_widget_contents;
}

QWidget* createregistertable(int lane_cnt){
    RfTableView *rmview = new RfTableView;
    QComboBox *rmselect = new QComboBox;
    QLabel *registerlabel = new QLabel;
    registerlabel->setText("Registerfiledata:");
    QObject::connect(rmselect, QOverload<int>::of(&QComboBox::activated),
                     [rmview](int index){rmview->selectRegisterFile(index); });
    for(int l=0;l<lane_cnt;l++){
        rmselect->addItem("Lane "+ QString::number(l));
    }
    QGridLayout *rmglayout = new QGridLayout;
    QWidget *rmwidget = new QWidget;
    rmglayout->addWidget(registerlabel,0,0);
    rmglayout->addWidget(rmselect,1,0);
    rmglayout->addWidget(rmview,1,1);
    rmwidget->setLayout(rmglayout);

    return rmwidget;
}

void createdebugoptions(QWidget *parent){
    QVBoxLayout *debugoptionslayout= new QVBoxLayout(parent);
    for (int i = 0; i < log(float(DebugOptions::end))/log(2.); i++) { //loops trough debugoption creating corresponding checkboxes and connection to the debugoptions enum
        auto option = DebugOptions(1 << i);
        QString option_text = debugToText(option);
        if (option_text != "end") {
            QCheckBox *optionbox = new QCheckBox;
            optionbox->setText(option_text);
            QObject::connect(optionbox, QOverload<bool>::of(&QCheckBox::clicked), [optionbox, option](bool checked) {
                if (checked) {
                    debug = debug | option;
                } else if (!checked) {
                    debug = debug & ~option;
                }
            });
            debugoptionslayout->addWidget(optionbox);
        }
    }
}
