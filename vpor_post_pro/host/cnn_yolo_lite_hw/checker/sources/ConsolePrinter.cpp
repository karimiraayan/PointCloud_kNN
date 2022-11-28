//
// Created by gesper on 2/5/21.
//

#include <QDebug>
#include <ConsolePrinter.h>
#include "../../../isa_intrinsic_lib/simulator/helper/debugHelper.h"

void ConsolePrinter::run() {
    QThread::run();
}


int ConsolePrinter::printf_normal(const char *format, ...) {
    va_list arglist;
    va_start( arglist, format );
    vsprintf(dest, format, arglist);
    va_end( arglist );
    snprintf(buff, MAXSTRINGSIZE, dest, arglist);
    return printf_normal_(QString(buff));
}

int ConsolePrinter::printf_warning(const char *format, ...) {
    va_list arglist;
    va_start( arglist, format );
    vsprintf(dest, format, arglist);
    va_end( arglist );
    snprintf(buff, MAXSTRINGSIZE, dest, arglist);
    return printf_warning_(QString(buff));
}

int ConsolePrinter::printf_error(const char *format, ...) {
    va_list arglist;
    va_start( arglist, format );
    vsprintf(dest, format, arglist);
    va_end( arglist );
    snprintf(buff, MAXSTRINGSIZE, dest, arglist);
    return printf_error_(QString(buff));
}

int ConsolePrinter::printf_info(const char *format, ...) {
    va_list arglist;
    va_start( arglist, format );
    vsprintf(dest, format, arglist);
    va_end( arglist );
    snprintf(buff, MAXSTRINGSIZE, dest, arglist);
    return printf_info_(QString(buff));
}

int ConsolePrinter::printf_success(const char *format, ...) {
    va_list arglist;
    va_start( arglist, format );
    vsprintf(dest, format, arglist);
    va_end( arglist );
    snprintf(buff, MAXSTRINGSIZE, dest, arglist);
    return printf_success_(QString(buff));
}

int ConsolePrinter::printf_normal_(QString string, bool left) {
    return output(string, left);
}

int ConsolePrinter::printf_warning_(QString string, bool left) {
    return output(ORANGE + string + RESET_COLOR, left);
}

int ConsolePrinter::printf_error_(QString string, bool left) {
    return output(RED + string + RESET_COLOR, left);
}

int ConsolePrinter::printf_info_(QString string, bool left) {
    return output(LBLUE + string + RESET_COLOR, left);
}

int ConsolePrinter::printf_success_(QString string, bool left) {
    return output(LGREEN + string + RESET_COLOR, left);
}

// \033[F => move cursor to the beginning of the previous line
// \033[A => move cursor up one line

int ConsolePrinter::output(QString string, bool left, bool temp) {
    if (temp){ // e.g. time visualization
        std::cout  << std::left << string.toStdString() << "\r";
        last_temp_string = string;
        return 0;
    }

    if (left)
        currentLineBuffer_left += string;
    else
        currentLineBuffer_right += string;

    if (string.contains("\n")){
        std::cout.width(terminal_cols());
        std::cout << std::right << currentLineBuffer_right.toStdString();
        std::cout << '\r' << std::left << currentLineBuffer_left.toStdString();
        currentLineBuffer_left.clear();
        currentLineBuffer_right.clear();
    } else if (!left){
        std::cout.width(terminal_cols());
        std::cout << std::right << currentLineBuffer_right.toStdString();
        std::cout << '\r' << std::left << last_temp_string.toStdString();
        if (!currentLineBuffer_right.contains("\n"))
            std::cout << "\n";
        currentLineBuffer_right.clear();
    }


//    if (string.contains("\n")){
//        std::cout.width(terminal_cols());
////            qInfo().nospace().noquote() << currentLineBuffer << string;
////            currentLineBuffer.clear();
//    }
    return 0;
}
