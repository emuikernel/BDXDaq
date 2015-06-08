/****************************************************************************
** Meta object code from reading C++ file 'elabel.h'
**
** Created: Wed Oct 2 14:10:51 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/elabel.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'elabel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ELabel[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       4,   14, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // properties: name, type, flags
      14,    7, 0x43094103,
      24,    7, 0x43094103,
      43,   35, 0x0a094103,
      54,   35, 0x0a094103,

       0        // eod
};

static const char qt_meta_stringdata_ELabel[] = {
    "ELabel\0QColor\0trueColor\0falseColor\0"
    "QString\0trueString\0falseString\0"
};

void ELabel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData ELabel::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ELabel::staticMetaObject = {
    { &ESimpleLabel::staticMetaObject, qt_meta_stringdata_ELabel,
      qt_meta_data_ELabel, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ELabel::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ELabel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ELabel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ELabel))
        return static_cast<void*>(const_cast< ELabel*>(this));
    return ESimpleLabel::qt_metacast(_clname);
}

int ELabel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ESimpleLabel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
     if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QColor*>(_v) = trueColor(); break;
        case 1: *reinterpret_cast< QColor*>(_v) = falseColor(); break;
        case 2: *reinterpret_cast< QString*>(_v) = trueString(); break;
        case 3: *reinterpret_cast< QString*>(_v) = falseString(); break;
        }
        _id -= 4;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setTrueColor(*reinterpret_cast< QColor*>(_v)); break;
        case 1: setFalseColor(*reinterpret_cast< QColor*>(_v)); break;
        case 2: setTrueString(*reinterpret_cast< QString*>(_v)); break;
        case 3: setFalseString(*reinterpret_cast< QString*>(_v)); break;
        }
        _id -= 4;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 4;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
