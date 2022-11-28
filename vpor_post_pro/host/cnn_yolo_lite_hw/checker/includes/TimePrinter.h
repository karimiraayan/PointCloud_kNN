//
// Created by gesper on 2/8/21.
//

#ifndef HW_VERIFICATION_APP_VERSION_0_0_TIMEPRINTER_H
#define HW_VERIFICATION_APP_VERSION_0_0_TIMEPRINTER_H

#include <QThread>
#include <QString>

class ConsolePrinter;

class TimePrinter : public QThread{
    Q_OBJECT
public:
    explicit TimePrinter(ConsolePrinter &console, QThread *parent = nullptr) : console(console), QThread(parent) {
    };

protected:
    void run() override;

private:
    ConsolePrinter &console;
};


#endif //HW_VERIFICATION_APP_VERSION_0_0_TIMEPRINTER_H
