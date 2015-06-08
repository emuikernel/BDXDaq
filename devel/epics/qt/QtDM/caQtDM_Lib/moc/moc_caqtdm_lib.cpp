/****************************************************************************
** Meta object code from reading C++ file 'caqtdm_lib.h'
**
** Created: Wed Oct 2 14:13:00 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/caqtdm_lib.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'caqtdm_lib.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_CaQtDM_Lib[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      22,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x05,
      31,   29,   11,   11, 0x05,
      68,   65,   11,   11, 0x05,
     113,   11,   11,   11, 0x05,
     142,   11,   11,   11, 0x05,

 // slots: signature, parameters, type, tag, flags
     156,  152,   11,   11, 0x08,
     199,  193,   11,   11, 0x08,
     230,  193,   11,   11, 0x08,
     256,  193,   11,   11, 0x08,
     285,  280,   11,   11, 0x08,
     313,  280,   11,   11, 0x08,
     341,   11,   11,   11, 0x08,
     396,  365,   11,   11, 0x08,
     465,   29,   11,   11, 0x08,
     502,   11,   11,   11, 0x08,
     532,   11,   11,   11, 0x08,
     564,   11,   11,   11, 0x08,
     600,   11,   11,   11, 0x08,
     634,   11,   11,   11, 0x08,
     660,  658,   11,   11, 0x08,
     689,   11,   11,   11, 0x08,
     724,   11,   11,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_CaQtDM_Lib[] = {
    "CaQtDM_Lib\0\0clicked(QString)\0,\0"
    "Signal_QLineEdit(QString,QString)\0,,\0"
    "Signal_OpenNewWFile(QString,QString,QString)\0"
    "Signal_ContextMenu(QWidget*)\0clicked()\0"
    "err\0processError(QProcess::ProcessError)\0"
    "value\0Callback_EApplyNumeric(double)\0"
    "Callback_ENumeric(double)\0"
    "Callback_Slider(double)\0type\0"
    "Callback_MessageButton(int)\0"
    "Callback_ToggleButton(bool)\0"
    "Callback_ScriptButton()\0"
    ",w,units,fec,statusString,data\0"
    "Callback_UpdateWidget(int,QWidget*,QString,QString,QString,knobData)\0"
    "Callback_UpdateLine(QString,QString)\0"
    "Callback_MenuClicked(QString)\0"
    "Callback_ChoiceClicked(QString)\0"
    "Callback_RelatedDisplayClicked(int)\0"
    "Callback_ShellCommandClicked(int)\0"
    "ShowContextMenu(QPoint)\0w\0"
    "DisplayContextMenu(QWidget*)\0"
    "Callback_TextEntryChanged(QString)\0"
    "processTerminated()\0"
};

void CaQtDM_Lib::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        CaQtDM_Lib *_t = static_cast<CaQtDM_Lib *>(_o);
        switch (_id) {
        case 0: _t->clicked((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->Signal_QLineEdit((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 2: _t->Signal_OpenNewWFile((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 3: _t->Signal_ContextMenu((*reinterpret_cast< QWidget*(*)>(_a[1]))); break;
        case 4: _t->clicked(); break;
        case 5: _t->processError((*reinterpret_cast< QProcess::ProcessError(*)>(_a[1]))); break;
        case 6: _t->Callback_EApplyNumeric((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 7: _t->Callback_ENumeric((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 8: _t->Callback_Slider((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 9: _t->Callback_MessageButton((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: _t->Callback_ToggleButton((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 11: _t->Callback_ScriptButton(); break;
        case 12: _t->Callback_UpdateWidget((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QWidget*(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3])),(*reinterpret_cast< const QString(*)>(_a[4])),(*reinterpret_cast< const QString(*)>(_a[5])),(*reinterpret_cast< const knobData(*)>(_a[6]))); break;
        case 13: _t->Callback_UpdateLine((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 14: _t->Callback_MenuClicked((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 15: _t->Callback_ChoiceClicked((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 16: _t->Callback_RelatedDisplayClicked((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 17: _t->Callback_ShellCommandClicked((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 18: _t->ShowContextMenu((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 19: _t->DisplayContextMenu((*reinterpret_cast< QWidget*(*)>(_a[1]))); break;
        case 20: _t->Callback_TextEntryChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 21: _t->processTerminated(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData CaQtDM_Lib::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject CaQtDM_Lib::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_CaQtDM_Lib,
      qt_meta_data_CaQtDM_Lib, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &CaQtDM_Lib::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *CaQtDM_Lib::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *CaQtDM_Lib::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CaQtDM_Lib))
        return static_cast<void*>(const_cast< CaQtDM_Lib*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int CaQtDM_Lib::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 22)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 22;
    }
    return _id;
}

// SIGNAL 0
void CaQtDM_Lib::clicked(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void CaQtDM_Lib::Signal_QLineEdit(const QString & _t1, const QString & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void CaQtDM_Lib::Signal_OpenNewWFile(const QString & _t1, const QString & _t2, const QString & _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void CaQtDM_Lib::Signal_ContextMenu(QWidget * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void CaQtDM_Lib::clicked()
{
    QMetaObject::activate(this, &staticMetaObject, 4, 0);
}
QT_END_MOC_NAMESPACE
