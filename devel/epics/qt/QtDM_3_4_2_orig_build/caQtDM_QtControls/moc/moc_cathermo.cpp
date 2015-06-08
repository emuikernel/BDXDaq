/****************************************************************************
** Meta object code from reading C++ file 'cathermo.h'
**
** Created: Wed Oct 2 14:11:37 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/cathermo.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'cathermo.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_caThermo[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       8,   14, // properties
       4,   38, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // properties: name, type, flags
      17,    9, 0x0a095003,
      35,   25, 0x0009510b,
      50,   45, 0x0009510b,
      60,   55, 0x01095103,
      76,   69, 0x43095103,
      87,   69, 0x43095103,
     106,   98, 0x0009510b,
     127,  116, 0x0009510b,

 // enums: name, flags, count, data
      25, 0x0,    4,   54,
      45, 0x0,    5,   62,
      98, 0x0,    3,   72,
     116, 0x0,    2,   78,

 // enum data: key, value
     138, uint(caThermo::Up),
     141, uint(caThermo::Down),
     146, uint(caThermo::Left),
     151, uint(caThermo::Right),
     157, uint(caThermo::noLabel),
     165, uint(caThermo::noDeco),
     172, uint(caThermo::Outline),
     180, uint(caThermo::Limits),
     187, uint(caThermo::ChannelV),
     196, uint(caThermo::Default),
     204, uint(caThermo::Static),
     211, uint(caThermo::Alarm),
     217, uint(caThermo::Channel),
     225, uint(caThermo::User),

       0        // eod
};

static const char qt_meta_stringdata_caThermo[] = {
    "caThermo\0QString\0channel\0Direction\0"
    "direction\0Look\0look\0bool\0logScale\0"
    "QColor\0foreground\0background\0colMode\0"
    "colorMode\0SourceMode\0limitsMode\0Up\0"
    "Down\0Left\0Right\0noLabel\0noDeco\0Outline\0"
    "Limits\0ChannelV\0Default\0Static\0Alarm\0"
    "Channel\0User\0"
};

void caThermo::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData caThermo::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject caThermo::staticMetaObject = {
    { &QwtThermoMarker::staticMetaObject, qt_meta_stringdata_caThermo,
      qt_meta_data_caThermo, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &caThermo::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *caThermo::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *caThermo::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_caThermo))
        return static_cast<void*>(const_cast< caThermo*>(this));
    return QwtThermoMarker::qt_metacast(_clname);
}

int caThermo::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QwtThermoMarker::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
     if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = getPV(); break;
        case 1: *reinterpret_cast< Direction*>(_v) = getDirection(); break;
        case 2: *reinterpret_cast< Look*>(_v) = getLook(); break;
        case 3: *reinterpret_cast< bool*>(_v) = getLogScale(); break;
        case 4: *reinterpret_cast< QColor*>(_v) = getForeground(); break;
        case 5: *reinterpret_cast< QColor*>(_v) = getBackground(); break;
        case 6: *reinterpret_cast< colMode*>(_v) = getColorMode(); break;
        case 7: *reinterpret_cast< SourceMode*>(_v) = getLimitsMode(); break;
        }
        _id -= 8;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setPV(*reinterpret_cast< QString*>(_v)); break;
        case 1: setDirection(*reinterpret_cast< Direction*>(_v)); break;
        case 2: setLook(*reinterpret_cast< Look*>(_v)); break;
        case 3: setLogScale(*reinterpret_cast< bool*>(_v)); break;
        case 4: setForeground(*reinterpret_cast< QColor*>(_v)); break;
        case 5: setBackground(*reinterpret_cast< QColor*>(_v)); break;
        case 6: setColorMode(*reinterpret_cast< colMode*>(_v)); break;
        case 7: setLimitsMode(*reinterpret_cast< SourceMode*>(_v)); break;
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
