/****************************************************************************
** Meta object code from reading C++ file 'mutexKnobData.h'
**
** Created: Wed Oct 2 14:13:05 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/mutexKnobData.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mutexKnobData.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MutexKnobData[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      21,   15,   14,   14, 0x05,
      90,   88,   14,   14, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_MutexKnobData[] = {
    "MutexKnobData\0\0,,,,,\0"
    "Signal_UpdateWidget(int,QWidget*,QString,QString,QString,knobData)\0"
    ",\0Signal_QLineEdit(QString,QString)\0"
};

void MutexKnobData::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        MutexKnobData *_t = static_cast<MutexKnobData *>(_o);
        switch (_id) {
        case 0: _t->Signal_UpdateWidget((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QWidget*(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3])),(*reinterpret_cast< const QString(*)>(_a[4])),(*reinterpret_cast< const QString(*)>(_a[5])),(*reinterpret_cast< const knobData(*)>(_a[6]))); break;
        case 1: _t->Signal_QLineEdit((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData MutexKnobData::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject MutexKnobData::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_MutexKnobData,
      qt_meta_data_MutexKnobData, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MutexKnobData::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MutexKnobData::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MutexKnobData::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MutexKnobData))
        return static_cast<void*>(const_cast< MutexKnobData*>(this));
    return QObject::qt_metacast(_clname);
}

int MutexKnobData::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void MutexKnobData::Signal_UpdateWidget(int _t1, QWidget * _t2, const QString & _t3, const QString & _t4, const QString & _t5, const knobData & _t6)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)), const_cast<void*>(reinterpret_cast<const void*>(&_t5)), const_cast<void*>(reinterpret_cast<const void*>(&_t6)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void MutexKnobData::Signal_QLineEdit(const QString & _t1, const QString & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
