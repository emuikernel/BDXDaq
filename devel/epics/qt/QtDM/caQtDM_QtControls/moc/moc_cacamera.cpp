/****************************************************************************
** Meta object code from reading C++ file 'cacamera.h'
**
** Created: Wed Oct 2 14:11:38 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/cacamera.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'cacamera.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_caCamera[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
      11,   14, // properties
       2,   47, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // properties: name, type, flags
      17,    9, 0x0a095003,
      29,    9, 0x0a095003,
      42,    9, 0x0a095003,
      56,    9, 0x0a095003,
      68,    9, 0x0a095003,
      84,   79, 0x0009510b,
      98,   89, 0x0009500b,
     112,  107, 0x01095003,
     128,    9, 0x0a095103,
     137,    9, 0x0a095103,
     146,    9, 0x0a095103,

 // enums: name, flags, count, data
      79, 0x0,    2,   55,
      89, 0x0,    3,   59,

 // enum data: key, value
     163, uint(caCamera::No),
     166, uint(caCamera::Yes),
     170, uint(caCamera::Default),
     178, uint(caCamera::grey),
     183, uint(caCamera::spectrum),

       0        // eod
};

static const char qt_meta_stringdata_caCamera[] = {
    "caCamera\0QString\0channelData\0channelWidth\0"
    "channelHeight\0channelCode\0channelBPP\0"
    "zoom\0Zoom\0colormap\0ColorMap\0bool\0"
    "automaticLevels\0minLevel\0maxLevel\0"
    "dataProcChannels\0No\0Yes\0Default\0grey\0"
    "spectrum\0"
};

void caCamera::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData caCamera::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject caCamera::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_caCamera,
      qt_meta_data_caCamera, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &caCamera::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *caCamera::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *caCamera::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_caCamera))
        return static_cast<void*>(const_cast< caCamera*>(this));
    return QWidget::qt_metacast(_clname);
}

int caCamera::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
     if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = getPV_Data(); break;
        case 1: *reinterpret_cast< QString*>(_v) = getPV_Width(); break;
        case 2: *reinterpret_cast< QString*>(_v) = getPV_Height(); break;
        case 3: *reinterpret_cast< QString*>(_v) = getPV_Code(); break;
        case 4: *reinterpret_cast< QString*>(_v) = getPV_BPP(); break;
        case 5: *reinterpret_cast< zoom*>(_v) = getZoom(); break;
        case 6: *reinterpret_cast< colormap*>(_v) = getColormap(); break;
        case 7: *reinterpret_cast< bool*>(_v) = getInitialAutomatic(); break;
        case 8: *reinterpret_cast< QString*>(_v) = getMinLevel(); break;
        case 9: *reinterpret_cast< QString*>(_v) = getMaxLevel(); break;
        case 10: *reinterpret_cast< QString*>(_v) = getDataProcChannels(); break;
        }
        _id -= 11;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setPV_Data(*reinterpret_cast< QString*>(_v)); break;
        case 1: setPV_Width(*reinterpret_cast< QString*>(_v)); break;
        case 2: setPV_Height(*reinterpret_cast< QString*>(_v)); break;
        case 3: setPV_Code(*reinterpret_cast< QString*>(_v)); break;
        case 4: setPV_BPP(*reinterpret_cast< QString*>(_v)); break;
        case 5: setZoom(*reinterpret_cast< zoom*>(_v)); break;
        case 6: setColormap(*reinterpret_cast< colormap*>(_v)); break;
        case 7: setInitialAutomatic(*reinterpret_cast< bool*>(_v)); break;
        case 8: setMinLevel(*reinterpret_cast< QString*>(_v)); break;
        case 9: setMaxLevel(*reinterpret_cast< QString*>(_v)); break;
        case 10: setDataProcChannels(*reinterpret_cast< QString*>(_v)); break;
        }
        _id -= 11;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 11;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 11;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 11;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 11;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 11;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 11;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
