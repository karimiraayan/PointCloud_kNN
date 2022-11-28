// ########################################################
// # VPRO instruction & system simulation library         #
// # Sven Gesper, IMS, Uni Hannover, 2019                 #
// ########################################################
// # simulation processes commands. this is the base      #
// ########################################################

#ifndef VPRO_CPP_COMMANDBASE_H
#define VPRO_CPP_COMMANDBASE_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <inttypes.h>
#include <vector>
#include <QString>

class CommandBase {
public:
    enum Type{
        BASE,
        DMA,
        VPRO,
        SIM
    } class_type;

    CommandBase(Type class_type = CommandBase::BASE):
        class_type(class_type)
    {
    };

    virtual bool is_done();

    void print_class_type(FILE * out = stdout);
    virtual void print(FILE * out = stdout) = 0;

    QString get_class_type();
    virtual QString get_type() = 0;
    virtual QString get_string() = 0;
};



#endif //VPRO_CPP_COMMANDBASE_H
