/****************************************************************************
** Meta object code from reading C++ file 'qwt_thermo_marker.h'
**
** Created: Wed Oct 2 14:11:57 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/qwt_thermo_marker.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qwt_thermo_marker.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QwtThermoMarker[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
      10,   19, // properties
       2,   49, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      21,   17,   16,   16, 0x0a,

 // properties: name, type, flags
      43,   38, 0x01095103,
      63,   56, 0x06095103,
      83,   74, 0x0009510b,
     101,   97, 0x02095103,
     109,   97, 0x02095103,
     121,   56, 0x06095103,
     130,   56, 0x06095103,
     139,   97, 0x02095103,
     149,   56, 0x06095103,
     167,  155, 0x0009510b,

 // enums: name, flags, count, data
     155, 0x0,    3,   57,
      74, 0x0,    5,   63,

 // enum data: key, value
     172, uint(QwtThermoMarker::Pipe),
     177, uint(QwtThermoMarker::Marker),
     184, uint(QwtThermoMarker::PipeFromCenter),
     199, uint(QwtThermoMarker::NoScale),
     207, uint(QwtThermoMarker::LeftScale),
     217, uint(QwtThermoMarker::RightScale),
     228, uint(QwtThermoMarker::TopScale),
     237, uint(QwtThermoMarker::BottomScale),

       0        // eod
};

static const char qt_meta_stringdata_QwtThermoMarker[] = {
    "QwtThermoMarker\0\0val\0setValue(double)\0"
    "bool\0alarmEnabled\0double\0alarmLevel\0"
    "ScalePos\0scalePosition\0int\0spacing\0"
    "borderWidth\0maxValue\0minValue\0pipeWidth\0"
    "value\0DisplayType\0type\0Pipe\0Marker\0"
    "PipeFromCenter\0NoScale\0LeftScale\0"
    "RightScale\0TopScale\0BottomScale\0"
};

void QwtThermoMarker::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        QwtThermoMarker *_t = static_cast<QwtThermoMarker *>(_o);
        switch (_id) {
        case 0: _t->setValue((*reinterpret_cast< double(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData QwtThermoMarker::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QwtThermoMarker::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QwtThermoMarker,
      qt_meta_data_QwtThermoMarker, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QwtThermoMarker::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QwtThermoMarker::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QwtThermoMarker::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QwtThermoMarker))
        return static_cast<void*>(const_cast< QwtThermoMarker*>(this));
    if (!strcmp(_clname, "QwtAbstractScale"))
        return static_cast< QwtAbstractScale*>(const_cast< QwtThermoMarker*>(this));
    return QWidget::qt_metacast(_clname);
}

int QwtThermoMarker::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = alarmEnabled(); break;
        case 1: *reinterpret_cast< double*>(_v) = alarmLevel(); break;
        case 2: *reinterpret_cast< ScalePos*>(_v) = scalePosition(); break;
        case 3: *reinterpret_cast< int*>(_v) = spacing(); break;
        case 4: *reinterpret_cast< int*>(_v) = borderWidth(); break;
        case 5: *reinterpret_cast< double*>(_v) = maxValue(); break;
        case 6: *reinterpret_cast< double*>(_v) = minValue(); break;
        case 7: *reinterpret_cast< int*>(_v) = pipeWidth(); break;
        case 8: *reinterpret_cast< double*>(_v) = value(); break;
        case 9: *reinterpret_cast< DisplayType*>(_v) = getType(); break;
        }
        _id -= 10;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setAlarmEnabled(*reinterpret_cast< bool*>(_v)); break;
        case 1: setAlarmLevel(*reinterpret_cast< double*>(_v)); break;
        case 2: setScalePosition(*reinterpret_cast< ScalePos*>(_v)); break;
        case 3: setSpacing(*reinterpret_cast< int*>(_v)); break;
        case 4: setBorderWidth(*reinterpret_cast< int*>(_v)); break;
        case 5: setMaxValue(*reinterpret_cast< double*>(_v)); break;
        case 6: setMinValue(*reinterpret_cast< double*>(_v)); break;
        case 7: setPipeWidth(*reinterpret_cast< int*>(_v)); break;
        case 8: setValue(*reinterpret_cast< double*>(_v)); break;
        case 9: setType(*reinterpret_cast< DisplayType*>(_v)); break;
        }
        _id -= 10;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 10;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
