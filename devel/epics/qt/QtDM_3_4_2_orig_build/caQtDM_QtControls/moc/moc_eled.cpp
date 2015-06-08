/****************************************************************************
** Meta object code from reading C++ file 'eled.h'
**
** Created: Wed Oct 2 14:11:22 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/eled.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'eled.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ELed[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
      10,   14, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // properties: name, type, flags
      10,    5, 0x01095103,
      22,    5, 0x01095103,
      42,   38, 0x02095103,
      51,   38, 0x02095103,
      61,   38, 0x02095103,
      67,   38, 0x02095103,
      80,    5, 0x01095103,
      95,    5, 0x01095103,
     116,  109, 0x06095103,
     129,  109, 0x06095103,

       0        // eod
};

static const char qt_meta_stringdata_ELed[] = {
    "ELed\0bool\0rectangular\0gradientEnabled\0"
    "int\0ledWidth\0ledHeight\0angle\0alphaChannel\0"
    "linearGradient\0scaleContents\0double\0"
    "gradientStop\0gradientStart\0"
};

void ELed::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData ELed::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ELed::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_ELed,
      qt_meta_data_ELed, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ELed::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ELed::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ELed::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ELed))
        return static_cast<void*>(const_cast< ELed*>(this));
    return QWidget::qt_metacast(_clname);
}

int ELed::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
     if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = rectangular(); break;
        case 1: *reinterpret_cast< bool*>(_v) = gradientEnabled(); break;
        case 2: *reinterpret_cast< int*>(_v) = ledWidth(); break;
        case 3: *reinterpret_cast< int*>(_v) = ledHeight(); break;
        case 4: *reinterpret_cast< int*>(_v) = angle(); break;
        case 5: *reinterpret_cast< int*>(_v) = alphaChannel(); break;
        case 6: *reinterpret_cast< bool*>(_v) = linearGradient(); break;
        case 7: *reinterpret_cast< bool*>(_v) = scaleContents(); break;
        case 8: *reinterpret_cast< double*>(_v) = gradientStop(); break;
        case 9: *reinterpret_cast< double*>(_v) = gradientStart(); break;
        }
        _id -= 10;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setRectangular(*reinterpret_cast< bool*>(_v)); break;
        case 1: setGradientEnabled(*reinterpret_cast< bool*>(_v)); break;
        case 2: setLedWidth(*reinterpret_cast< int*>(_v)); break;
        case 3: setLedHeight(*reinterpret_cast< int*>(_v)); break;
        case 4: setAngle(*reinterpret_cast< int*>(_v)); break;
        case 5: setAlphaChannel(*reinterpret_cast< int*>(_v)); break;
        case 6: setLinearGradient(*reinterpret_cast< bool*>(_v)); break;
        case 7: setScaleContents(*reinterpret_cast< bool*>(_v)); break;
        case 8: setGradientStop(*reinterpret_cast< double*>(_v)); break;
        case 9: setGradientStart(*reinterpret_cast< double*>(_v)); break;
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
QT_END_MOC_NAMESPACE
