/****************************************************************************
** Meta object code from reading C++ file 'cabitnames.h'
**
** Created: Wed Oct 2 14:11:24 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/cabitnames.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'cabitnames.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_caBitnames[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
      15,   14, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // properties: name, type, flags
      19,   11, 0x0a095003,
      31,   11, 0x0a095003,
      48,   44, 0x02095103,
      57,   44, 0x02095103,
      71,   64, 0x43095003,
      82,   64, 0x43095003,
     113,   93, 0x0009510b,
     147,  123, 0x0009510b,
     161,   44, 0x02094103,
     169,   44, 0x02094103,
     180,   11, 0x0a094103,
     191,   11, 0x0a094103,
     203,   11, 0x0a094103,
     215,   11, 0x0a094103,
     228,   11, 0x0a094103,

 // enums: name, flags, count, data

 // enum data: key, value

       0        // eod
};

static const char qt_meta_stringdata_caBitnames[] = {
    "caBitnames\0QString\0channelEnum\0"
    "channelValue\0int\0startBit\0endBit\0"
    "QColor\0foreground\0background\0"
    "EFlag::alignmentHor\0alignment\0"
    "ESimpleLabel::ScaleMode\0fontScaleMode\0"
    "numRows\0numColumns\0trueColors\0falseColors\0"
    "trueStrings\0falseStrings\0displayMask\0"
};

void caBitnames::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

#ifdef Q_NO_DATA_RELOCATION
static const QMetaObjectAccessor qt_meta_extradata_caBitnames[] = {
        EFlag::getStaticMetaObject,
    ESimpleLabel::getStaticMetaObject,
#else
static const QMetaObject *qt_meta_extradata_caBitnames[] = {
        &EFlag::staticMetaObject,
    &ESimpleLabel::staticMetaObject,
#endif //Q_NO_DATA_RELOCATION
    0
};

const QMetaObjectExtraData caBitnames::staticMetaObjectExtraData = {
    qt_meta_extradata_caBitnames,  qt_static_metacall 
};

const QMetaObject caBitnames::staticMetaObject = {
    { &EFlag::staticMetaObject, qt_meta_stringdata_caBitnames,
      qt_meta_data_caBitnames, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &caBitnames::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *caBitnames::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *caBitnames::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_caBitnames))
        return static_cast<void*>(const_cast< caBitnames*>(this));
    return EFlag::qt_metacast(_clname);
}

int caBitnames::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = EFlag::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
     if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = getEnumPV(); break;
        case 1: *reinterpret_cast< QString*>(_v) = getValuePV(); break;
        case 2: *reinterpret_cast< int*>(_v) = getStartBit(); break;
        case 3: *reinterpret_cast< int*>(_v) = getEndBit(); break;
        case 4: *reinterpret_cast< QColor*>(_v) = getTrueColor(); break;
        case 5: *reinterpret_cast< QColor*>(_v) = getFalseColor(); break;
        case 6: *reinterpret_cast< EFlag::alignmentHor*>(_v) = getAlignment(); break;
        case 7: *reinterpret_cast< ESimpleLabel::ScaleMode*>(_v) = fontScaleMode(); break;
        case 8: *reinterpret_cast< int*>(_v) = readNumRows(); break;
        case 9: *reinterpret_cast< int*>(_v) = readNumColumns(); break;
        case 10: *reinterpret_cast< QString*>(_v) = trueColors(); break;
        case 11: *reinterpret_cast< QString*>(_v) = falseColors(); break;
        case 12: *reinterpret_cast< QString*>(_v) = trueStrings(); break;
        case 13: *reinterpret_cast< QString*>(_v) = falseStrings(); break;
        case 14: *reinterpret_cast< QString*>(_v) = getDisplayMask(); break;
        }
        _id -= 15;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setEnumPV(*reinterpret_cast< QString*>(_v)); break;
        case 1: setValuePV(*reinterpret_cast< QString*>(_v)); break;
        case 2: setStartBit(*reinterpret_cast< int*>(_v)); break;
        case 3: setEndBit(*reinterpret_cast< int*>(_v)); break;
        case 4: setTrueColor(*reinterpret_cast< QColor*>(_v)); break;
        case 5: setFalseColor(*reinterpret_cast< QColor*>(_v)); break;
        case 6: setAlignment(*reinterpret_cast< EFlag::alignmentHor*>(_v)); break;
        case 7: setFontScaleMode(*reinterpret_cast< ESimpleLabel::ScaleMode*>(_v)); break;
        case 8: setNumRows(*reinterpret_cast< int*>(_v)); break;
        case 9: setNumColumns(*reinterpret_cast< int*>(_v)); break;
        case 10: setTrueColors(*reinterpret_cast< QString*>(_v)); break;
        case 11: setFalseColors(*reinterpret_cast< QString*>(_v)); break;
        case 12: setTrueStrings(*reinterpret_cast< QString*>(_v)); break;
        case 13: setFalseStrings(*reinterpret_cast< QString*>(_v)); break;
        case 14: setDisplayMask(*reinterpret_cast< QString*>(_v)); break;
        }
        _id -= 15;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 15;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 15;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 15;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 15;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 15;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 15;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
