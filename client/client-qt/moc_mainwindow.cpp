/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "mainwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[21];
    char stringdata0[372];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "MainWindow"
QT_MOC_LITERAL(1, 11, 15), // "change_dirLocal"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 17), // "change_dirPriDisk"
QT_MOC_LITERAL(4, 46, 25), // "display_right_click_local"
QT_MOC_LITERAL(5, 72, 3), // "pos"
QT_MOC_LITERAL(6, 76, 27), // "display_right_click_priDisk"
QT_MOC_LITERAL(7, 104, 15), // "show_confDialog"
QT_MOC_LITERAL(8, 120, 19), // "delete_file_priDisk"
QT_MOC_LITERAL(9, 140, 17), // "delete_file_local"
QT_MOC_LITERAL(10, 158, 21), // "create_folder_priDisk"
QT_MOC_LITERAL(11, 180, 19), // "create_folder_local"
QT_MOC_LITERAL(12, 200, 19), // "change_file_visible"
QT_MOC_LITERAL(13, 220, 8), // "selected"
QT_MOC_LITERAL(14, 229, 14), // "switch_connect"
QT_MOC_LITERAL(15, 244, 6), // "turnOn"
QT_MOC_LITERAL(16, 251, 14), // "update_display"
QT_MOC_LITERAL(17, 266, 30), // "on_lineEditLocal_returnPressed"
QT_MOC_LITERAL(18, 297, 32), // "on_lineEditpriDisk_returnPressed"
QT_MOC_LITERAL(19, 330, 21), // "on_BtopriDisk_clicked"
QT_MOC_LITERAL(20, 352, 19) // "on_BtoLocal_clicked"

    },
    "MainWindow\0change_dirLocal\0\0"
    "change_dirPriDisk\0display_right_click_local\0"
    "pos\0display_right_click_priDisk\0"
    "show_confDialog\0delete_file_priDisk\0"
    "delete_file_local\0create_folder_priDisk\0"
    "create_folder_local\0change_file_visible\0"
    "selected\0switch_connect\0turnOn\0"
    "update_display\0on_lineEditLocal_returnPressed\0"
    "on_lineEditpriDisk_returnPressed\0"
    "on_BtopriDisk_clicked\0on_BtoLocal_clicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      16,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   94,    2, 0x08 /* Private */,
       3,    1,   97,    2, 0x08 /* Private */,
       4,    1,  100,    2, 0x08 /* Private */,
       6,    1,  103,    2, 0x08 /* Private */,
       7,    0,  106,    2, 0x08 /* Private */,
       8,    0,  107,    2, 0x08 /* Private */,
       9,    0,  108,    2, 0x08 /* Private */,
      10,    0,  109,    2, 0x08 /* Private */,
      11,    0,  110,    2, 0x08 /* Private */,
      12,    1,  111,    2, 0x08 /* Private */,
      14,    1,  114,    2, 0x08 /* Private */,
      16,    0,  117,    2, 0x08 /* Private */,
      17,    0,  118,    2, 0x08 /* Private */,
      18,    0,  119,    2, 0x08 /* Private */,
      19,    0,  120,    2, 0x08 /* Private */,
      20,    0,  121,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::QModelIndex,    2,
    QMetaType::Void, QMetaType::QModelIndex,    2,
    QMetaType::Void, QMetaType::QPoint,    5,
    QMetaType::Void, QMetaType::QPoint,    5,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   13,
    QMetaType::Void, QMetaType::Bool,   15,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MainWindow *_t = static_cast<MainWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->change_dirLocal((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 1: _t->change_dirPriDisk((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 2: _t->display_right_click_local((*reinterpret_cast< QPoint(*)>(_a[1]))); break;
        case 3: _t->display_right_click_priDisk((*reinterpret_cast< QPoint(*)>(_a[1]))); break;
        case 4: _t->show_confDialog(); break;
        case 5: _t->delete_file_priDisk(); break;
        case 6: _t->delete_file_local(); break;
        case 7: _t->create_folder_priDisk(); break;
        case 8: _t->create_folder_local(); break;
        case 9: _t->change_file_visible((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 10: _t->switch_connect((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 11: _t->update_display(); break;
        case 12: _t->on_lineEditLocal_returnPressed(); break;
        case 13: _t->on_lineEditpriDisk_returnPressed(); break;
        case 14: _t->on_BtopriDisk_clicked(); break;
        case 15: _t->on_BtoLocal_clicked(); break;
        default: ;
        }
    }
}

const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow.data,
      qt_meta_data_MainWindow,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata0))
        return static_cast<void*>(const_cast< MainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 16)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 16;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 16)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 16;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
