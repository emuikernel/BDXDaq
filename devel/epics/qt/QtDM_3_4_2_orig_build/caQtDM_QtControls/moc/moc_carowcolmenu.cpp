/****************************************************************************
** Meta object code from reading C++ file 'carowcolmenu.h'
**
** Created: Wed Oct 2 14:11:07 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/carowcolmenu.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'carowcolmenu.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_caRowColMenu[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       7,   24, // properties
       1,   45, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      19,   14,   13,   13, 0x05,
      32,   14,   13,   13, 0x05,

 // properties: name, type, flags
      55,   47, 0x0a095103,
      68,   61, 0x43095103,
      79,   61, 0x43095103,
      90,   47, 0x0a095103,
      97,   47, 0x0a095103,
     103,   47, 0x0a095103,
     131,  108, 0x0009510b,

 // enums: name, flags, count, data
     145, 0x0,    5,   49,

 // enum data: key, value
     154, uint(caRowColMenu::Menu),
     159, uint(caRowColMenu::Row),
     163, uint(caRowColMenu::Column),
     170, uint(caRowColMenu::RowColumn),
     180, uint(caRowColMenu::Hidden),

       0        // eod
};

static const char qt_meta_stringdata_caRowColMenu[] = {
    "caRowColMenu\0\0indx\0clicked(int)\0"
    "triggered(int)\0QString\0label\0QColor\0"
    "foreground\0background\0labels\0files\0"
    "args\0EPushButton::ScaleMode\0fontScaleMode\0"
    "Stacking\0Menu\0Row\0Column\0RowColumn\0"
    "Hidden\0"
};

void caRowColMenu::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        caRowColMenu *_t = static_cast<caRowColMenu *>(_o);
        switch (_id) {
        case 0: _t->clicked((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->triggered((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

#ifdef Q_NO_DATA_RELOCATION
static const QMetaObjectAccessor qt_meta_extradata_caRowColMenu[] = {
        EPushButton::getStaticMetaObject,
#else
static const QMetaObject *qt_meta_extradata_caRowColMenu[] = {
        &EPushButton::staticMetaObject,
#endif //Q_NO_DATA_RELOCATION
    0
};

const QMetaObjectExtraData caRowColMenu::staticMetaObjectExtraData = {
    qt_meta_extradata_caRowColMenu,  qt_static_metacall 
};

const QMetaObject caRowColMenu::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_caRowColMenu,
      qt_meta_data_caRowColMenu, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &caRowColMenu::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *caRowColMenu::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *caRowColMenu::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_caRowColMenu))
        return static_cast<void*>(const_cast< caRowColMenu*>(this));
    return QWidget::qt_metacast(_clname);
}

int caRowColMenu::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = getLabel(); break;
        case 1: *reinterpret_cast< QColor*>(_v) = getForeground(); break;
        case 2: *reinterpret_cast< QColor*>(_v) = getBackground(); break;
        case 3: *reinterpret_cast< QString*>(_v) = getLabels(); break;
        case 4: *reinterpret_cast< QString*>(_v) = getFiles(); break;
        case 5: *reinterpret_cast< QString*>(_v) = getArgs(); break;
        case 6: *reinterpret_cast< EPushButton::ScaleMode*>(_v) = fontScaleMode(); break;
        }
        _id -= 7;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setLabel(*reinterpret_cast< QString*>(_v)); break;
        case 1: setForeground(*reinterpret_cast< QColor*>(_v)); break;
        case 2: setBackground(*reinterpret_cast< QColor*>(_v)); break;
        case 3: setLabels(*reinterpret_cast< QString*>(_v)); break;
        case 4: setFiles(*reinterpret_cast< QString*>(_v)); break;
        case 5: setArgs(*reinterpret_cast< QString*>(_v)); break;
        case 6: setFontScaleMode(*reinterpret_cast< EPushButton::ScaleMode*>(_v)); break;
        }
        _id -= 7;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 7;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void caRowColMenu::clicked(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void caRowColMenu::triggered(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
