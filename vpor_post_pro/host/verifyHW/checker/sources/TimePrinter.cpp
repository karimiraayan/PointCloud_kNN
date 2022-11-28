//
// Created by gesper on 2/8/21.
//

#include <TimePrinter.h>
#include "ConsolePrinter.h"

#include <chrono>
#include <thread>


void TimePrinter::run(){
    auto t1 = std::chrono::high_resolution_clock::now();
    while(isRunning()){
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        auto t2 = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>( t2 - t1 ).count();

        console.output(QString("Runtime: ")+QString::number(duration)+"s", true, true);
    }
}
