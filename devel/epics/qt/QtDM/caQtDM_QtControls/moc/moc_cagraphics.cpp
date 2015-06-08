/****************************************************************************
** Meta object code from reading C++ file 'cagraphics.h'
**
** Created: Wed Oct 2 14:10:48 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/cagraphics.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'cagraphics.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_caGraphics[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
      18,   14, // properties
       6,   68, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // properties: name, type, flags
      18,   11, 0x43095103,
      39,   29, 0x0009500b,
      59,   49, 0x0009500b,
      73,   69, 0x02095103,
      82,   11, 0x43095103,
     100,   92, 0x0009510b,
     121,  110, 0x0009510b,
     140,  132, 0x0a095103,
     155,  132, 0x0a095003,
     163,  132, 0x0a095103,
     172,  132, 0x0a095103,
     181,  132, 0x0a095103,
     195,  190, 0x0009510b,
     200,   69, 0x02095103,
     220,  210, 0x0009510b,
     230,   69, 0x02095103,
     240,   69, 0x02095103,
     251,   69, 0x02095103,

 // enums: name, flags, count, data
     110, 0x0,    4,   92,
      29, 0x0,    3,  100,
      49, 0x0,    2,  106,
     190, 0x0,    6,  110,
     210, 0x0,    2,  122,
      92, 0x0,    2,  126,

 // enum data: key, value
     261, uint(caGraphics::StaticV),
     269, uint(caGraphics::IfNotZero),
     279, uint(caGraphics::IfZero),
     286, uint(caGraphics::Calc),
     291, uint(caGraphics::Solid),
     297, uint(caGraphics::Dash),
     302, uint(caGraphics::BigDash),
     310, uint(caGraphics::Filled),
     317, uint(caGraphics::Outline),
     325, uint(caGraphics::Rectangle),
     335, uint(caGraphics::Circle),
     342, uint(caGraphics::Arc),
     346, uint(caGraphics::Triangle),
     355, uint(caGraphics::Line),
     360, uint(caGraphics::Arrow),
     366, uint(caGraphics::Single),
     373, uint(caGraphics::Double),
     380, uint(caGraphics::Static),
     387, uint(caGraphics::Alarm),

       0        // eod
};

static const char qt_meta_stringdata_caGraphics[] = {
    "caGraphics\0QColor\0foreground\0LineStyle\0"
    "linestyle\0FillStyle\0fillstyle\0int\0"
    "lineSize\0lineColor\0colMode\0colorMode\0"
    "Visibility\0visibility\0QString\0"
    "visibilityCalc\0channel\0channelB\0"
    "channelC\0channelD\0Form\0form\0arrowSize\0"
    "ArrowMode\0arrowMode\0tiltAngle\0startAngle\0"
    "spanAngle\0StaticV\0IfNotZero\0IfZero\0"
    "Calc\0Solid\0Dash\0BigDash\0Filled\0Outline\0"
    "Rectangle\0Circle\0Arc\0Triangle\0Line\0"
    "Arrow\0Single\0Double\0Static\0Alarm\0"
};

void caGraphics::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData caGraphics::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject caGraphics::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_caGraphics,
      qt_meta_data_caGraphics, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &caGraphics::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *caGraphics::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *caGraphics::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_caGraphics))
        return static_cast<void*>(const_cast< caGraphics*>(this));
    return QWidget::qt_metacast(_clname);
}

int caGraphics::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
        case 2: *reinterpret_cast< FillStyle*>(_v) = getFillStyle(); break;
        case 3: *reinterpret_cast< int*>(_v) = getLineSize(); break;
        case 4: *reinterpret_cast< QColor*>(_v) = getLineColor(); break;
        case 5: *reinterpret_cast< colMode*>(_v) = getColorMode(); break;
        case 6: *reinterpret_cast< Visibility*>(_v) = getVisibility(); break;
        case 7: *reinterpret_cast< QString*>(_v) = getVisibilityCalc(); break;
        case 8: *reinterpret_cast< QString*>(_v) = getChannelA(); break;
        case 9: *reinterpret_cast< QString*>(_v) = getChannelB(); break;
        case 10: *reinterpret_cast< QString*>(_v) = getChannelC(); break;
        case 11: *reinterpret_cast< QString*>(_v) = getChannelD(); break;
        case 12: *reinterpret_cast< Form*>(_v) = getForm(); break;
        case 13: *reinterpret_cast< int*>(_v) = getArrowSize(); break;
        case 14: *reinterpret_cast< ArrowMode*>(_v) = getArrowMode(); break;
        case 15: *reinterpret_cast< int*>(_v) = getTiltAngle(); break;
        case 16: *reinterpret_cast< int*>(_v) = getStartAngle(); break;
        case 17: *reinterpret_cast< int*>(_v) = getSpanAngle(); break;
        }
        _id -= 18;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setForeground(*reinterpret_cast< QColor*>(_v)); break;
        case 1: setLineStyle(*reinterpret_cast< LineStyle*>(_v)); break;
        case 2: setFillStyle(*reinterpret_cast< FillStyle*>(_v)); break;
        case 3: setLineSize(*reinterpret_cast< int*>(_v)); break;
        case 4: setLineColor(*reinterpret_cast< QColor*>(_v)); break;
        case 5: setColorMode(*reinterpret_cast< colMode*>(_v)); break;
        case 6: setVisibility(*reinterpret_cast< Visibility*>(_v)); break;
        case 7: setVisibilityCalc(*reinterpret_cast< QString*>(_v)); break;
        case 8: setChannelA(*reinterpret_cast< QString*>(_v)); break;
        case 9: setChannelB(*reinterpret_cast< QString*>(_v)); break;
        case 10: setChannelC(*reinterpret_cast< QString*>(_v)); break;
        case 11: setChannelD(*reinterpret_cast< QString*>(_v)); break;
        case 12: setForm(*reinterpret_cast< Form*>(_v)); break;
        case 13: setArrowSize(*reinterpret_cast< int*>(_v)); break;
        case 14: setArrowMode(*reinterpret_cast< ArrowMode*>(_v)); break;
        case 15: setTiltAngle(*reinterpret_cast< int*>(_v)); break;
        case 16: setStartAngle(*reinterpret_cast< int*>(_v)); break;
        case 17: setSpanAngle(*reinterpret_cast< int*>(_v)); break;
        }
        _id -= 18;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 18;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 18;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 18;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 18;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 18;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 18;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
