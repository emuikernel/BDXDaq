/****************************************************************************
** Meta object code from reading C++ file 'QCaSliderPlugin.h'
**
** Created: Wed Oct 16 16:50:00 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../include/QCaSliderPlugin.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QCaSliderPlugin.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QCaSliderPlugin[] = {

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
      74,   17,   16,   16, 0x08,

 // properties: name, type, flags
     131,  123, 0x0a095003,
     140,  123, 0x0a095003,
     167,  162, 0x01095103,
     175,  162, 0x01095103,
     189,  162, 0x01095103,
     199,  162, 0x01095103,

       0        // eod
};

static const char qt_meta_stringdata_QCaSliderPlugin[] = {
    "QCaSliderPlugin\0\0"
    "variableNameIn,variableNameSubstitutionsIn,variableIndex\0"
    "useNewVariableNameProperty(QString,QString,uint)\0"
    "QString\0variable\0variableSubstitutions\0"
    "bool\0enabled\0writeOnChange\0subscribe\0"
    "variableAsToolTip\0"
};

void QCaSliderPlugin::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        QCaSliderPlugin *_t = static_cast<QCaSliderPlugin *>(_o);
        switch (_id) {
        case 0: _t->useNewVariableNameProperty((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< uint(*)>(_a[3]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData QCaSliderPlugin::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QCaSliderPlugin::staticMetaObject = {
    { &QCaSlider::staticMetaObject, qt_meta_stringdata_QCaSliderPlugin,
      qt_meta_data_QCaSliderPlugin, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QCaSliderPlugin::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QCaSliderPlugin::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QCaSliderPlugin::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QCaSliderPlugin))
        return static_cast<void*>(const_cast< QCaSliderPlugin*>(this));
    return QCaSlider::qt_metacast(_clname);
}

int QCaSliderPlugin::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QCaSlider::qt_metacall(_c, _id, _a);
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
        case 2: *reinterpret_cast< bool*>(_v) = isEnabled(); break;
        case 3: *reinterpret_cast< bool*>(_v) = getWriteOnChange(); break;
        case 4: *reinterpret_cast< bool*>(_v) = getSubscribe(); break;
        case 5: *reinterpret_cast< bool*>(_v) = getVariableAsToolTip(); break;
        }
        _id -= 6;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setVariableNameProperty(*reinterpret_cast< QString*>(_v)); break;
        case 1: setVariableNameSubstitutionsProperty(*reinterpret_cast< QString*>(_v)); break;
        case 2: setEnabled(*reinterpret_cast< bool*>(_v)); break;
        case 3: setWriteOnChange(*reinterpret_cast< bool*>(_v)); break;
        case 4: setSubscribe(*reinterpret_cast< bool*>(_v)); break;
        case 5: setVariableAsToolTip(*reinterpret_cast< bool*>(_v)); break;
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
