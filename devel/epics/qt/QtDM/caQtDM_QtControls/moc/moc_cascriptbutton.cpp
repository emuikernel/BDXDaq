/****************************************************************************
** Meta object code from reading C++ file 'cascriptbutton.h'
**
** Created: Wed Oct 2 14:11:48 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/cascriptbutton.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'cascriptbutton.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_caScriptButton[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       7,   29, // properties
       1,   50, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      16,   15,   15,   15, 0x05,

 // slots: signature, parameters, type, tag, flags
      37,   15,   15,   15, 0x08,
      53,   15,   15,   15, 0x08,

 // properties: name, type, flags
      83,   75, 0x0a095103,
      96,   89, 0x43095103,
     107,   89, 0x43095103,
     141,  118, 0x0009510b,
     170,  155, 0x0009510b,
     184,   75, 0x0a095103,
     198,   75, 0x0a095003,

 // enums: name, flags, count, data
     155, 0x0,    2,   54,

 // enum data: key, value
     214, uint(caScriptButton::Invisible),
     224, uint(caScriptButton::Visible),

       0        // eod
};

static const char qt_meta_stringdata_caScriptButton[] = {
    "caScriptButton\0\0scriptButtonSignal()\0"
    "buttonToggled()\0scriptButtonClicked()\0"
    "QString\0label\0QColor\0foreground\0"
    "background\0EPushButton::ScaleMode\0"
    "fontScaleMode\0defaultDisplay\0scriptDisplay\0"
    "scriptCommand\0scriptParameter\0Invisible\0"
    "Visible\0"
};

void caScriptButton::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        caScriptButton *_t = static_cast<caScriptButton *>(_o);
        switch (_id) {
        case 0: _t->scriptButtonSignal(); break;
        case 1: _t->buttonToggled(); break;
        case 2: _t->scriptButtonClicked(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

#ifdef Q_NO_DATA_RELOCATION
static const QMetaObjectAccessor qt_meta_extradata_caScriptButton[] = {
        EPushButton::getStaticMetaObject,
#else
static const QMetaObject *qt_meta_extradata_caScriptButton[] = {
        &EPushButton::staticMetaObject,
#endif //Q_NO_DATA_RELOCATION
    0
};

const QMetaObjectExtraData caScriptButton::staticMetaObjectExtraData = {
    qt_meta_extradata_caScriptButton,  qt_static_metacall 
};

const QMetaObject caScriptButton::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_caScriptButton,
      qt_meta_data_caScriptButton, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &caScriptButton::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *caScriptButton::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *caScriptButton::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_caScriptButton))
        return static_cast<void*>(const_cast< caScriptButton*>(this));
    return QWidget::qt_metacast(_clname);
}

int caScriptButton::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = getLabel(); break;
        case 1: *reinterpret_cast< QColor*>(_v) = getForeground(); break;
        case 2: *reinterpret_cast< QColor*>(_v) = getBackground(); break;
        case 3: *reinterpret_cast< EPushButton::ScaleMode*>(_v) = fontScaleMode(); break;
        case 4: *reinterpret_cast< defaultDisplay*>(_v) = getScriptDisplay(); break;
        case 5: *reinterpret_cast< QString*>(_v) = getScriptCommand(); break;
        case 6: *reinterpret_cast< QString*>(_v) = getScriptParam(); break;
        }
        _id -= 7;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setLabel(*reinterpret_cast< QString*>(_v)); break;
        case 1: setForeground(*reinterpret_cast< QColor*>(_v)); break;
        case 2: setBackground(*reinterpret_cast< QColor*>(_v)); break;
        case 3: setFontScaleMode(*reinterpret_cast< EPushButton::ScaleMode*>(_v)); break;
        case 4: setScriptDisplay(*reinterpret_cast< defaultDisplay*>(_v)); break;
        case 5: setScriptCommand(*reinterpret_cast< QString*>(_v)); break;
        case 6: setScriptParam(*reinterpret_cast< QString*>(_v)); break;
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
void caScriptButton::scriptButtonSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
