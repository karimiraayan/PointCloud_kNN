//
// Created by gesper on 27.06.19.
//

#ifndef VPRO_TUTORIAL_CPP_VECTORMAIN_H
#define VPRO_TUTORIAL_CPP_VECTORMAIN_H

#include <QObject>
#include <QDebug>

class VectorMain : public QObject{

    Q_OBJECT
public:
    VectorMain(int (*main_fkt)(int, char**), int &argc, char **argv);

public slots:
    int doWork();

private:
    int (*main_fkt)(int, char**);
    int &argc;
    char** argv;
};

#endif //VPRO_TUTORIAL_CPP_VECTORMAIN_H
