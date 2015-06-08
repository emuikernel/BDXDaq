/****************************************************************************
** Meta object code from reading C++ file 'QCaSpinBox.h'
**
** Created: Wed Oct 16 16:50:23 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../widgets/include/QCaSpinBox.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QCaSpinBox.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QCaSpinBox[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      16,   12,   11,   11, 0x05,

 // slots: signature, parameters, type, tag, flags
      57,   42,   11,   11, 0x08,
     104,   95,   11,   11, 0x08,
     166,  160,   11,   11, 0x08,
     194,  188,   11,   11, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_QCaSpinBox[] = {
    "QCaSpinBox\0\0out\0dbValueChanged(qlonglong)\0"
    "connectionInfo\0connectionChanged(QCaConnectionInfo&)\0"
    "value,,,\0"
    "setValueIfNoFocus(long,QCaAlarmInfo&,QCaDateTime&,uint)\0"
    "value\0userValueChanged(int)\0state\0"
    "requestEnabled(bool)\0"
};

void QCaSpinBox::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        QCaSpinBox *_t = static_cast<QCaSpinBox *>(_o);
        switch (_id) {
        case 0: _t->dbValueChanged((*reinterpret_cast< const qlonglong(*)>(_a[1]))); break;
        case 1: _t->connectionChanged((*reinterpret_cast< QCaConnectionInfo(*)>(_a[1]))); break;
        case 2: _t->setValueIfNoFocus((*reinterpret_cast< const long(*)>(_a[1])),(*reinterpret_cast< QCaAlarmInfo(*)>(_a[2])),(*reinterpret_cast< QCaDateTime(*)>(_a[3])),(*reinterpret_cast< const uint(*)>(_a[4]))); break;
        case 3: _t->userValueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->requestEnabled((*reinterpret_cast< const bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData QCaSpinBox::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QCaSpinBox::staticMetaObject = {
    { &QSpinBox::staticMetaObject, qt_meta_stringdata_QCaSpinBox,
      qt_meta_data_QCaSpinBox, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QCaSpinBox::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QCaSpinBox::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QCaSpinBox::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QCaSpinBox))
        return static_cast<void*>(const_cast< QCaSpinBox*>(this));
    if (!strcmp(_clname, "QCaWidget"))
        return static_cast< QCaWidget*>(const_cast< QCaSpinBox*>(this));
    return QSpinBox::qt_metacast(_clname);
}

int QCaSpinBox::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QSpinBox::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void QCaSpinBox::dbValueChanged(const qlonglong & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
