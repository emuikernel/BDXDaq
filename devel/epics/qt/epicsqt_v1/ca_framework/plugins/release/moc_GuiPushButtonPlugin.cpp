/****************************************************************************
** Meta object code from reading C++ file 'GuiPushButtonPlugin.h'
**
** Created: Wed Oct 16 16:50:09 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../include/GuiPushButtonPlugin.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'GuiPushButtonPlugin.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GuiPushButtonPlugin[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       3,   14, // properties
       1,   23, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // properties: name, type, flags
      28,   20, 0x0a095003,
      56,   36, 0x0009500b,
      71,   20, 0x0a095003,

 // enums: name, flags, count, data
      36, 0x0,    3,   27,

 // enum data: key, value
      93, uint(GuiPushButtonPlugin::Open),
      98, uint(GuiPushButtonPlugin::NewTab),
     105, uint(GuiPushButtonPlugin::NewWindow),

       0        // eod
};

static const char qt_meta_stringdata_GuiPushButtonPlugin[] = {
    "GuiPushButtonPlugin\0QString\0guiFile\0"
    "CreationOptionNames\0creationOption\0"
    "variableSubstitutions\0Open\0NewTab\0"
    "NewWindow\0"
};

void GuiPushButtonPlugin::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData GuiPushButtonPlugin::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject GuiPushButtonPlugin::staticMetaObject = {
    { &GuiPushButton::staticMetaObject, qt_meta_stringdata_GuiPushButtonPlugin,
      qt_meta_data_GuiPushButtonPlugin, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GuiPushButtonPlugin::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GuiPushButtonPlugin::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GuiPushButtonPlugin::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GuiPushButtonPlugin))
        return static_cast<void*>(const_cast< GuiPushButtonPlugin*>(this));
    return GuiPushButton::qt_metacast(_clname);
}

int GuiPushButtonPlugin::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = GuiPushButton::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
     if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = getGuiName(); break;
        case 1: *reinterpret_cast< CreationOptionNames*>(_v) = getCreationOptionProperty(); break;
        case 2: *reinterpret_cast< QString*>(_v) = getVariableNameSubstitutions(); break;
        }
        _id -= 3;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setGuiName(*reinterpret_cast< QString*>(_v)); break;
        case 1: setCreationOptionProperty(*reinterpret_cast< CreationOptionNames*>(_v)); break;
        case 2: setVariableNameSubstitutions(*reinterpret_cast< QString*>(_v)); break;
        }
        _id -= 3;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 3;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
