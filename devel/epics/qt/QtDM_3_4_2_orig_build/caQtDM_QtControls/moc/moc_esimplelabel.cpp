/****************************************************************************
** Meta object code from reading C++ file 'esimplelabel.h'
**
** Created: Wed Oct 2 14:10:53 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/esimplelabel.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'esimplelabel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ESimpleLabel[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       5,   14, // properties
       1,   29, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // properties: name, type, flags
      18,   13, 0x01094001,
      42,   35, 0x06095103,
      60,   35, 0x06095103,
      79,   35, 0x06094103,
     105,   95, 0x0009510b,

 // enums: name, flags, count, data
      95, 0x0,    3,   33,

 // enum data: key, value
     119, uint(ESimpleLabel::None),
     124, uint(ESimpleLabel::Height),
     131, uint(ESimpleLabel::WidthAndHeight),

       0        // eod
};

static const char qt_meta_stringdata_ESimpleLabel[] = {
    "ESimpleLabel\0bool\0fontScaleEnabled\0"
    "double\0botTopBorderWidth\0lateralBorderWidth\0"
    "fontScaleFactor\0ScaleMode\0fontScaleMode\0"
    "None\0Height\0WidthAndHeight\0"
};

void ESimpleLabel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData ESimpleLabel::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ESimpleLabel::staticMetaObject = {
    { &QLabel::staticMetaObject, qt_meta_stringdata_ESimpleLabel,
      qt_meta_data_ESimpleLabel, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ESimpleLabel::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ESimpleLabel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ESimpleLabel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ESimpleLabel))
        return static_cast<void*>(const_cast< ESimpleLabel*>(this));
    if (!strcmp(_clname, "FontScalingWidget"))
        return static_cast< FontScalingWidget*>(const_cast< ESimpleLabel*>(this));
    return QLabel::qt_metacast(_clname);
}

int ESimpleLabel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QLabel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
     if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = fontScaleEnabled(); break;
        case 1: *reinterpret_cast< double*>(_v) = botTopBorderWidth(); break;
        case 2: *reinterpret_cast< double*>(_v) = lateralBorderWidth(); break;
        case 3: *reinterpret_cast< double*>(_v) = fontScaleFactor(); break;
        case 4: *reinterpret_cast< ScaleMode*>(_v) = fontScaleMode(); break;
        }
        _id -= 5;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 1: setBotTopBorderWidth(*reinterpret_cast< double*>(_v)); break;
        case 2: setLateralBorderWidth(*reinterpret_cast< double*>(_v)); break;
        case 3: setFontScaleFactor(*reinterpret_cast< double*>(_v)); break;
        case 4: setFontScaleMode(*reinterpret_cast< ScaleMode*>(_v)); break;
        }
        _id -= 5;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        bool *_b = reinterpret_cast<bool*>(_a[0]);
        switch (_id) {
        case 1: *_b = fontScaleEnabled(); break;
        case 2: *_b = fontScaleEnabled(); break;
        }
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 5;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
