/****************************************************************************
** Meta object code from reading C++ file 'CmdPushButtonPlugin.h'
**
** Created: Wed Oct 16 16:50:11 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../include/CmdPushButtonPlugin.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CmdPushButtonPlugin.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_CmdPushButtonPlugin[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       3,   14, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // properties: name, type, flags
      28,   20, 0x0a095103,
      48,   36, 0x0b095103,
      58,   20, 0x0a095003,

       0        // eod
};

static const char qt_meta_stringdata_CmdPushButtonPlugin[] = {
    "CmdPushButtonPlugin\0QString\0program\0"
    "QStringList\0arguments\0variableSubstitutions\0"
};

void CmdPushButtonPlugin::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData CmdPushButtonPlugin::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject CmdPushButtonPlugin::staticMetaObject = {
    { &CmdPushButton::staticMetaObject, qt_meta_stringdata_CmdPushButtonPlugin,
      qt_meta_data_CmdPushButtonPlugin, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &CmdPushButtonPlugin::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *CmdPushButtonPlugin::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *CmdPushButtonPlugin::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CmdPushButtonPlugin))
        return static_cast<void*>(const_cast< CmdPushButtonPlugin*>(this));
    return CmdPushButton::qt_metacast(_clname);
}

int CmdPushButtonPlugin::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = CmdPushButton::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
     if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = getProgram(); break;
        case 1: *reinterpret_cast< QStringList*>(_v) = getArguments(); break;
        case 2: *reinterpret_cast< QString*>(_v) = getVariableNameSubstitutions(); break;
        }
        _id -= 3;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setProgram(*reinterpret_cast< QString*>(_v)); break;
        case 1: setArguments(*reinterpret_cast< QStringList*>(_v)); break;
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
