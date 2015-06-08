/****************************************************************************
** Meta object code from reading C++ file 'cagauge.h'
**
** Created: Wed Oct 2 14:11:33 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/cagauge.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'cagauge.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_caAbstractGauge[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       1,   14, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // properties: name, type, flags
      24,   16, 0x0a095003,

       0        // eod
};

static const char qt_meta_stringdata_caAbstractGauge[] = {
    "caAbstractGauge\0QString\0channel\0"
};

void caAbstractGauge::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData caAbstractGauge::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject caAbstractGauge::staticMetaObject = {
    { &EAbstractGauge::staticMetaObject, qt_meta_stringdata_caAbstractGauge,
      qt_meta_data_caAbstractGauge, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &caAbstractGauge::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *caAbstractGauge::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *caAbstractGauge::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_caAbstractGauge))
        return static_cast<void*>(const_cast< caAbstractGauge*>(this));
    return EAbstractGauge::qt_metacast(_clname);
}

int caAbstractGauge::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = EAbstractGauge::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
     if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = getPV(); break;
        }
        _id -= 1;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setPV(*reinterpret_cast< QString*>(_v)); break;
        }
        _id -= 1;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 1;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
static const uint qt_meta_data_caLinearGauge[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       2,   14, // properties
       1,   20, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // properties: name, type, flags
      30,   14, 0x0009510b,
      51,   42, 0x0009510b,

 // enums: name, flags, count, data
      42, 0x0,    4,   24,

 // enum data: key, value
      60, uint(caLinearGauge::ALL),
      64, uint(caLinearGauge::FROM_MIN),
      73, uint(caLinearGauge::FROM_ZERO),
      83, uint(caLinearGauge::FROM_REF),

       0        // eod
};

static const char qt_meta_stringdata_caLinearGauge[] = {
    "caLinearGauge\0Qt::Orientation\0orientation\0"
    "FillMode\0fillMode\0ALL\0FROM_MIN\0FROM_ZERO\0"
    "FROM_REF\0"
};

void caLinearGauge::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData caLinearGauge::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject caLinearGauge::staticMetaObject = {
    { &caAbstractGauge::staticMetaObject, qt_meta_stringdata_caLinearGauge,
      qt_meta_data_caLinearGauge, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &caLinearGauge::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *caLinearGauge::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *caLinearGauge::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_caLinearGauge))
        return static_cast<void*>(const_cast< caLinearGauge*>(this));
    return caAbstractGauge::qt_metacast(_clname);
}

int caLinearGauge::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = caAbstractGauge::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
     if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< Qt::Orientation*>(_v) = orientation(); break;
        case 1: *reinterpret_cast< FillMode*>(_v) = fillMode(); break;
        }
        _id -= 2;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setOrientation(*reinterpret_cast< Qt::Orientation*>(_v)); break;
        case 1: setFillMode(*reinterpret_cast< FillMode*>(_v)); break;
        }
        _id -= 2;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 2;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
static const uint qt_meta_data_caCircularGauge[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       2,   14, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // properties: name, type, flags
      21,   16, 0x01095103,
      44,   36, 0x0a095103,

       0        // eod
};

static const char qt_meta_stringdata_caCircularGauge[] = {
    "caCircularGauge\0bool\0valueDisplayed\0"
    "QString\0label\0"
};

void caCircularGauge::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData caCircularGauge::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject caCircularGauge::staticMetaObject = {
    { &caAbstractGauge::staticMetaObject, qt_meta_stringdata_caCircularGauge,
      qt_meta_data_caCircularGauge, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &caCircularGauge::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *caCircularGauge::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *caCircularGauge::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_caCircularGauge))
        return static_cast<void*>(const_cast< caCircularGauge*>(this));
    return caAbstractGauge::qt_metacast(_clname);
}

int caCircularGauge::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = caAbstractGauge::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
     if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = isValueDisplayed(); break;
        case 1: *reinterpret_cast< QString*>(_v) = label(); break;
        }
        _id -= 2;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setValueDisplayed(*reinterpret_cast< bool*>(_v)); break;
        case 1: setLabel(*reinterpret_cast< QString*>(_v)); break;
        }
        _id -= 2;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 2;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
