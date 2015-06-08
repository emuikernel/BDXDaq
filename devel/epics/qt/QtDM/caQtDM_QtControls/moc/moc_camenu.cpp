/****************************************************************************
** Meta object code from reading C++ file 'camenu.h'
**
** Created: Wed Oct 2 14:11:02 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/camenu.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'camenu.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_caMenu[] = {

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
      15,    7, 0x0a095003,
      31,   23, 0x0009510b,
      48,   41, 0x43095103,
      59,   41, 0x43095103,
      75,   70, 0x01095103,

 // enums: name, flags, count, data
      23, 0x0,    3,   33,

 // enum data: key, value
      88, uint(caMenu::Default),
      96, uint(caMenu::Static),
     103, uint(caMenu::Alarm),

       0        // eod
};

static const char qt_meta_stringdata_caMenu[] = {
    "caMenu\0QString\0channel\0colMode\0colorMode\0"
    "QColor\0foreground\0background\0bool\0"
    "labelDisplay\0Default\0Static\0Alarm\0"
};

void caMenu::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData caMenu::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject caMenu::staticMetaObject = {
    { &QComboBox::staticMetaObject, qt_meta_stringdata_caMenu,
      qt_meta_data_caMenu, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &caMenu::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *caMenu::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *caMenu::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_caMenu))
        return static_cast<void*>(const_cast< caMenu*>(this));
    return QComboBox::qt_metacast(_clname);
}

int caMenu::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QComboBox::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
     if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = getPV(); break;
        case 1: *reinterpret_cast< colMode*>(_v) = getColorMode(); break;
        case 2: *reinterpret_cast< QColor*>(_v) = getForeground(); break;
        case 3: *reinterpret_cast< QColor*>(_v) = getBackground(); break;
        case 4: *reinterpret_cast< bool*>(_v) = getLabelDisplay(); break;
        }
        _id -= 5;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setPV(*reinterpret_cast< QString*>(_v)); break;
        case 1: setColorMode(*reinterpret_cast< colMode*>(_v)); break;
        case 2: setForeground(*reinterpret_cast< QColor*>(_v)); break;
        case 3: setBackground(*reinterpret_cast< QColor*>(_v)); break;
        case 4: setLabelDisplay(*reinterpret_cast< bool*>(_v)); break;
        }
        _id -= 5;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
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
