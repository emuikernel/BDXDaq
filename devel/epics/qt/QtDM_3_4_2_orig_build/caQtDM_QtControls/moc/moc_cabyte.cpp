/****************************************************************************
** Meta object code from reading C++ file 'cabyte.h'
**
** Created: Wed Oct 2 14:11:30 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/cabyte.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'cabyte.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_caByte[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       7,   14, // properties
       2,   35, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // properties: name, type, flags
      15,    7, 0x0a095003,
      33,   23, 0x0009510b,
      47,   43, 0x02095103,
      56,   43, 0x02095103,
      71,   63, 0x0009510b,
      88,   81, 0x43095003,
      99,   81, 0x43095003,

 // enums: name, flags, count, data
      63, 0x0,    2,   43,
      23, 0x0,    4,   47,

 // enum data: key, value
     110, uint(caByte::Static),
     117, uint(caByte::Alarm),
     123, uint(caByte::Up),
     126, uint(caByte::Down),
     131, uint(caByte::Left),
     136, uint(caByte::Right),

       0        // eod
};

static const char qt_meta_stringdata_caByte[] = {
    "caByte\0QString\0channel\0Direction\0"
    "direction\0int\0startBit\0endBit\0colMode\0"
    "colorMode\0QColor\0foreground\0background\0"
    "Static\0Alarm\0Up\0Down\0Left\0Right\0"
};

void caByte::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData caByte::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject caByte::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_caByte,
      qt_meta_data_caByte, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &caByte::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *caByte::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *caByte::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_caByte))
        return static_cast<void*>(const_cast< caByte*>(this));
    return QWidget::qt_metacast(_clname);
}

int caByte::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
     if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = getPV(); break;
        case 1: *reinterpret_cast< Direction*>(_v) = getDirection(); break;
        case 2: *reinterpret_cast< int*>(_v) = getStartBit(); break;
        case 3: *reinterpret_cast< int*>(_v) = getEndBit(); break;
        case 4: *reinterpret_cast< colMode*>(_v) = getColorMode(); break;
        case 5: *reinterpret_cast< QColor*>(_v) = getTrueColor(); break;
        case 6: *reinterpret_cast< QColor*>(_v) = getFalseColor(); break;
        }
        _id -= 7;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setPV(*reinterpret_cast< QString*>(_v)); break;
        case 1: setDirection(*reinterpret_cast< Direction*>(_v)); break;
        case 2: setStartBit(*reinterpret_cast< int*>(_v)); break;
        case 3: setEndBit(*reinterpret_cast< int*>(_v)); break;
        case 4: setColorMode(*reinterpret_cast< colMode*>(_v)); break;
        case 5: setTrueColor(*reinterpret_cast< QColor*>(_v)); break;
        case 6: setFalseColor(*reinterpret_cast< QColor*>(_v)); break;
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
