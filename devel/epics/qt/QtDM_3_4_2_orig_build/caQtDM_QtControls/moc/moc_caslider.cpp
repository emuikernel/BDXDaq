/****************************************************************************
** Meta object code from reading C++ file 'caslider.h'
**
** Created: Wed Oct 2 14:09:35 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/caslider.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'caslider.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_caSlider[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       9,   14, // properties
       3,   41, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // properties: name, type, flags
      17,    9, 0x0a095003,
      35,   25, 0x0009510b,
      52,   45, 0x43095103,
      63,   45, 0x43095103,
      82,   74, 0x0009510b,
     103,   92, 0x0009510b,
     121,  114, 0x06095103,
     130,  114, 0x06095103,
     139,  114, 0x06095003,

 // enums: name, flags, count, data
      25, 0x0,    4,   53,
      74, 0x0,    3,   61,
      92, 0x0,    2,   67,

 // enum data: key, value
     145, uint(caSlider::Up),
     148, uint(caSlider::Down),
     153, uint(caSlider::Left),
     158, uint(caSlider::Right),
     164, uint(caSlider::Default),
     172, uint(caSlider::Static),
     179, uint(caSlider::Alarm),
     185, uint(caSlider::Channel),
     193, uint(caSlider::User),

       0        // eod
};

static const char qt_meta_stringdata_caSlider[] = {
    "caSlider\0QString\0channel\0Direction\0"
    "direction\0QColor\0foreground\0background\0"
    "colMode\0colorMode\0SourceMode\0limitsMode\0"
    "double\0maxValue\0minValue\0value\0Up\0"
    "Down\0Left\0Right\0Default\0Static\0Alarm\0"
    "Channel\0User\0"
};

void caSlider::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData caSlider::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject caSlider::staticMetaObject = {
    { &QwtSlider::staticMetaObject, qt_meta_stringdata_caSlider,
      qt_meta_data_caSlider, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &caSlider::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *caSlider::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *caSlider::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_caSlider))
        return static_cast<void*>(const_cast< caSlider*>(this));
    return QwtSlider::qt_metacast(_clname);
}

int caSlider::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QwtSlider::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
     if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = getPV(); break;
        case 1: *reinterpret_cast< Direction*>(_v) = getDirection(); break;
        case 2: *reinterpret_cast< QColor*>(_v) = getForeground(); break;
        case 3: *reinterpret_cast< QColor*>(_v) = getBackground(); break;
        case 4: *reinterpret_cast< colMode*>(_v) = getColorMode(); break;
        case 5: *reinterpret_cast< SourceMode*>(_v) = getLimitsMode(); break;
        case 6: *reinterpret_cast< double*>(_v) = getMaxValue(); break;
        case 7: *reinterpret_cast< double*>(_v) = getMinValue(); break;
        case 8: *reinterpret_cast< double*>(_v) = getSliderValue(); break;
        }
        _id -= 9;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setPV(*reinterpret_cast< QString*>(_v)); break;
        case 1: setDirection(*reinterpret_cast< Direction*>(_v)); break;
        case 2: setForeground(*reinterpret_cast< QColor*>(_v)); break;
        case 3: setBackground(*reinterpret_cast< QColor*>(_v)); break;
        case 4: setColorMode(*reinterpret_cast< colMode*>(_v)); break;
        case 5: setLimitsMode(*reinterpret_cast< SourceMode*>(_v)); break;
        case 6: setMaxValue(*reinterpret_cast< double*>(_v)); break;
        case 7: setMinValue(*reinterpret_cast< double*>(_v)); break;
        case 8: setSliderValue(*reinterpret_cast< double*>(_v)); break;
        }
        _id -= 9;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 9;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 9;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 9;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 9;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 9;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 9;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
