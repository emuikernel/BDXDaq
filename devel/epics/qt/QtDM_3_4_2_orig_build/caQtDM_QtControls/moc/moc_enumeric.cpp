/****************************************************************************
** Meta object code from reading C++ file 'enumeric.h'
**
** Created: Wed Oct 2 14:10:58 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/enumeric.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'enumeric.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ENumeric[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       6,   54, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      10,    9,    9,    9, 0x05,

 // slots: signature, parameters, type, tag, flags
      31,    9,    9,    9, 0x0a,
      48,    9,    9,    9, 0x0a,
      66,    9,    9,    9, 0x08,
      78,    9,    9,    9, 0x08,
     103,    9,    9,    9, 0x08,
     130,    9,    9,    9, 0x08,
     145,    9,    9,    9, 0x09,

 // properties: name, type, flags
     176,  172, 0x02095003,
     190,  172, 0x02095003,
     211,  204, 0x06095103,
     217,  204, 0x06095003,
     226,  204, 0x06095003,
     240,  235, 0x01095103,

       0        // eod
};

static const char qt_meta_stringdata_ENumeric[] = {
    "ENumeric\0\0valueChanged(double)\0"
    "setEnabled(bool)\0setDisabled(bool)\0"
    "dataInput()\0upData(QAbstractButton*)\0"
    "downData(QAbstractButton*)\0valueUpdated()\0"
    "resizeEvent(QResizeEvent*)\0int\0"
    "integerDigits\0decimalDigits\0double\0"
    "value\0maxValue\0minValue\0bool\0"
    "digitsFontScaleEnabled\0"
};

void ENumeric::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ENumeric *_t = static_cast<ENumeric *>(_o);
        switch (_id) {
        case 0: _t->valueChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 1: _t->setEnabled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->setDisabled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->dataInput(); break;
        case 4: _t->upData((*reinterpret_cast< QAbstractButton*(*)>(_a[1]))); break;
        case 5: _t->downData((*reinterpret_cast< QAbstractButton*(*)>(_a[1]))); break;
        case 6: _t->valueUpdated(); break;
        case 7: _t->resizeEvent((*reinterpret_cast< QResizeEvent*(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ENumeric::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ENumeric::staticMetaObject = {
    { &QFrame::staticMetaObject, qt_meta_stringdata_ENumeric,
      qt_meta_data_ENumeric, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ENumeric::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ENumeric::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ENumeric::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ENumeric))
        return static_cast<void*>(const_cast< ENumeric*>(this));
    if (!strcmp(_clname, "FloatDelegate"))
        return static_cast< FloatDelegate*>(const_cast< ENumeric*>(this));
    return QFrame::qt_metacast(_clname);
}

int ENumeric::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFrame::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = intDigits(); break;
        case 1: *reinterpret_cast< int*>(_v) = decDigits(); break;
        case 2: *reinterpret_cast< double*>(_v) = value(); break;
        case 3: *reinterpret_cast< double*>(_v) = maximum(); break;
        case 4: *reinterpret_cast< double*>(_v) = minimum(); break;
        case 5: *reinterpret_cast< bool*>(_v) = digitsFontScaleEnabled(); break;
        }
        _id -= 6;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setIntDigits(*reinterpret_cast< int*>(_v)); break;
        case 1: setDecDigits(*reinterpret_cast< int*>(_v)); break;
        case 2: setValue(*reinterpret_cast< double*>(_v)); break;
        case 3: setMaximum(*reinterpret_cast< double*>(_v)); break;
        case 4: setMinimum(*reinterpret_cast< double*>(_v)); break;
        case 5: setDigitsFontScaleEnabled(*reinterpret_cast< bool*>(_v)); break;
        }
        _id -= 6;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 6;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void ENumeric::valueChanged(double _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
