/****************************************************************************
** Meta object code from reading C++ file 'QCaLineEdit.h'
**
** Created: Wed Oct 16 16:50:29 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../widgets/include/QCaLineEdit.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QCaLineEdit.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QCaLineEdit[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      17,   13,   12,   12, 0x05,

 // slots: signature, parameters, type, tag, flags
      56,   41,   12,   12, 0x08,
     103,   94,   12,   12, 0x08,
     161,   12,   12,   12, 0x08,
     181,   12,   12,   12, 0x08,
     209,  203,   12,   12, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_QCaLineEdit[] = {
    "QCaLineEdit\0\0out\0dbValueChanged(QString)\0"
    "connectionInfo\0connectionChanged(QCaConnectionInfo&)\0"
    "value,,,\0"
    "setTextIfNoFocus(QString,QCaAlarmInfo&,QCaDateTime&,uint)\0"
    "userReturnPressed()\0userEditingFinished()\0"
    "state\0requestEnabled(bool)\0"
};

void QCaLineEdit::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        QCaLineEdit *_t = static_cast<QCaLineEdit *>(_o);
        switch (_id) {
        case 0: _t->dbValueChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->connectionChanged((*reinterpret_cast< QCaConnectionInfo(*)>(_a[1]))); break;
        case 2: _t->setTextIfNoFocus((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< QCaAlarmInfo(*)>(_a[2])),(*reinterpret_cast< QCaDateTime(*)>(_a[3])),(*reinterpret_cast< const uint(*)>(_a[4]))); break;
        case 3: _t->userReturnPressed(); break;
        case 4: _t->userEditingFinished(); break;
        case 5: _t->requestEnabled((*reinterpret_cast< const bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData QCaLineEdit::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QCaLineEdit::staticMetaObject = {
    { &QLineEdit::staticMetaObject, qt_meta_stringdata_QCaLineEdit,
      qt_meta_data_QCaLineEdit, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QCaLineEdit::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QCaLineEdit::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QCaLineEdit::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QCaLineEdit))
        return static_cast<void*>(const_cast< QCaLineEdit*>(this));
    if (!strcmp(_clname, "QCaWidget"))
        return static_cast< QCaWidget*>(const_cast< QCaLineEdit*>(this));
    return QLineEdit::qt_metacast(_clname);
}

int QCaLineEdit::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QLineEdit::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void QCaLineEdit::dbValueChanged(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
