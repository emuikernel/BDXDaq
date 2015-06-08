/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created: Tue Jan 19 17:19:23 2010
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "mainwindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MainWindow[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      19,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x08,
      22,   11,   11,   11, 0x08,
      29,   11,   11,   11, 0x08,
      36,   11,   11,   11, 0x08,
      44,   11,   11,   11, 0x08,
      51,   11,   11,   11, 0x08,
      58,   11,   11,   11, 0x08,
      64,   11,   11,   11, 0x08,
      71,   11,   11,   11, 0x08,
      79,   11,   11,   11, 0x08,
      86,   11,   11,   11, 0x08,
      95,   11,   11,   11, 0x08,
     107,   11,   11,   11, 0x08,
     120,   11,   11,   11, 0x08,
     130,   11,   11,   11, 0x08,
     139,   11,   11,   11, 0x08,
     156,   11,   11,   11, 0x08,
     178,   11,   11,   11, 0x08,
     186,   11,   11,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MainWindow[] = {
    "MainWindow\0\0newFile()\0open()\0save()\0"
    "print()\0undo()\0redo()\0cut()\0copy()\0"
    "paste()\0bold()\0italic()\0leftAlign()\0"
    "rightAlign()\0justify()\0center()\0"
    "setLineSpacing()\0setParagraphSpacing()\0"
    "about()\0aboutQt()\0"
};

const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow,
      qt_meta_data_MainWindow, 0 }
};

const QMetaObject *MainWindow::metaObject() const
{
    return &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow))
        return static_cast<void*>(const_cast< MainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: newFile(); break;
        case 1: open(); break;
        case 2: save(); break;
        case 3: print(); break;
        case 4: undo(); break;
        case 5: redo(); break;
        case 6: cut(); break;
        case 7: copy(); break;
        case 8: paste(); break;
        case 9: bold(); break;
        case 10: italic(); break;
        case 11: leftAlign(); break;
        case 12: rightAlign(); break;
        case 13: justify(); break;
        case 14: center(); break;
        case 15: setLineSpacing(); break;
        case 16: setParagraphSpacing(); break;
        case 17: about(); break;
        case 18: aboutQt(); break;
        default: ;
        }
        _id -= 19;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
