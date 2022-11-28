//
// Created by gesper on 2/5/21.
//

#ifndef HW_VERIFICATION_APP_VERSION_0_0_CONSOLEPRINTER_H
#define HW_VERIFICATION_APP_VERSION_0_0_CONSOLEPRINTER_H


#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <inttypes.h>
#include <vector>
#include <sys/ioctl.h>    /* ioctl, TIOCGWINSZ */
#include <err.h>    /* err */
#include <fcntl.h>    /* open */
#include <stdio.h>    /* printf */
#include <utility>
#include <iostream>
#include <unistd.h> // for STDOUT_FILENO
#include <iomanip>

#include <QMutex>
#include <QThread>
#include <QString>
#include <QCoreApplication>
#include "TimePrinter.h"

class ConsolePrinter : public QThread {
Q_OBJECT

public:
    ConsolePrinter() :
        currentLineBuffer_left(""),
        currentLineBuffer_right("")
    {
        // Disable buffering on std out
        setbuf(stdout, NULL);
        std::cout.width(terminal_cols());
//        auto tt = new TimePrinter(*this, this);
//        tt->start();
//        QCoreApplication::connect(this, SIGNAL(finished()), tt, SLOT(quit()));
//        QCoreApplication::connect(this, SIGNAL(destroyed()), tt, SLOT(quit()));
    }

    static void customHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
    {
        switch (type) {
            case QtInfoMsg:
            case QtDebugMsg:
            case QtWarningMsg:
                printf("%s", msg.toStdString().c_str());
                fflush(stdout);
                break;
            case QtCriticalMsg:
            case QtFatalMsg:
                fprintf(stderr, "%s", msg.toStdString().c_str());
                fflush(stderr);
                break;
        }
    }

protected:
    void run()
    override;

// clear whole terminal screen
    static void cls(void) {
        printf("\33[2J");
    }

    static winsize win_size() {
        struct winsize ws{};
        int fd;

        /* Open the controlling terminal. */
        fd = open("/dev/tty", O_RDWR);
        if (fd < 0)
            err(1, "/dev/tty");

        /* Get window size of terminal. */
        if (ioctl(fd, TIOCGWINSZ, &ws) < 0)
            err(1, "/dev/tty");

        close(fd);
        return ws;
        //printf("%d rows by %d columns\n", ws.ws_row, ws.ws_col);
    }

    static int terminal_rows() {
        return win_size().ws_row;
    }

    static int terminal_cols() {
        return win_size().ws_col;
    }

//    std::stringstream ss;
//    ss << RED << "[MIPS Debug Addr] " << std::dec << tmp << " | 0x" << std::hex << tmp << RESET_COLOR;
//    std::cout << std::right << ss.str();
//    std::cout << '\r' << std::left;

    static constexpr int MAXSTRINGSIZE = 512;
    char buff[MAXSTRINGSIZE]{};
    char dest[MAXSTRINGSIZE]{};

    QString currentLineBuffer_left;
    QString currentLineBuffer_right;
    QString last_temp_string;
public:
    int printf_normal(const char *format, ...);

    int printf_warning(const char *format, ...);

    int printf_error(const char *format, ...);

    int printf_info(const char *format, ...);

    int printf_success(const char *format, ...);

    int printf_normal_(QString string, bool left = true);

    int printf_warning_(QString string, bool left = true);

    int printf_error_(QString string, bool left = true);

    int printf_info_(QString string, bool left = true);

    int printf_success_(QString string, bool left = true);

    int output(QString string, bool left = true, bool temp = false);
};


#endif //HW_VERIFICATION_APP_VERSION_0_0_CONSOLEPRINTER_H
