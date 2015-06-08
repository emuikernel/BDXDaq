/****************************************************************************
** Meta object code from reading C++ file 'egauge.h'
**
** Created: Wed Oct 2 14:11:35 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/egauge.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'egauge.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_EAbstractGauge[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
      17,   24, // properties
       1,   75, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      16,   15,   15,   15, 0x0a,
      33,   15,   15,   15, 0x0a,

 // properties: name, type, flags
      54,   47, 0x06095103,
      63,   47, 0x06095103,
      72,   47, 0x06095103,
      83,   47, 0x06095103,
      95,   47, 0x06095103,
     105,   47, 0x06095103,
     114,   47, 0x06095103,
     120,   47, 0x06095103,
     135,  130, 0x01095103,
     148,  130, 0x01095103,
     165,  130, 0x01095003,
     197,  187, 0x0009510b,
     207,  130, 0x01095103,
     221,  130, 0x01095103,
     246,  238, 0x0a095103,
     262,  258, 0x02095103,
     276,  258, 0x02095103,

 // enums: name, flags, count, data
     187, 0x0,    3,   79,

 // enum data: key, value
     290, uint(EAbstractGauge::GRADIENT),
     299, uint(EAbstractGauge::COLORBAR),
     308, uint(EAbstractGauge::SINGLECOLOR),

       0        // eod
};

static const char qt_meta_stringdata_EAbstractGauge[] = {
    "EAbstractGauge\0\0setValue(double)\0"
    "setValue(int)\0double\0minValue\0lowError\0"
    "lowWarning\0highWarning\0highError\0"
    "maxValue\0value\0reference\0bool\0"
    "scaleEnabled\0referenceEnabled\0"
    "externalLimitsEnabled\0ColorMode\0"
    "colorMode\0externalScale\0logarithmicScale\0"
    "QString\0valueFormat\0int\0numMajorTicks\0"
    "numMinorTicks\0GRADIENT\0COLORBAR\0"
    "SINGLECOLOR\0"
};

void EAbstractGauge::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        EAbstractGauge *_t = static_cast<EAbstractGauge *>(_o);
        switch (_id) {
        case 0: _t->setValue((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 1: _t->setValue((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData EAbstractGauge::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject EAbstractGauge::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_EAbstractGauge,
      qt_meta_data_EAbstractGauge, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &EAbstractGauge::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *EAbstractGauge::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *EAbstractGauge::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_EAbstractGauge))
        return static_cast<void*>(const_cast< EAbstractGauge*>(this));
    return QWidget::qt_metacast(_clname);
}

int EAbstractGauge::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< double*>(_v) = minValue(); break;
        case 1: *reinterpret_cast< double*>(_v) = lowError(); break;
        case 2: *reinterpret_cast< double*>(_v) = lowWarning(); break;
        case 3: *reinterpret_cast< double*>(_v) = highWarning(); break;
        case 4: *reinterpret_cast< double*>(_v) = highError(); break;
        case 5: *reinterpret_cast< double*>(_v) = maxValue(); break;
        case 6: *reinterpret_cast< double*>(_v) = value(); break;
        case 7: *reinterpret_cast< double*>(_v) = reference(); break;
        case 8: *reinterpret_cast< bool*>(_v) = isScaleEnabled(); break;
        case 9: *reinterpret_cast< bool*>(_v) = isReferenceEnabled(); break;
        case 10: *reinterpret_cast< bool*>(_v) = isExternalEnabled(); break;
        case 11: *reinterpret_cast< ColorMode*>(_v) = colorMode(); break;
        case 12: *reinterpret_cast< bool*>(_v) = externalScale(); break;
        case 13: *reinterpret_cast< bool*>(_v) = logarithmicScale(); break;
        case 14: *reinterpret_cast< QString*>(_v) = valueFormat(); break;
        case 15: *reinterpret_cast< int*>(_v) = numMajorTicks(); break;
        case 16: *reinterpret_cast< int*>(_v) = numMinorTicks(); break;
        }
        _id -= 17;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setMinValue(*reinterpret_cast< double*>(_v)); break;
        case 1: setLowError(*reinterpret_cast< double*>(_v)); break;
        case 2: setLowWarning(*reinterpret_cast< double*>(_v)); break;
        case 3: setHighWarning(*reinterpret_cast< double*>(_v)); break;
        case 4: setHighError(*reinterpret_cast< double*>(_v)); break;
        case 5: setMaxValue(*reinterpret_cast< double*>(_v)); break;
        case 6: setValue(*reinterpret_cast< double*>(_v)); break;
        case 7: setReference(*reinterpret_cast< double*>(_v)); break;
        case 8: setScaleEnabled(*reinterpret_cast< bool*>(_v)); break;
        case 9: setReferenceEnabled(*reinterpret_cast< bool*>(_v)); break;
        case 10: setExternalEnabled(*reinterpret_cast< bool*>(_v)); break;
        case 11: setColorMode(*reinterpret_cast< ColorMode*>(_v)); break;
        case 12: setExternalScale(*reinterpret_cast< bool*>(_v)); break;
        case 13: setLogarithmicScale(*reinterpret_cast< bool*>(_v)); break;
        case 14: setValueFormat(*reinterpret_cast< QString*>(_v)); break;
        case 15: setNumMajorTicks(*reinterpret_cast< int*>(_v)); break;
        case 16: setNumMinorTicks(*reinterpret_cast< int*>(_v)); break;
        }
        _id -= 17;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 17;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 17;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 17;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 17;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 17;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 17;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
