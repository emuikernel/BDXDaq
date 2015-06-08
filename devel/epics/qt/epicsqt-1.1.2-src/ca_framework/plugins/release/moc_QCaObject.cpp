/****************************************************************************
** Meta object code from reading C++ file 'QCaObject.h'
**
** Created: Wed Oct 16 16:50:20 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../data/include/QCaObject.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QCaObject.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_qcaobject__QCaObject[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      48,   22,   21,   21, 0x05,
     112,   97,   21,   21, 0x05,

 // slots: signature, parameters, type, tag, flags
     161,  155,  150,   21, 0x0a,
     181,   21,   21,   21, 0x0a,
     198,   21,   21,   21, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_qcaobject__QCaObject[] = {
    "qcaobject::QCaObject\0\0value,alarmInfo,timeStamp\0"
    "dataChanged(QVariant,QCaAlarmInfo&,QCaDateTime&)\0"
    "connectionInfo\0connectionChanged(QCaConnectionInfo&)\0"
    "bool\0value\0writeData(QVariant)\0"
    "resendLastData()\0setChannelExpired()\0"
};

void qcaobject::QCaObject::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        QCaObject *_t = static_cast<QCaObject *>(_o);
        switch (_id) {
        case 0: _t->dataChanged((*reinterpret_cast< const QVariant(*)>(_a[1])),(*reinterpret_cast< QCaAlarmInfo(*)>(_a[2])),(*reinterpret_cast< QCaDateTime(*)>(_a[3]))); break;
        case 1: _t->connectionChanged((*reinterpret_cast< QCaConnectionInfo(*)>(_a[1]))); break;
        case 2: { bool _r = _t->writeData((*reinterpret_cast< const QVariant(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 3: _t->resendLastData(); break;
        case 4: _t->setChannelExpired(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData qcaobject::QCaObject::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject qcaobject::QCaObject::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_qcaobject__QCaObject,
      qt_meta_data_qcaobject__QCaObject, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &qcaobject::QCaObject::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *qcaobject::QCaObject::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *qcaobject::QCaObject::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_qcaobject__QCaObject))
        return static_cast<void*>(const_cast< QCaObject*>(this));
    if (!strcmp(_clname, "caobject::CaObject"))
        return static_cast< caobject::CaObject*>(const_cast< QCaObject*>(this));
    return QObject::qt_metacast(_clname);
}

int qcaobject::QCaObject::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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
void qcaobject::QCaObject::dataChanged(const QVariant & _t1, QCaAlarmInfo & _t2, QCaDateTime & _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void qcaobject::QCaObject::connectionChanged(QCaConnectionInfo & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
