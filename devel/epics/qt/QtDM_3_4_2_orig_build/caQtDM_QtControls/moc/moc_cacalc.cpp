/****************************************************************************
** Meta object code from reading C++ file 'cacalc.h'
**
** Created: Wed Oct 2 14:11:42 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/cacalc.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'cacalc.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_caCalc[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       7,   14, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // properties: name, type, flags
      15,    7, 0x0a095103,
      24,    7, 0x0a095103,
      29,    7, 0x0a095003,
      37,    7, 0x0a095103,
      46,    7, 0x0a095103,
      55,    7, 0x0a095103,
      71,   64, 0x06095103,

       0        // eod
};

static const char qt_meta_stringdata_caCalc[] = {
    "caCalc\0QString\0variable\0calc\0channel\0"
    "channelB\0channelC\0channelD\0double\0"
    "initialValue\0"
};

void caCalc::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData caCalc::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject caCalc::staticMetaObject = {
    { &QLabel::staticMetaObject, qt_meta_stringdata_caCalc,
      qt_meta_data_caCalc, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &caCalc::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *caCalc::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *caCalc::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_caCalc))
        return static_cast<void*>(const_cast< caCalc*>(this));
    return QLabel::qt_metacast(_clname);
}

int caCalc::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QLabel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
     if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = getVariable(); break;
        case 1: *reinterpret_cast< QString*>(_v) = getCalc(); break;
        case 2: *reinterpret_cast< QString*>(_v) = getChannelA(); break;
        case 3: *reinterpret_cast< QString*>(_v) = getChannelB(); break;
        case 4: *reinterpret_cast< QString*>(_v) = getChannelC(); break;
        case 5: *reinterpret_cast< QString*>(_v) = getChannelD(); break;
        case 6: *reinterpret_cast< double*>(_v) = getInitialValue(); break;
        }
        _id -= 7;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setVariable(*reinterpret_cast< QString*>(_v)); break;
        case 1: setCalc(*reinterpret_cast< QString*>(_v)); break;
        case 2: setChannelA(*reinterpret_cast< QString*>(_v)); break;
        case 3: setChannelB(*reinterpret_cast< QString*>(_v)); break;
        case 4: setChannelC(*reinterpret_cast< QString*>(_v)); break;
        case 5: setChannelD(*reinterpret_cast< QString*>(_v)); break;
        case 6: setInitialValue(*reinterpret_cast< double*>(_v)); break;
        }
        _id -= 7;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 7;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
