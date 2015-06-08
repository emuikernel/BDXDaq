/****************************************************************************
** Meta object code from reading C++ file 'eflag.h'
**
** Created: Wed Oct 2 14:11:26 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/eflag.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'eflag.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_EFlag[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       8,   14, // properties
       1,   38, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // properties: name, type, flags
      10,    6, 0x02095103,
      18,    6, 0x02095103,
      53,   29, 0x0009510b,
      75,   67, 0x0a095103,
      86,   67, 0x0a095103,
      98,   67, 0x0a095103,
     110,   67, 0x0a095103,
     123,   67, 0x0a095103,

 // enums: name, flags, count, data
     135, 0x0,    3,   42,

 // enum data: key, value
     148, uint(EFlag::left),
     153, uint(EFlag::right),
     159, uint(EFlag::center),

       0        // eod
};

static const char qt_meta_stringdata_EFlag[] = {
    "EFlag\0int\0numRows\0numColumns\0"
    "ESimpleLabel::ScaleMode\0fontScaleMode\0"
    "QString\0trueColors\0falseColors\0"
    "trueStrings\0falseStrings\0displayMask\0"
    "alignmentHor\0left\0right\0center\0"
};

void EFlag::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

#ifdef Q_NO_DATA_RELOCATION
static const QMetaObjectAccessor qt_meta_extradata_EFlag[] = {
        ESimpleLabel::getStaticMetaObject,
#else
static const QMetaObject *qt_meta_extradata_EFlag[] = {
        &ESimpleLabel::staticMetaObject,
#endif //Q_NO_DATA_RELOCATION
    0
};

const QMetaObjectExtraData EFlag::staticMetaObjectExtraData = {
    qt_meta_extradata_EFlag,  qt_static_metacall 
};

const QMetaObject EFlag::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_EFlag,
      qt_meta_data_EFlag, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &EFlag::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *EFlag::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *EFlag::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_EFlag))
        return static_cast<void*>(const_cast< EFlag*>(this));
    return QWidget::qt_metacast(_clname);
}

int EFlag::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
     if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = readNumRows(); break;
        case 1: *reinterpret_cast< int*>(_v) = readNumColumns(); break;
        case 2: *reinterpret_cast< ESimpleLabel::ScaleMode*>(_v) = fontScaleMode(); break;
        case 3: *reinterpret_cast< QString*>(_v) = trueColors(); break;
        case 4: *reinterpret_cast< QString*>(_v) = falseColors(); break;
        case 5: *reinterpret_cast< QString*>(_v) = trueStrings(); break;
        case 6: *reinterpret_cast< QString*>(_v) = falseStrings(); break;
        case 7: *reinterpret_cast< QString*>(_v) = getDisplayMask(); break;
        }
        _id -= 8;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setNumRows(*reinterpret_cast< int*>(_v)); break;
        case 1: setNumColumns(*reinterpret_cast< int*>(_v)); break;
        case 2: setFontScaleMode(*reinterpret_cast< ESimpleLabel::ScaleMode*>(_v)); break;
        case 3: setTrueColors(*reinterpret_cast< QString*>(_v)); break;
        case 4: setFalseColors(*reinterpret_cast< QString*>(_v)); break;
        case 5: setTrueStrings(*reinterpret_cast< QString*>(_v)); break;
        case 6: setFalseStrings(*reinterpret_cast< QString*>(_v)); break;
        case 7: setDisplayMask(*reinterpret_cast< QString*>(_v)); break;
        }
        _id -= 8;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 8;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 8;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 8;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 8;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 8;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 8;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
