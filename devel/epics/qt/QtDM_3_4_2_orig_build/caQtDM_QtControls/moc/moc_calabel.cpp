/****************************************************************************
** Meta object code from reading C++ file 'calabel.h'
**
** Created: Wed Oct 2 14:10:55 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/calabel.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'calabel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_caLabel[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       9,   14, // properties
       2,   41, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // properties: name, type, flags
      15,    8, 0x43095103,
      26,    8, 0x43095103,
      45,   37, 0x0009510b,
      66,   55, 0x0009510b,
      85,   77, 0x0a095103,
     100,   77, 0x0a095003,
     108,   77, 0x0a095103,
     117,   77, 0x0a095103,
     126,   77, 0x0a095103,

 // enums: name, flags, count, data
      55, 0x0,    4,   49,
      37, 0x0,    2,   57,

 // enum data: key, value
     135, uint(caLabel::StaticV),
     143, uint(caLabel::IfNotZero),
     153, uint(caLabel::IfZero),
     160, uint(caLabel::Calc),
     165, uint(caLabel::Static),
     172, uint(caLabel::Alarm),

       0        // eod
};

static const char qt_meta_stringdata_caLabel[] = {
    "caLabel\0QColor\0foreground\0background\0"
    "colMode\0colorMode\0Visibility\0visibility\0"
    "QString\0visibilityCalc\0channel\0channelB\0"
    "channelC\0channelD\0StaticV\0IfNotZero\0"
    "IfZero\0Calc\0Static\0Alarm\0"
};

void caLabel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData caLabel::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject caLabel::staticMetaObject = {
    { &ESimpleLabel::staticMetaObject, qt_meta_stringdata_caLabel,
      qt_meta_data_caLabel, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &caLabel::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *caLabel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *caLabel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_caLabel))
        return static_cast<void*>(const_cast< caLabel*>(this));
    return ESimpleLabel::qt_metacast(_clname);
}

int caLabel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ESimpleLabel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
     if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QColor*>(_v) = getForeground(); break;
        case 1: *reinterpret_cast< QColor*>(_v) = getBackground(); break;
        case 2: *reinterpret_cast< colMode*>(_v) = getColorMode(); break;
        case 3: *reinterpret_cast< Visibility*>(_v) = getVisibility(); break;
        case 4: *reinterpret_cast< QString*>(_v) = getVisibilityCalc(); break;
        case 5: *reinterpret_cast< QString*>(_v) = getChannelA(); break;
        case 6: *reinterpret_cast< QString*>(_v) = getChannelB(); break;
        case 7: *reinterpret_cast< QString*>(_v) = getChannelC(); break;
        case 8: *reinterpret_cast< QString*>(_v) = getChannelD(); break;
        }
        _id -= 9;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setForeground(*reinterpret_cast< QColor*>(_v)); break;
        case 1: setBackground(*reinterpret_cast< QColor*>(_v)); break;
        case 2: setColorMode(*reinterpret_cast< colMode*>(_v)); break;
        case 3: setVisibility(*reinterpret_cast< Visibility*>(_v)); break;
        case 4: setVisibilityCalc(*reinterpret_cast< QString*>(_v)); break;
        case 5: setChannelA(*reinterpret_cast< QString*>(_v)); break;
        case 6: setChannelB(*reinterpret_cast< QString*>(_v)); break;
        case 7: setChannelC(*reinterpret_cast< QString*>(_v)); break;
        case 8: setChannelD(*reinterpret_cast< QString*>(_v)); break;
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
