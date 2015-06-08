/****************************************************************************
** Meta object code from reading C++ file 'caapplynumeric.h'
**
** Created: Wed Oct 2 14:11:12 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/caapplynumeric.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'caapplynumeric.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_caApplyNumeric[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       8,   14, // properties
       1,   38, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // properties: name, type, flags
      23,   15, 0x0a095003,
      38,   31, 0x43095103,
      49,   31, 0x43095103,
      71,   60, 0x0009510b,
      90,   85, 0x01095103,
     102,   60, 0x0009510b,
     120,  113, 0x06095103,
     129,  113, 0x06095103,

 // enums: name, flags, count, data
      60, 0x0,    2,   42,

 // enum data: key, value
     138, uint(caApplyNumeric::Channel),
     146, uint(caApplyNumeric::User),

       0        // eod
};

static const char qt_meta_stringdata_caApplyNumeric[] = {
    "caApplyNumeric\0QString\0channel\0QColor\0"
    "foreground\0background\0SourceMode\0"
    "precisionMode\0bool\0fixedFormat\0"
    "limitsMode\0double\0maxValue\0minValue\0"
    "Channel\0User\0"
};

void caApplyNumeric::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData caApplyNumeric::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject caApplyNumeric::staticMetaObject = {
    { &EApplyNumeric::staticMetaObject, qt_meta_stringdata_caApplyNumeric,
      qt_meta_data_caApplyNumeric, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &caApplyNumeric::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *caApplyNumeric::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *caApplyNumeric::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_caApplyNumeric))
        return static_cast<void*>(const_cast< caApplyNumeric*>(this));
    return EApplyNumeric::qt_metacast(_clname);
}

int caApplyNumeric::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = EApplyNumeric::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
     if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = getPV(); break;
        case 1: *reinterpret_cast< QColor*>(_v) = getForeground(); break;
        case 2: *reinterpret_cast< QColor*>(_v) = getBackground(); break;
        case 3: *reinterpret_cast< SourceMode*>(_v) = getPrecisionMode(); break;
        case 4: *reinterpret_cast< bool*>(_v) = getFixedFormat(); break;
        case 5: *reinterpret_cast< SourceMode*>(_v) = getLimitsMode(); break;
        case 6: *reinterpret_cast< double*>(_v) = getMaxValue(); break;
        case 7: *reinterpret_cast< double*>(_v) = getMinValue(); break;
        }
        _id -= 8;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setPV(*reinterpret_cast< QString*>(_v)); break;
        case 1: setForeground(*reinterpret_cast< QColor*>(_v)); break;
        case 2: setBackground(*reinterpret_cast< QColor*>(_v)); break;
        case 3: setPrecisionMode(*reinterpret_cast< SourceMode*>(_v)); break;
        case 4: setFixedFormat(*reinterpret_cast< bool*>(_v)); break;
        case 5: setLimitsMode(*reinterpret_cast< SourceMode*>(_v)); break;
        case 6: setMaxValue(*reinterpret_cast< double*>(_v)); break;
        case 7: setMinValue(*reinterpret_cast< double*>(_v)); break;
        }
        _id -= 8;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 8;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 8;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 8;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 8;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 8;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 8;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
