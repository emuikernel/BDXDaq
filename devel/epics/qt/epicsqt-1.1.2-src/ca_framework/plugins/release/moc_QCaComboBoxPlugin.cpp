/****************************************************************************
** Meta object code from reading C++ file 'QCaComboBoxPlugin.h'
**
** Created: Wed Oct 16 16:49:58 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../include/QCaComboBoxPlugin.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QCaComboBoxPlugin.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QCaComboBoxPlugin[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       6,   19, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      76,   19,   18,   18, 0x08,

 // properties: name, type, flags
     133,  125, 0x0a095003,
     142,  125, 0x0a095003,
     169,  164, 0x01095103,
     179,  164, 0x01095103,
     197,  164, 0x01095103,
     205,  164, 0x01095103,

       0        // eod
};

static const char qt_meta_stringdata_QCaComboBoxPlugin[] = {
    "QCaComboBoxPlugin\0\0"
    "variableNameIn,variableNameSubstitutionsIn,variableIndex\0"
    "useNewVariableNameProperty(QString,QString,uint)\0"
    "QString\0variable\0variableSubstitutions\0"
    "bool\0subscribe\0variableAsToolTip\0"
    "enabled\0useDbEnumerations\0"
};

void QCaComboBoxPlugin::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        QCaComboBoxPlugin *_t = static_cast<QCaComboBoxPlugin *>(_o);
        switch (_id) {
        case 0: _t->useNewVariableNameProperty((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< uint(*)>(_a[3]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData QCaComboBoxPlugin::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QCaComboBoxPlugin::staticMetaObject = {
    { &QCaComboBox::staticMetaObject, qt_meta_stringdata_QCaComboBoxPlugin,
      qt_meta_data_QCaComboBoxPlugin, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QCaComboBoxPlugin::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QCaComboBoxPlugin::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QCaComboBoxPlugin::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QCaComboBoxPlugin))
        return static_cast<void*>(const_cast< QCaComboBoxPlugin*>(this));
    return QCaComboBox::qt_metacast(_clname);
}

int QCaComboBoxPlugin::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QCaComboBox::qt_metacall(_c, _id, _a);
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
        case 0: *reinterpret_cast< QString*>(_v) = getVariableNameProperty(); break;
        case 1: *reinterpret_cast< QString*>(_v) = getVariableNameSubstitutionsProperty(); break;
        case 2: *reinterpret_cast< bool*>(_v) = getSubscribe(); break;
        case 3: *reinterpret_cast< bool*>(_v) = getVariableAsToolTip(); break;
        case 4: *reinterpret_cast< bool*>(_v) = isEnabled(); break;
        case 5: *reinterpret_cast< bool*>(_v) = getUseDbEnumerations(); break;
        }
        _id -= 6;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setVariableNameProperty(*reinterpret_cast< QString*>(_v)); break;
        case 1: setVariableNameSubstitutionsProperty(*reinterpret_cast< QString*>(_v)); break;
        case 2: setSubscribe(*reinterpret_cast< bool*>(_v)); break;
        case 3: setVariableAsToolTip(*reinterpret_cast< bool*>(_v)); break;
        case 4: setEnabled(*reinterpret_cast< bool*>(_v)); break;
        case 5: setUseDbEnumerations(*reinterpret_cast< bool*>(_v)); break;
        }
        _id -= 6;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 6;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
