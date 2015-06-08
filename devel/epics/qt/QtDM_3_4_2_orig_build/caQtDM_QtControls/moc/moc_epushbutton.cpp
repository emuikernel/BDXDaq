/****************************************************************************
** Meta object code from reading C++ file 'epushbutton.h'
**
** Created: Wed Oct 2 14:10:57 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/epushbutton.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'epushbutton.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_EPushButton[] = {

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
      22,   12, 0x0009510b,
      43,   36, 0x06094103,

 // enums: name, flags, count, data
      12, 0x0,    3,   24,

 // enum data: key, value
      59, uint(EPushButton::None),
      64, uint(EPushButton::Height),
      71, uint(EPushButton::WidthAndHeight),

       0        // eod
};

static const char qt_meta_stringdata_EPushButton[] = {
    "EPushButton\0ScaleMode\0fontScaleMode\0"
    "double\0fontScaleFactor\0None\0Height\0"
    "WidthAndHeight\0"
};

void EPushButton::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData EPushButton::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject EPushButton::staticMetaObject = {
    { &QPushButton::staticMetaObject, qt_meta_stringdata_EPushButton,
      qt_meta_data_EPushButton, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &EPushButton::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *EPushButton::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *EPushButton::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_EPushButton))
        return static_cast<void*>(const_cast< EPushButton*>(this));
    if (!strcmp(_clname, "FontScalingWidget"))
        return static_cast< FontScalingWidget*>(const_cast< EPushButton*>(this));
    return QPushButton::qt_metacast(_clname);
}

int EPushButton::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QPushButton::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
     if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< ScaleMode*>(_v) = fontScaleMode(); break;
        case 1: *reinterpret_cast< double*>(_v) = fontScaleFactor(); break;
        }
        _id -= 2;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setFontScaleMode(*reinterpret_cast< ScaleMode*>(_v)); break;
        case 1: setFontScaleFactor(*reinterpret_cast< double*>(_v)); break;
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
