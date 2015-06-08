/****************************************************************************
** Meta object code from reading C++ file 'ASguiFormPlugin.h'
**
** Created: Wed Oct 16 16:50:12 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../include/ASguiFormPlugin.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ASguiFormPlugin.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ASguiFormPlugin[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       3,   19, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      74,   17,   16,   16, 0x08,

 // properties: name, type, flags
     131,  123, 0x0a095003,
     138,  123, 0x0a095003,
     165,  160, 0x01095103,

       0        // eod
};

static const char qt_meta_stringdata_ASguiFormPlugin[] = {
    "ASguiFormPlugin\0\0"
    "variableNameIn,variableNameSubstitutionsIn,variableIndex\0"
    "useNewVariableNameProperty(QString,QString,uint)\0"
    "QString\0uiFile\0variableSubstitutions\0"
    "bool\0handleGuiLaunchRequests\0"
};

void ASguiFormPlugin::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ASguiFormPlugin *_t = static_cast<ASguiFormPlugin *>(_o);
        switch (_id) {
        case 0: _t->useNewVariableNameProperty((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< uint(*)>(_a[3]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ASguiFormPlugin::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ASguiFormPlugin::staticMetaObject = {
    { &ASguiForm::staticMetaObject, qt_meta_stringdata_ASguiFormPlugin,
      qt_meta_data_ASguiFormPlugin, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ASguiFormPlugin::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ASguiFormPlugin::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ASguiFormPlugin::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ASguiFormPlugin))
        return static_cast<void*>(const_cast< ASguiFormPlugin*>(this));
    return ASguiForm::qt_metacast(_clname);
}

int ASguiFormPlugin::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ASguiForm::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = getUiFileName(); break;
        case 1: *reinterpret_cast< QString*>(_v) = getVariableNameSubstitutionsProperty(); break;
        case 2: *reinterpret_cast< bool*>(_v) = getHandleGuiLaunchRequests(); break;
        }
        _id -= 3;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setUiFileName(*reinterpret_cast< QString*>(_v)); break;
        case 1: setVariableNameSubstitutionsProperty(*reinterpret_cast< QString*>(_v)); break;
        case 2: setHandleGuiLaunchRequests(*reinterpret_cast< bool*>(_v)); break;
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
