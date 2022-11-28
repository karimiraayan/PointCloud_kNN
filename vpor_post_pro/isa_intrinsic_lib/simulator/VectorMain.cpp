//
// Created by gesper on 27.06.19.
//

#include "VectorMain.h"

VectorMain::VectorMain(int (*main_fkt)(int, char**), int &argc, char** argv):
main_fkt(main_fkt), argc(argc), argv(argv)
{

}
int VectorMain::doWork() {
//    qDebug() << "Call of main(); Simulator Thread! Starting main again (parallel)";
    return main_fkt(argc, argv);
}
