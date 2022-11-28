
QT       += core gui widgets

TARGET = Commands
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += "SKIP_SIMSTAT_CORE=1"

CONFIG += c++17

SOURCES += \
    main.cpp \
    commandwindow.cpp \
    ../../../simulator/helper/debugHelper.cpp \
    ../../../model/commands/CommandBase.cpp \
    ../../../model/commands/CommandVPRO.cpp \
    ../../../model/commands/CommandDMA.cpp \
    ../../../model/commands/CommandSim.cpp \
    ../../../model/architecture/stats/LaneStat.cpp \
    ../../../model/architecture/stats/DMAStat.cpp \
    ../../../model/architecture/stats/UnitStat.cpp \
    ../../../model/architecture/stats/SimStat.cpp \
    lmtableview.cpp \
    commandstablewidget.cpp \
    savesettings.cpp \
    savemain.cpp \
    siminitfunctions.cpp \
    simupdatefunctions.cpp \
    dataupdatefunctions.cpp \
    rftableview.cpp

HEADERS += \
    commandwindow.h \
    ../../../simulator/helper/debugHelper.h \
    ../../../simulator/helper/structTypes.hpp \
    ../../../simulator/setting.h \
    ../../../isa_intrinsic_aux_lib.h \
    ../../../model/commands/CommandBase.h \
    ../../../model/commands/CommandVPRO.h \
    ../../../model/commands/CommandDMA.h \
    ../../../model/commands/CommandSim.h \
    ../../../model/architecture/stats/LaneStat.h \
    ../../../isa_intrinsic_aux_lib.h \
    lmtableview.h \
    commandstablewidget.h \
    savesettings.h \
    savemain.h \
    siminitfunctions.h \
    simupdatefunctions.h \
    dataupdatefunctions.h \
    rftableview.h


FORMS += \
    commandwindow.ui \
    commandstablewidget.ui


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
