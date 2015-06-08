/****************************************************************************
** Meta object code from reading C++ file 'caled.h'
**
** Created: Wed Oct 2 14:11:21 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/caled.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'caled.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_caLed[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       5,   14, // properties
       1,   29, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // properties: name, type, flags
      14,    6, 0x0a095003,
      26,   22, 0x02095103,
      40,   32, 0x0009510b,
      57,   50, 0x43095103,
      68,   50, 0x43095103,

 // enums: name, flags, count, data
      32, 0x0,    2,   33,

 // enum data: key, value
      78, uint(caLed::Static),
      85, uint(caLed::Alarm),

       0        // eod
};

static const char qt_meta_stringdata_caLed[] = {
    "caLed\0QString\0channel\0int\0bitNr\0colMode\0"
    "colorMode\0QColor\0falseColor\0trueColor\0"
    "Static\0Alarm\0"
};

void caLed::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData caLed::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject caLed::staticMetaObject = {
    { &ELed::staticMetaObject, qt_meta_stringdata_caLed,
      qt_meta_data_caLed, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &caLed::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *caLed::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *caLed::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_caLed))
        return static_cast<void*>(const_cast< caLed*>(this));
    return ELed::qt_metacast(_clname);
}

int caLed::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ELed::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
     if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = getPV(); break;
        case 1: *reinterpret_cast< int*>(_v) = getBitNr(); break;
        case 2: *reinterpret_cast< colMode*>(_v) = getColorMode(); break;
        case 3: *reinterpret_cast< QColor*>(_v) = getFalseColor(); break;
        case 4: *reinterpret_cast< QColor*>(_v) = getTrueColor(); break;
        }
        _id -= 5;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setPV(*reinterpret_cast< QString*>(_v)); break;
        case 1: setBitNr(*reinterpret_cast< int*>(_v)); break;
        case 2: setColorMode(*reinterpret_cast< colMode*>(_v)); break;
        case 3: setFalseColor(*reinterpret_cast< QColor*>(_v)); break;
        case 4: setTrueColor(*reinterpret_cast< QColor*>(_v)); break;
        }
        _id -= 5;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 5;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
