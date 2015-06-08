/****************************************************************************
** Meta object code from reading C++ file 'cainclude.h'
**
** Created: Wed Oct 2 14:10:44 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/cainclude.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'cainclude.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_caInclude[] = {

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
      18,   10, 0x0a095103,
      24,   10, 0x0a095003,
      44,   33, 0x0009510b,
      55,   10, 0x0a095103,
      70,   10, 0x0a095003,
      78,   10, 0x0a095103,
      87,   10, 0x0a095103,
      96,   10, 0x0a095103,

 // enums: name, flags, count, data
      33, 0x0,    4,   42,

 // enum data: key, value
     105, uint(caInclude::StaticV),
     113, uint(caInclude::IfNotZero),
     123, uint(caInclude::IfZero),
     130, uint(caInclude::Calc),

       0        // eod
};

static const char qt_meta_stringdata_caInclude[] = {
    "caInclude\0QString\0macro\0filename\0"
    "Visibility\0visibility\0visibilityCalc\0"
    "channel\0channelB\0channelC\0channelD\0"
    "StaticV\0IfNotZero\0IfZero\0Calc\0"
};

void caInclude::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData caInclude::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject caInclude::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_caInclude,
      qt_meta_data_caInclude, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &caInclude::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *caInclude::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *caInclude::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_caInclude))
        return static_cast<void*>(const_cast< caInclude*>(this));
    return QWidget::qt_metacast(_clname);
}

int caInclude::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
     if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = getMacro(); break;
        case 1: *reinterpret_cast< QString*>(_v) = getFileName(); break;
        case 2: *reinterpret_cast< Visibility*>(_v) = getVisibility(); break;
        case 3: *reinterpret_cast< QString*>(_v) = getVisibilityCalc(); break;
        case 4: *reinterpret_cast< QString*>(_v) = getChannelA(); break;
        case 5: *reinterpret_cast< QString*>(_v) = getChannelB(); break;
        case 6: *reinterpret_cast< QString*>(_v) = getChannelC(); break;
        case 7: *reinterpret_cast< QString*>(_v) = getChannelD(); break;
        }
        _id -= 8;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setMacro(*reinterpret_cast< QString*>(_v)); break;
        case 1: setFileName(*reinterpret_cast< QString*>(_v)); break;
        case 2: setVisibility(*reinterpret_cast< Visibility*>(_v)); break;
        case 3: setVisibilityCalc(*reinterpret_cast< QString*>(_v)); break;
        case 4: setChannelA(*reinterpret_cast< QString*>(_v)); break;
        case 5: setChannelB(*reinterpret_cast< QString*>(_v)); break;
        case 6: setChannelC(*reinterpret_cast< QString*>(_v)); break;
        case 7: setChannelD(*reinterpret_cast< QString*>(_v)); break;
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
