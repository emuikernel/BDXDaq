/****************************************************************************
** Meta object code from reading C++ file 'cachoice.h'
**
** Created: Wed Oct 2 14:11:15 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/cachoice.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'cachoice.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_caChoice[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
      10,   19, // properties
       3,   49, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      15,   10,    9,    9, 0x05,

 // properties: name, type, flags
      40,   32, 0x0a095003,
      55,   48, 0x43095103,
      66,   48, 0x43095103,
      77,   48, 0x43095103,
     102,   89, 0x0009510b,
     120,  112, 0x0009510b,
     139,  130, 0x0009500b,
     156,  152, 0x02095103,
     165,  152, 0x02095103,
     195,  172, 0x0009510b,

 // enums: name, flags, count, data
      89, 0x0,    3,   61,
     112, 0x0,    3,   67,
     130, 0x0,    3,   73,

 // enum data: key, value
     209, uint(caChoice::left),
     214, uint(caChoice::right),
     220, uint(caChoice::center),
     227, uint(caChoice::Default),
     235, uint(caChoice::Static),
     242, uint(caChoice::Alarm),
     248, uint(caChoice::Row),
     252, uint(caChoice::Column),
     259, uint(caChoice::RowColumn),

       0        // eod
};

static const char qt_meta_stringdata_caChoice[] = {
    "caChoice\0\0text\0clicked(QString)\0QString\0"
    "channel\0QColor\0foreground\0background\0"
    "bordercolor\0alignmentHor\0alignment\0"
    "colMode\0colorMode\0Stacking\0stackingMode\0"
    "int\0startBit\0endBit\0EPushButton::ScaleMode\0"
    "fontScaleMode\0left\0right\0center\0Default\0"
    "Static\0Alarm\0Row\0Column\0RowColumn\0"
};

void caChoice::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        caChoice *_t = static_cast<caChoice *>(_o);
        switch (_id) {
        case 0: _t->clicked((*reinterpret_cast< QString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

#ifdef Q_NO_DATA_RELOCATION
static const QMetaObjectAccessor qt_meta_extradata_caChoice[] = {
        EPushButton::getStaticMetaObject,
#else
static const QMetaObject *qt_meta_extradata_caChoice[] = {
        &EPushButton::staticMetaObject,
#endif //Q_NO_DATA_RELOCATION
    0
};

const QMetaObjectExtraData caChoice::staticMetaObjectExtraData = {
    qt_meta_extradata_caChoice,  qt_static_metacall 
};

const QMetaObject caChoice::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_caChoice,
      qt_meta_data_caChoice, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &caChoice::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *caChoice::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *caChoice::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_caChoice))
        return static_cast<void*>(const_cast< caChoice*>(this));
    return QWidget::qt_metacast(_clname);
}

int caChoice::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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
        case 1: *reinterpret_cast< QColor*>(_v) = getForeground(); break;
        case 2: *reinterpret_cast< QColor*>(_v) = getBackground(); break;
        case 3: *reinterpret_cast< QColor*>(_v) = getBordercolor(); break;
        case 4: *reinterpret_cast< alignmentHor*>(_v) = getAlignment(); break;
        case 5: *reinterpret_cast< colMode*>(_v) = getColorMode(); break;
        case 6: *reinterpret_cast< Stacking*>(_v) = getStacking(); break;
        case 7: *reinterpret_cast< int*>(_v) = getStartBit(); break;
        case 8: *reinterpret_cast< int*>(_v) = getEndBit(); break;
        case 9: *reinterpret_cast< EPushButton::ScaleMode*>(_v) = fontScaleMode(); break;
        }
        _id -= 10;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setPV(*reinterpret_cast< QString*>(_v)); break;
        case 1: setForeground(*reinterpret_cast< QColor*>(_v)); break;
        case 2: setBackground(*reinterpret_cast< QColor*>(_v)); break;
        case 3: setBordercolor(*reinterpret_cast< QColor*>(_v)); break;
        case 4: setAlignment(*reinterpret_cast< alignmentHor*>(_v)); break;
        case 5: setColorMode(*reinterpret_cast< colMode*>(_v)); break;
        case 6: setStacking(*reinterpret_cast< Stacking*>(_v)); break;
        case 7: setStartBit(*reinterpret_cast< int*>(_v)); break;
        case 8: setEndBit(*reinterpret_cast< int*>(_v)); break;
        case 9: setFontScaleMode(*reinterpret_cast< EPushButton::ScaleMode*>(_v)); break;
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

// SIGNAL 0
void caChoice::clicked(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
