/****************************************************************************
** Meta object code from reading C++ file 'carelateddisplay.h'
**
** Created: Wed Oct 2 14:11:09 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/carelateddisplay.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'carelateddisplay.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_caRelatedDisplay[] = {

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
      26,   17, 0x0009500b,
      47,   39, 0x0a095003,

       0        // eod
};

static const char qt_meta_stringdata_caRelatedDisplay[] = {
    "caRelatedDisplay\0Stacking\0stackingMode\0"
    "QString\0removeParent\0"
};

void caRelatedDisplay::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData caRelatedDisplay::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject caRelatedDisplay::staticMetaObject = {
    { &caRowColMenu::staticMetaObject, qt_meta_stringdata_caRelatedDisplay,
      qt_meta_data_caRelatedDisplay, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &caRelatedDisplay::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *caRelatedDisplay::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *caRelatedDisplay::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_caRelatedDisplay))
        return static_cast<void*>(const_cast< caRelatedDisplay*>(this));
    return caRowColMenu::qt_metacast(_clname);
}

int caRelatedDisplay::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = caRowColMenu::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
     if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< Stacking*>(_v) = getStacking(); break;
        case 1: *reinterpret_cast< QString*>(_v) = getReplaceModes(); break;
        }
        _id -= 2;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setStacking(*reinterpret_cast< Stacking*>(_v)); break;
        case 1: setReplaceModes(*reinterpret_cast< QString*>(_v)); break;
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
