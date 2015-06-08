/****************************************************************************
** Meta object code from reading C++ file 'LinkPlugin.h'
**
** Created: Wed Oct 16 16:50:16 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../include/LinkPlugin.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'LinkPlugin.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_LinkPlugin[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       6,   14, // properties
       1,   32, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // properties: name, type, flags
      26,   11, 0x0009500b,
      44,   36, 0x0a095103,
      65,   60, 0x01095103,
      76,   60, 0x01095103,
      88,   36, 0x0a095103,
     101,   36, 0x0a095103,

 // enums: name, flags, count, data
      11, 0x0,    6,   36,

 // enum data: key, value
     115, uint(LinkPlugin::Equal),
     121, uint(LinkPlugin::NotEqual),
     130, uint(LinkPlugin::GreaterThan),
     142, uint(LinkPlugin::GreaterThanOrEqual),
     161, uint(LinkPlugin::LessThan),
     170, uint(LinkPlugin::LessThanOrEqual),

       0        // eod
};

static const char qt_meta_stringdata_LinkPlugin[] = {
    "LinkPlugin\0ConditionNames\0condition\0"
    "QString\0comparisonValue\0bool\0signalTrue\0"
    "signalFalse\0outTrueValue\0outFalseValue\0"
    "Equal\0NotEqual\0GreaterThan\0"
    "GreaterThanOrEqual\0LessThan\0LessThanOrEqual\0"
};

void LinkPlugin::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData LinkPlugin::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject LinkPlugin::staticMetaObject = {
    { &Link::staticMetaObject, qt_meta_stringdata_LinkPlugin,
      qt_meta_data_LinkPlugin, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &LinkPlugin::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *LinkPlugin::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *LinkPlugin::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_LinkPlugin))
        return static_cast<void*>(const_cast< LinkPlugin*>(this));
    return Link::qt_metacast(_clname);
}

int LinkPlugin::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Link::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
     if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< ConditionNames*>(_v) = getConditionProperty(); break;
        case 1: *reinterpret_cast< QString*>(_v) = getComparisonValue(); break;
        case 2: *reinterpret_cast< bool*>(_v) = getSignalTrue(); break;
        case 3: *reinterpret_cast< bool*>(_v) = getSignalFalse(); break;
        case 4: *reinterpret_cast< QString*>(_v) = getOutTrueValue(); break;
        case 5: *reinterpret_cast< QString*>(_v) = getOutFalseValue(); break;
        }
        _id -= 6;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setConditionProperty(*reinterpret_cast< ConditionNames*>(_v)); break;
        case 1: setComparisonValue(*reinterpret_cast< QString*>(_v)); break;
        case 2: setSignalTrue(*reinterpret_cast< bool*>(_v)); break;
        case 3: setSignalFalse(*reinterpret_cast< bool*>(_v)); break;
        case 4: setOutTrueValue(*reinterpret_cast< QString*>(_v)); break;
        case 5: setOutFalseValue(*reinterpret_cast< QString*>(_v)); break;
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
QT_END_MOC_NAMESPACE
