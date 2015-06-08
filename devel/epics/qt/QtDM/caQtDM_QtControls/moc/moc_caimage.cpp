/****************************************************************************
** Meta object code from reading C++ file 'caimage.h'
**
** Created: Wed Oct 2 14:10:46 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/caimage.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'caimage.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_caImage[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
      10,   14, // properties
       1,   44, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // properties: name, type, flags
      16,    8, 0x0a095103,
      26,    8, 0x0a095003,
      39,   35, 0x02095103,
      45,   35, 0x02095003,
      74,   63, 0x0009510b,
      85,    8, 0x0a095103,
     100,    8, 0x0a095003,
     108,    8, 0x0a095103,
     117,    8, 0x0a095103,
     126,    8, 0x0a095103,

 // enums: name, flags, count, data
      63, 0x0,    4,   48,

 // enum data: key, value
     135, uint(caImage::StaticV),
     143, uint(caImage::IfNotZero),
     153, uint(caImage::IfZero),
     160, uint(caImage::Calc),

       0        // eod
};

static const char qt_meta_stringdata_caImage[] = {
    "caImage\0QString\0imageCalc\0filename\0"
    "int\0frame\0delayMilliseconds\0Visibility\0"
    "visibility\0visibilityCalc\0channel\0"
    "channelB\0channelC\0channelD\0StaticV\0"
    "IfNotZero\0IfZero\0Calc\0"
};

void caImage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData caImage::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject caImage::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_caImage,
      qt_meta_data_caImage, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &caImage::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *caImage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *caImage::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_caImage))
        return static_cast<void*>(const_cast< caImage*>(this));
    return QWidget::qt_metacast(_clname);
}

int caImage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
     if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = getImageCalc(); break;
        case 1: *reinterpret_cast< QString*>(_v) = getFileName(); break;
        case 2: *reinterpret_cast< int*>(_v) = getFrame(); break;
        case 3: *reinterpret_cast< int*>(_v) = getDelay(); break;
        case 4: *reinterpret_cast< Visibility*>(_v) = getVisibility(); break;
        case 5: *reinterpret_cast< QString*>(_v) = getVisibilityCalc(); break;
        case 6: *reinterpret_cast< QString*>(_v) = getChannelA(); break;
        case 7: *reinterpret_cast< QString*>(_v) = getChannelB(); break;
        case 8: *reinterpret_cast< QString*>(_v) = getChannelC(); break;
        case 9: *reinterpret_cast< QString*>(_v) = getChannelD(); break;
        }
        _id -= 10;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setImageCalc(*reinterpret_cast< QString*>(_v)); break;
        case 1: setFileName(*reinterpret_cast< QString*>(_v)); break;
        case 2: setFrame(*reinterpret_cast< int*>(_v)); break;
        case 3: setDelay(*reinterpret_cast< int*>(_v)); break;
        case 4: setVisibility(*reinterpret_cast< Visibility*>(_v)); break;
        case 5: setVisibilityCalc(*reinterpret_cast< QString*>(_v)); break;
        case 6: setChannelA(*reinterpret_cast< QString*>(_v)); break;
        case 7: setChannelB(*reinterpret_cast< QString*>(_v)); break;
        case 8: setChannelC(*reinterpret_cast< QString*>(_v)); break;
        case 9: setChannelD(*reinterpret_cast< QString*>(_v)); break;
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
