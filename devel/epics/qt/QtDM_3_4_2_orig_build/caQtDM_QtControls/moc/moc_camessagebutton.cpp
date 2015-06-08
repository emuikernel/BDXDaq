/****************************************************************************
** Meta object code from reading C++ file 'camessagebutton.h'
**
** Created: Wed Oct 2 14:11:04 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/camessagebutton.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'camessagebutton.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_caMessageButton[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       7,   19, // properties
       1,   40, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      22,   17,   16,   16, 0x05,

 // properties: name, type, flags
      55,   47, 0x0a095003,
      63,   47, 0x0a095103,
      76,   69, 0x43095103,
      87,   69, 0x43095103,
      98,   47, 0x0a095103,
     113,   47, 0x0a095103,
     134,  126, 0x0009510b,

 // enums: name, flags, count, data
     126, 0x0,    2,   44,

 // enum data: key, value
     144, uint(caMessageButton::Static),
     151, uint(caMessageButton::Alarm),

       0        // eod
};

static const char qt_meta_stringdata_caMessageButton[] = {
    "caMessageButton\0\0type\0messageButtonSignal(int)\0"
    "QString\0channel\0label\0QColor\0foreground\0"
    "background\0releaseMessage\0pressMessage\0"
    "colMode\0colorMode\0Static\0Alarm\0"
};

void caMessageButton::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        caMessageButton *_t = static_cast<caMessageButton *>(_o);
        switch (_id) {
        case 0: _t->messageButtonSignal((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData caMessageButton::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject caMessageButton::staticMetaObject = {
    { &EPushButton::staticMetaObject, qt_meta_stringdata_caMessageButton,
      qt_meta_data_caMessageButton, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &caMessageButton::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *caMessageButton::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *caMessageButton::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_caMessageButton))
        return static_cast<void*>(const_cast< caMessageButton*>(this));
    return EPushButton::qt_metacast(_clname);
}

int caMessageButton::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = EPushButton::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = getPV(); break;
        case 1: *reinterpret_cast< QString*>(_v) = getLabel(); break;
        case 2: *reinterpret_cast< QColor*>(_v) = getForeground(); break;
        case 3: *reinterpret_cast< QColor*>(_v) = getBackground(); break;
        case 4: *reinterpret_cast< QString*>(_v) = getReleaseMessage(); break;
        case 5: *reinterpret_cast< QString*>(_v) = getPressMessage(); break;
        case 6: *reinterpret_cast< colMode*>(_v) = getColorMode(); break;
        }
        _id -= 7;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setPV(*reinterpret_cast< QString*>(_v)); break;
        case 1: setLabel(*reinterpret_cast< QString*>(_v)); break;
        case 2: setForeground(*reinterpret_cast< QColor*>(_v)); break;
        case 3: setBackground(*reinterpret_cast< QColor*>(_v)); break;
        case 4: setReleaseMessage(*reinterpret_cast< QString*>(_v)); break;
        case 5: setPressMessage(*reinterpret_cast< QString*>(_v)); break;
        case 6: setColorMode(*reinterpret_cast< colMode*>(_v)); break;
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

// SIGNAL 0
void caMessageButton::messageButtonSignal(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
