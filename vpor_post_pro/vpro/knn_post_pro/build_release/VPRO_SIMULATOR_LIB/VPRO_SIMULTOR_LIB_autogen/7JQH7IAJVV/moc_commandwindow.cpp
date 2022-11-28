/****************************************************************************
** Meta object code from reading C++ file 'commandwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../../isa_intrinsic_lib/simulator/windows/Commands/commandwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'commandwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CommandWindow_t {
    QByteArrayData data[41];
    char stringdata0[599];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CommandWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CommandWindow_t qt_meta_stringdata_CommandWindow = {
    {
QT_MOC_LITERAL(0, 0, 13), // "CommandWindow"
QT_MOC_LITERAL(1, 14, 8), // "pauseSim"
QT_MOC_LITERAL(2, 23, 0), // ""
QT_MOC_LITERAL(3, 24, 8), // "runSteps"
QT_MOC_LITERAL(4, 33, 16), // "dumpRegisterFile"
QT_MOC_LITERAL(5, 50, 15), // "dumpLocalMemory"
QT_MOC_LITERAL(6, 66, 10), // "laneupdate"
QT_MOC_LITERAL(7, 77, 15), // "savemainclicked"
QT_MOC_LITERAL(8, 93, 12), // "getSimupdate"
QT_MOC_LITERAL(9, 106, 17), // "closeotherwindows"
QT_MOC_LITERAL(10, 124, 10), // "dataUpdate"
QT_MOC_LITERAL(11, 135, 19), // "CommandWindow::Data"
QT_MOC_LITERAL(12, 155, 4), // "data"
QT_MOC_LITERAL(13, 160, 9), // "simUpdate"
QT_MOC_LITERAL(14, 170, 5), // "clock"
QT_MOC_LITERAL(15, 176, 11), // "simIsPaused"
QT_MOC_LITERAL(16, 188, 12), // "simIsResumed"
QT_MOC_LITERAL(17, 201, 13), // "getmainmemory"
QT_MOC_LITERAL(18, 215, 8), // "uint8_t*"
QT_MOC_LITERAL(19, 224, 22), // "on_pauseButton_clicked"
QT_MOC_LITERAL(20, 247, 14), // "setLocalMemory"
QT_MOC_LITERAL(21, 262, 4), // "unit"
QT_MOC_LITERAL(22, 267, 1), // "c"
QT_MOC_LITERAL(23, 269, 17), // "setRegisterMemory"
QT_MOC_LITERAL(24, 287, 4), // "lane"
QT_MOC_LITERAL(25, 292, 3), // "ref"
QT_MOC_LITERAL(26, 296, 1), // "u"
QT_MOC_LITERAL(27, 298, 19), // "recieveguiwaitpress"
QT_MOC_LITERAL(28, 318, 5), // "bool*"
QT_MOC_LITERAL(29, 324, 25), // "on_cycle100button_clicked"
QT_MOC_LITERAL(30, 350, 23), // "on_cycle1button_clicked"
QT_MOC_LITERAL(31, 374, 22), // "on_cycleButton_clicked"
QT_MOC_LITERAL(32, 397, 21), // "on_exitButton_clicked"
QT_MOC_LITERAL(33, 419, 20), // "on_lanecheck_clicked"
QT_MOC_LITERAL(34, 440, 27), // "on_consoledumpcheck_clicked"
QT_MOC_LITERAL(35, 468, 26), // "on_mainmemorycheck_clicked"
QT_MOC_LITERAL(36, 495, 18), // "on_lmcheck_clicked"
QT_MOC_LITERAL(37, 514, 24), // "on_command3check_clicked"
QT_MOC_LITERAL(38, 539, 19), // "on_tableint_clicked"
QT_MOC_LITERAL(39, 559, 19), // "on_tablehex_clicked"
QT_MOC_LITERAL(40, 579, 19) // "on_tablebit_clicked"

    },
    "CommandWindow\0pauseSim\0\0runSteps\0"
    "dumpRegisterFile\0dumpLocalMemory\0"
    "laneupdate\0savemainclicked\0getSimupdate\0"
    "closeotherwindows\0dataUpdate\0"
    "CommandWindow::Data\0data\0simUpdate\0"
    "clock\0simIsPaused\0simIsResumed\0"
    "getmainmemory\0uint8_t*\0on_pauseButton_clicked\0"
    "setLocalMemory\0unit\0c\0setRegisterMemory\0"
    "lane\0ref\0u\0recieveguiwaitpress\0bool*\0"
    "on_cycle100button_clicked\0"
    "on_cycle1button_clicked\0on_cycleButton_clicked\0"
    "on_exitButton_clicked\0on_lanecheck_clicked\0"
    "on_consoledumpcheck_clicked\0"
    "on_mainmemorycheck_clicked\0"
    "on_lmcheck_clicked\0on_command3check_clicked\0"
    "on_tableint_clicked\0on_tablehex_clicked\0"
    "on_tablebit_clicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CommandWindow[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      29,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       8,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,  159,    2, 0x06 /* Public */,
       3,    1,  160,    2, 0x06 /* Public */,
       4,    3,  163,    2, 0x06 /* Public */,
       5,    2,  170,    2, 0x06 /* Public */,
       6,    1,  175,    2, 0x06 /* Public */,
       7,    1,  178,    2, 0x06 /* Public */,
       8,    0,  181,    2, 0x06 /* Public */,
       9,    0,  182,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      10,    1,  183,    2, 0x0a /* Public */,
      13,    1,  186,    2, 0x0a /* Public */,
      15,    0,  189,    2, 0x0a /* Public */,
      16,    0,  190,    2, 0x0a /* Public */,
      17,    1,  191,    2, 0x0a /* Public */,
      19,    0,  194,    2, 0x0a /* Public */,
      20,    3,  195,    2, 0x0a /* Public */,
      23,    4,  202,    2, 0x0a /* Public */,
      27,    1,  211,    2, 0x0a /* Public */,
      29,    0,  214,    2, 0x08 /* Private */,
      30,    0,  215,    2, 0x08 /* Private */,
      31,    0,  216,    2, 0x08 /* Private */,
      32,    0,  217,    2, 0x08 /* Private */,
      33,    0,  218,    2, 0x08 /* Private */,
      34,    0,  219,    2, 0x08 /* Private */,
      35,    0,  220,    2, 0x08 /* Private */,
      36,    0,  221,    2, 0x08 /* Private */,
      37,    0,  222,    2, 0x08 /* Private */,
      38,    0,  223,    2, 0x08 /* Private */,
      39,    0,  224,    2, 0x08 /* Private */,
      40,    0,  225,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int,    2,    2,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    2,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 11,   12,
    QMetaType::Void, QMetaType::Long,   14,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 18,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 18, QMetaType::Int,   21,    2,   22,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 18, QMetaType::Int, QMetaType::Int,   24,   25,   22,   26,
    QMetaType::Void, 0x80000000 | 28,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void CommandWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CommandWindow *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->pauseSim(); break;
        case 1: _t->runSteps((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->dumpRegisterFile((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 3: _t->dumpLocalMemory((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 4: _t->laneupdate((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->savemainclicked((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->getSimupdate(); break;
        case 7: _t->closeotherwindows(); break;
        case 8: _t->dataUpdate((*reinterpret_cast< CommandWindow::Data(*)>(_a[1]))); break;
        case 9: _t->simUpdate((*reinterpret_cast< long(*)>(_a[1]))); break;
        case 10: _t->simIsPaused(); break;
        case 11: _t->simIsResumed(); break;
        case 12: _t->getmainmemory((*reinterpret_cast< uint8_t*(*)>(_a[1]))); break;
        case 13: _t->on_pauseButton_clicked(); break;
        case 14: _t->setLocalMemory((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< uint8_t*(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 15: _t->setRegisterMemory((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< uint8_t*(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4]))); break;
        case 16: _t->recieveguiwaitpress((*reinterpret_cast< bool*(*)>(_a[1]))); break;
        case 17: _t->on_cycle100button_clicked(); break;
        case 18: _t->on_cycle1button_clicked(); break;
        case 19: _t->on_cycleButton_clicked(); break;
        case 20: _t->on_exitButton_clicked(); break;
        case 21: _t->on_lanecheck_clicked(); break;
        case 22: _t->on_consoledumpcheck_clicked(); break;
        case 23: _t->on_mainmemorycheck_clicked(); break;
        case 24: _t->on_lmcheck_clicked(); break;
        case 25: _t->on_command3check_clicked(); break;
        case 26: _t->on_tableint_clicked(); break;
        case 27: _t->on_tablehex_clicked(); break;
        case 28: _t->on_tablebit_clicked(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 8:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< CommandWindow::Data >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (CommandWindow::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CommandWindow::pauseSim)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (CommandWindow::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CommandWindow::runSteps)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (CommandWindow::*)(int , int , int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CommandWindow::dumpRegisterFile)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (CommandWindow::*)(int , int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CommandWindow::dumpLocalMemory)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (CommandWindow::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CommandWindow::laneupdate)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (CommandWindow::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CommandWindow::savemainclicked)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (CommandWindow::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CommandWindow::getSimupdate)) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (CommandWindow::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CommandWindow::closeotherwindows)) {
                *result = 7;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CommandWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_CommandWindow.data,
    qt_meta_data_CommandWindow,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CommandWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CommandWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CommandWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int CommandWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
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
void CommandWindow::pauseSim()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void CommandWindow::runSteps(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void CommandWindow::dumpRegisterFile(int _t1, int _t2, int _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void CommandWindow::dumpLocalMemory(int _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void CommandWindow::laneupdate(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void CommandWindow::savemainclicked(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void CommandWindow::getSimupdate()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void CommandWindow::closeotherwindows()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
