/****************************************************************************
** Meta object code from reading C++ file 'simCore.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../../isa_intrinsic_lib/simulator/simCore.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QVector>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'simCore.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_SimCore_t {
    QByteArrayData data[43];
    char stringdata0[499];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SimCore_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SimCore_t qt_meta_stringdata_SimCore = {
    {
QT_MOC_LITERAL(0, 0, 7), // "SimCore"
QT_MOC_LITERAL(1, 8, 9), // "simFinish"
QT_MOC_LITERAL(2, 18, 0), // ""
QT_MOC_LITERAL(3, 19, 10), // "dataUpdate"
QT_MOC_LITERAL(4, 30, 19), // "CommandWindow::Data"
QT_MOC_LITERAL(5, 50, 7), // "simInit"
QT_MOC_LITERAL(6, 58, 11), // "cluster_cnt"
QT_MOC_LITERAL(7, 70, 8), // "unit_cnt"
QT_MOC_LITERAL(8, 79, 8), // "lane_cnt"
QT_MOC_LITERAL(9, 88, 9), // "simUpdate"
QT_MOC_LITERAL(10, 98, 5), // "clock"
QT_MOC_LITERAL(11, 104, 11), // "simIsPaused"
QT_MOC_LITERAL(12, 116, 12), // "simIsResumed"
QT_MOC_LITERAL(13, 129, 14), // "sendmainmemory"
QT_MOC_LITERAL(14, 144, 8), // "uint8_t*"
QT_MOC_LITERAL(15, 153, 12), // "sendRegister"
QT_MOC_LITERAL(16, 166, 13), // "QVector<bool>"
QT_MOC_LITERAL(17, 180, 15), // "sendLocalmemory"
QT_MOC_LITERAL(18, 196, 19), // "sendGuiwaitkeypress"
QT_MOC_LITERAL(19, 216, 5), // "bool*"
QT_MOC_LITERAL(20, 222, 7), // "runMain"
QT_MOC_LITERAL(21, 230, 27), // "performanceMeasurement_stop"
QT_MOC_LITERAL(22, 258, 28), // "performanceMeasurement_start"
QT_MOC_LITERAL(23, 287, 15), // "printSimResults"
QT_MOC_LITERAL(24, 303, 6), // "toFile"
QT_MOC_LITERAL(25, 310, 13), // "printSimStats"
QT_MOC_LITERAL(26, 324, 8), // "pauseSim"
QT_MOC_LITERAL(27, 333, 7), // "quitSim"
QT_MOC_LITERAL(28, 341, 8), // "runSteps"
QT_MOC_LITERAL(29, 350, 5), // "steps"
QT_MOC_LITERAL(30, 356, 3), // "run"
QT_MOC_LITERAL(31, 360, 19), // "runUntilReadyForCmd"
QT_MOC_LITERAL(32, 380, 23), // "runUntilMIPSReadyForCmd"
QT_MOC_LITERAL(33, 404, 19), // "visualizeDataUpdate"
QT_MOC_LITERAL(34, 424, 6), // "dumpLM"
QT_MOC_LITERAL(35, 431, 1), // "c"
QT_MOC_LITERAL(36, 433, 1), // "u"
QT_MOC_LITERAL(37, 435, 6), // "dumpRF"
QT_MOC_LITERAL(38, 442, 1), // "l"
QT_MOC_LITERAL(39, 444, 5), // "dumpQ"
QT_MOC_LITERAL(40, 450, 13), // "sendSimUpdate"
QT_MOC_LITERAL(41, 464, 8), // "sim_stop"
QT_MOC_LITERAL(42, 473, 25) // "performanceMeasureTimeout"

    },
    "SimCore\0simFinish\0\0dataUpdate\0"
    "CommandWindow::Data\0simInit\0cluster_cnt\0"
    "unit_cnt\0lane_cnt\0simUpdate\0clock\0"
    "simIsPaused\0simIsResumed\0sendmainmemory\0"
    "uint8_t*\0sendRegister\0QVector<bool>\0"
    "sendLocalmemory\0sendGuiwaitkeypress\0"
    "bool*\0runMain\0performanceMeasurement_stop\0"
    "performanceMeasurement_start\0"
    "printSimResults\0toFile\0printSimStats\0"
    "pauseSim\0quitSim\0runSteps\0steps\0run\0"
    "runUntilReadyForCmd\0runUntilMIPSReadyForCmd\0"
    "visualizeDataUpdate\0dumpLM\0c\0u\0dumpRF\0"
    "l\0dumpQ\0sendSimUpdate\0sim_stop\0"
    "performanceMeasureTimeout"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SimCore[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      29,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      13,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,  159,    2, 0x06 /* Public */,
       3,    1,  160,    2, 0x06 /* Public */,
       5,    3,  163,    2, 0x06 /* Public */,
       9,    1,  170,    2, 0x06 /* Public */,
      11,    0,  173,    2, 0x06 /* Public */,
      12,    0,  174,    2, 0x06 /* Public */,
      13,    1,  175,    2, 0x06 /* Public */,
      15,    2,  178,    2, 0x06 /* Public */,
      17,    1,  183,    2, 0x06 /* Public */,
      18,    1,  186,    2, 0x06 /* Public */,
      20,    0,  189,    2, 0x06 /* Public */,
      21,    0,  190,    2, 0x06 /* Public */,
      22,    1,  191,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      23,    1,  194,    2, 0x0a /* Public */,
      23,    0,  197,    2, 0x2a /* Public | MethodCloned */,
      25,    0,  198,    2, 0x0a /* Public */,
      26,    0,  199,    2, 0x0a /* Public */,
      27,    0,  200,    2, 0x0a /* Public */,
      28,    1,  201,    2, 0x0a /* Public */,
      30,    0,  204,    2, 0x0a /* Public */,
      31,    0,  205,    2, 0x0a /* Public */,
      32,    0,  206,    2, 0x0a /* Public */,
      33,    0,  207,    2, 0x0a /* Public */,
      34,    2,  208,    2, 0x0a /* Public */,
      37,    3,  213,    2, 0x0a /* Public */,
      39,    2,  220,    2, 0x0a /* Public */,
      40,    0,  225,    2, 0x0a /* Public */,
      41,    0,  226,    2, 0x0a /* Public */,
      42,    0,  227,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int,    6,    7,    8,
    QMetaType::Void, QMetaType::Long,   10,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 14,    2,
    QMetaType::Void, 0x80000000 | 14, 0x80000000 | 16,    2,    2,
    QMetaType::Void, 0x80000000 | 14,    2,
    QMetaType::Void, 0x80000000 | 19,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,   24,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   29,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   35,   36,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int,   35,   36,   38,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   35,   36,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void SimCore::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<SimCore *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->simFinish(); break;
        case 1: _t->dataUpdate((*reinterpret_cast< CommandWindow::Data(*)>(_a[1]))); break;
        case 2: _t->simInit((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 3: _t->simUpdate((*reinterpret_cast< long(*)>(_a[1]))); break;
        case 4: _t->simIsPaused(); break;
        case 5: _t->simIsResumed(); break;
        case 6: _t->sendmainmemory((*reinterpret_cast< uint8_t*(*)>(_a[1]))); break;
        case 7: _t->sendRegister((*reinterpret_cast< uint8_t*(*)>(_a[1])),(*reinterpret_cast< QVector<bool>(*)>(_a[2]))); break;
        case 8: _t->sendLocalmemory((*reinterpret_cast< uint8_t*(*)>(_a[1]))); break;
        case 9: _t->sendGuiwaitkeypress((*reinterpret_cast< bool*(*)>(_a[1]))); break;
        case 10: _t->runMain(); break;
        case 11: _t->performanceMeasurement_stop(); break;
        case 12: _t->performanceMeasurement_start((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 13: _t->printSimResults((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 14: _t->printSimResults(); break;
        case 15: _t->printSimStats(); break;
        case 16: _t->pauseSim(); break;
        case 17: _t->quitSim(); break;
        case 18: _t->runSteps((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 19: _t->run(); break;
        case 20: _t->runUntilReadyForCmd(); break;
        case 21: _t->runUntilMIPSReadyForCmd(); break;
        case 22: _t->visualizeDataUpdate(); break;
        case 23: _t->dumpLM((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 24: _t->dumpRF((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 25: _t->dumpQ((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 26: _t->sendSimUpdate(); break;
        case 27: _t->sim_stop(); break;
        case 28: _t->performanceMeasureTimeout(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< CommandWindow::Data >(); break;
            }
            break;
        case 7:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QVector<bool> >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (SimCore::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SimCore::simFinish)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (SimCore::*)(CommandWindow::Data );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SimCore::dataUpdate)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (SimCore::*)(int , int , int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SimCore::simInit)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (SimCore::*)(long );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SimCore::simUpdate)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (SimCore::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SimCore::simIsPaused)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (SimCore::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SimCore::simIsResumed)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (SimCore::*)(uint8_t * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SimCore::sendmainmemory)) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (SimCore::*)(uint8_t * , QVector<bool> );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SimCore::sendRegister)) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (SimCore::*)(uint8_t * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SimCore::sendLocalmemory)) {
                *result = 8;
                return;
            }
        }
        {
            using _t = void (SimCore::*)(bool * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SimCore::sendGuiwaitkeypress)) {
                *result = 9;
                return;
            }
        }
        {
            using _t = void (SimCore::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SimCore::runMain)) {
                *result = 10;
                return;
            }
        }
        {
            using _t = void (SimCore::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SimCore::performanceMeasurement_stop)) {
                *result = 11;
                return;
            }
        }
        {
            using _t = void (SimCore::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SimCore::performanceMeasurement_start)) {
                *result = 12;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject SimCore::staticMetaObject = { {
    QMetaObject::SuperData::link<QThread::staticMetaObject>(),
    qt_meta_stringdata_SimCore.data,
    qt_meta_data_SimCore,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *SimCore::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SimCore::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SimCore.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "Core"))
        return static_cast< Core*>(this);
    return QThread::qt_metacast(_clname);
}

int SimCore::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 29)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 29;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 29)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 29;
    }
    return _id;
}

// SIGNAL 0
void SimCore::simFinish()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void SimCore::dataUpdate(CommandWindow::Data _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void SimCore::simInit(int _t1, int _t2, int _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void SimCore::simUpdate(long _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void SimCore::simIsPaused()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void SimCore::simIsResumed()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void SimCore::sendmainmemory(uint8_t * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void SimCore::sendRegister(uint8_t * _t1, QVector<bool> _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void SimCore::sendLocalmemory(uint8_t * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void SimCore::sendGuiwaitkeypress(bool * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}

// SIGNAL 10
void SimCore::runMain()
{
    QMetaObject::activate(this, &staticMetaObject, 10, nullptr);
}

// SIGNAL 11
void SimCore::performanceMeasurement_stop()
{
    QMetaObject::activate(this, &staticMetaObject, 11, nullptr);
}

// SIGNAL 12
void SimCore::performanceMeasurement_start(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 12, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
