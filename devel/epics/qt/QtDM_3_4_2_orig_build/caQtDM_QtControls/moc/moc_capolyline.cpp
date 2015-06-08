/****************************************************************************
** Meta object code from reading C++ file 'capolyline.h'
**
** Created: Wed Oct 2 14:10:49 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/capolyline.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'capolyline.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_caPolyLine[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
      14,   14, // properties
       5,   56, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // properties: name, type, flags
      18,   11, 0x43095103,
      39,   29, 0x0009500b,
      53,   49, 0x02095103,
      62,   11, 0x43095103,
      80,   72, 0x0009510b,
      98,   90, 0x0a095003,
     116,  106, 0x0009500b,
     136,  126, 0x0009500b,
     157,  146, 0x0009510b,
     168,   90, 0x0a095103,
     183,   90, 0x0a095003,
     191,   90, 0x0a095103,
     200,   90, 0x0a095103,
     209,   90, 0x0a095103,

 // enums: name, flags, count, data
     146, 0x0,    4,   76,
      29, 0x0,    3,   84,
      72, 0x0,    2,   90,
     106, 0x0,    2,   94,
     126, 0x0,    2,   98,

 // enum data: key, value
     218, uint(caPolyLine::StaticV),
     226, uint(caPolyLine::IfNotZero),
     236, uint(caPolyLine::IfZero),
     243, uint(caPolyLine::Calc),
     248, uint(caPolyLine::Solid),
     254, uint(caPolyLine::Dash),
     259, uint(caPolyLine::BigDash),
     267, uint(caPolyLine::Static),
     274, uint(caPolyLine::Alarm),
     280, uint(caPolyLine::Filled),
     287, uint(caPolyLine::Outline),
     295, uint(caPolyLine::Polyline),
     304, uint(caPolyLine::Polygon),

       0        // eod
};

static const char qt_meta_stringdata_caPolyLine[] = {
    "caPolyLine\0QColor\0foreground\0LineStyle\0"
    "linestyle\0int\0lineSize\0lineColor\0"
    "colMode\0colorMode\0QString\0xyPairs\0"
    "FillStyle\0fillstyle\0PolyStyle\0polystyle\0"
    "Visibility\0visibility\0visibilityCalc\0"
    "channel\0channelB\0channelC\0channelD\0"
    "StaticV\0IfNotZero\0IfZero\0Calc\0Solid\0"
    "Dash\0BigDash\0Static\0Alarm\0Filled\0"
    "Outline\0Polyline\0Polygon\0"
};

void caPolyLine::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData caPolyLine::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject caPolyLine::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_caPolyLine,
      qt_meta_data_caPolyLine, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &caPolyLine::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *caPolyLine::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *caPolyLine::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_caPolyLine))
        return static_cast<void*>(const_cast< caPolyLine*>(this));
    return QWidget::qt_metacast(_clname);
}

int caPolyLine::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
     if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QColor*>(_v) = getForeground(); break;
        case 1: *reinterpret_cast< LineStyle*>(_v) = getLineStyle(); break;
        case 2: *reinterpret_cast< int*>(_v) = getLineSize(); break;
        case 3: *reinterpret_cast< QColor*>(_v) = getLineColor(); break;
        case 4: *reinterpret_cast< colMode*>(_v) = getColorMode(); break;
        case 5: *reinterpret_cast< QString*>(_v) = getXYpairs(); break;
        case 6: *reinterpret_cast< FillStyle*>(_v) = getFillStyle(); break;
        case 7: *reinterpret_cast< PolyStyle*>(_v) = getPolyStyle(); break;
        case 8: *reinterpret_cast< Visibility*>(_v) = getVisibility(); break;
        case 9: *reinterpret_cast< QString*>(_v) = getVisibilityCalc(); break;
        case 10: *reinterpret_cast< QString*>(_v) = getChannelA(); break;
        case 11: *reinterpret_cast< QString*>(_v) = getChannelB(); break;
        case 12: *reinterpret_cast< QString*>(_v) = getChannelC(); break;
        case 13: *reinterpret_cast< QString*>(_v) = getChannelD(); break;
        }
        _id -= 14;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setForeground(*reinterpret_cast< QColor*>(_v)); break;
        case 1: setLineStyle(*reinterpret_cast< LineStyle*>(_v)); break;
        case 2: setLineSize(*reinterpret_cast< int*>(_v)); break;
        case 3: setLineColor(*reinterpret_cast< QColor*>(_v)); break;
        case 4: setColorMode(*reinterpret_cast< colMode*>(_v)); break;
        case 5: setXYpairs(*reinterpret_cast< QString*>(_v)); break;
        case 6: setFillStyle(*reinterpret_cast< FillStyle*>(_v)); break;
        case 7: setPolyStyle(*reinterpret_cast< PolyStyle*>(_v)); break;
        case 8: setVisibility(*reinterpret_cast< Visibility*>(_v)); break;
        case 9: setVisibilityCalc(*reinterpret_cast< QString*>(_v)); break;
        case 10: setChannelA(*reinterpret_cast< QString*>(_v)); break;
        case 11: setChannelB(*reinterpret_cast< QString*>(_v)); break;
        case 12: setChannelC(*reinterpret_cast< QString*>(_v)); break;
        case 13: setChannelD(*reinterpret_cast< QString*>(_v)); break;
        }
        _id -= 14;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 14;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 14;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 14;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 14;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 14;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 14;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
