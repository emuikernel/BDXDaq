/****************************************************************************
** Meta object code from reading C++ file 'caframe.h'
**
** Created: Wed Oct 2 14:10:43 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/caframe.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'caframe.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_caFrame[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
      10,   14, // properties
       3,   44, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // properties: name, type, flags
      16,    8, 0x0a095103,
      29,   22, 0x43095103,
      55,   40, 0x0009510b,
      81,   70, 0x0009510b,
     107,   92, 0x0009510b,
     122,    8, 0x0a095103,
     137,    8, 0x0a095003,
     145,    8, 0x0a095103,
     154,    8, 0x0a095103,
     163,    8, 0x0a095103,

 // enums: name, flags, count, data
      70, 0x0,    4,   56,
      92, 0x0,    2,   64,
      40, 0x0,    2,   68,

 // enum data: key, value
     172, uint(caFrame::StaticV),
     180, uint(caFrame::IfNotZero),
     190, uint(caFrame::IfZero),
     197, uint(caFrame::Calc),
     202, uint(caFrame::All),
     206, uint(caFrame::Background),
     217, uint(caFrame::Outline),
     225, uint(caFrame::Filled),

       0        // eod
};

static const char qt_meta_stringdata_caFrame[] = {
    "caFrame\0QString\0macro\0QColor\0background\0"
    "BackgroundMode\0backgroundMode\0Visibility\0"
    "visibility\0VisibilityMode\0visibilityMode\0"
    "visibilityCalc\0channel\0channelB\0"
    "channelC\0channelD\0StaticV\0IfNotZero\0"
    "IfZero\0Calc\0All\0Background\0Outline\0"
    "Filled\0"
};

void caFrame::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData caFrame::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject caFrame::staticMetaObject = {
    { &QFrame::staticMetaObject, qt_meta_stringdata_caFrame,
      qt_meta_data_caFrame, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &caFrame::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *caFrame::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *caFrame::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_caFrame))
        return static_cast<void*>(const_cast< caFrame*>(this));
    return QFrame::qt_metacast(_clname);
}

int caFrame::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFrame::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
     if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = getMacro(); break;
        case 1: *reinterpret_cast< QColor*>(_v) = getBackground(); break;
        case 2: *reinterpret_cast< BackgroundMode*>(_v) = getBackgroundMode(); break;
        case 3: *reinterpret_cast< Visibility*>(_v) = getVisibility(); break;
        case 4: *reinterpret_cast< VisibilityMode*>(_v) = getVisibilityMode(); break;
        case 5: *reinterpret_cast< QString*>(_v) = getVisibilityCalc(); break;
        case 6: *reinterpret_cast< QString*>(_v) = getChannelA(); break;
        case 7: *reinterpret_cast< QString*>(_v) = getChannelB(); break;
        case 8: *reinterpret_cast< QString*>(_v) = getChannelC(); break;
        case 9: *reinterpret_cast< QString*>(_v) = getChannelD(); break;
        }
        _id -= 10;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setMacro(*reinterpret_cast< QString*>(_v)); break;
        case 1: setBackground(*reinterpret_cast< QColor*>(_v)); break;
        case 2: setBackgroundMode(*reinterpret_cast< BackgroundMode*>(_v)); break;
        case 3: setVisibility(*reinterpret_cast< Visibility*>(_v)); break;
        case 4: setVisibilityMode(*reinterpret_cast< VisibilityMode*>(_v)); break;
        case 5: setVisibilityCalc(*reinterpret_cast< QString*>(_v)); break;
        case 6: setChannelA(*reinterpret_cast< QString*>(_v)); break;
        case 7: setChannelB(*reinterpret_cast< QString*>(_v)); break;
        case 8: setChannelC(*reinterpret_cast< QString*>(_v)); break;
        case 9: setChannelD(*reinterpret_cast< QString*>(_v)); break;
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
