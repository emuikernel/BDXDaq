/****************************************************************************
** Meta object code from reading C++ file 'QCaLineEditPlugin.h'
**
** Created: Wed Oct 16 16:50:05 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../include/QCaLineEditPlugin.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QCaLineEditPlugin.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QCaLineEditPlugin[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
      16,   19, // properties
       2,   67, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      76,   19,   18,   18, 0x08,

 // properties: name, type, flags
     133,  125, 0x0a095003,
     142,  125, 0x0a095003,
     169,  164, 0x01095103,
     186,  164, 0x01095103,
     199,  164, 0x01095103,
     209,  164, 0x01095103,
     227,  164, 0x01095103,
     235,  164, 0x01095103,
     253,  248, 0x03095103,
     263,  164, 0x01095103,
     278,  164, 0x01095103,
     290,  164, 0x01095103,
     304,  164, 0x01095103,
     321,  313, 0x0009500b,
     328,  248, 0x03095103,
     344,  334, 0x0009500b,

 // enums: name, flags, count, data
     313, 0x0,    5,   75,
     334, 0x0,    3,   85,

 // enum data: key, value
     353, uint(QCaLineEditPlugin::Default),
     361, uint(QCaLineEditPlugin::Floating),
     370, uint(QCaLineEditPlugin::Integer),
     378, uint(QCaLineEditPlugin::UnsignedInteger),
     394, uint(QCaLineEditPlugin::Time),
     399, uint(QCaLineEditPlugin::Fixed),
     405, uint(QCaLineEditPlugin::Scientific),
     416, uint(QCaLineEditPlugin::Automatic),

       0        // eod
};

static const char qt_meta_stringdata_QCaLineEditPlugin[] = {
    "QCaLineEditPlugin\0\0"
    "variableNameIn,variableNameSubstitutionsIn,variableIndex\0"
    "useNewVariableNameProperty(QString,QString,uint)\0"
    "QString\0variable\0variableSubstitutions\0"
    "bool\0writeOnLoseFocus\0writeOnEnter\0"
    "subscribe\0variableAsToolTip\0enabled\0"
    "confirmWrite\0uint\0precision\0useDbPrecision\0"
    "leadingZero\0trailingZeros\0addUnits\0"
    "Formats\0format\0radix\0Notations\0notation\0"
    "Default\0Floating\0Integer\0UnsignedInteger\0"
    "Time\0Fixed\0Scientific\0Automatic\0"
};

void QCaLineEditPlugin::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        QCaLineEditPlugin *_t = static_cast<QCaLineEditPlugin *>(_o);
        switch (_id) {
        case 0: _t->useNewVariableNameProperty((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< uint(*)>(_a[3]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData QCaLineEditPlugin::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QCaLineEditPlugin::staticMetaObject = {
    { &QCaLineEdit::staticMetaObject, qt_meta_stringdata_QCaLineEditPlugin,
      qt_meta_data_QCaLineEditPlugin, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QCaLineEditPlugin::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QCaLineEditPlugin::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QCaLineEditPlugin::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QCaLineEditPlugin))
        return static_cast<void*>(const_cast< QCaLineEditPlugin*>(this));
    return QCaLineEdit::qt_metacast(_clname);
}

int QCaLineEditPlugin::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QCaLineEdit::qt_metacall(_c, _id, _a);
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
        case 2: *reinterpret_cast< bool*>(_v) = getWriteOnLoseFocus(); break;
        case 3: *reinterpret_cast< bool*>(_v) = getWriteOnEnter(); break;
        case 4: *reinterpret_cast< bool*>(_v) = getSubscribe(); break;
        case 5: *reinterpret_cast< bool*>(_v) = getVariableAsToolTip(); break;
        case 6: *reinterpret_cast< bool*>(_v) = isEnabled(); break;
        case 7: *reinterpret_cast< bool*>(_v) = getConfirmWrite(); break;
        case 8: *reinterpret_cast< uint*>(_v) = getPrecision(); break;
        case 9: *reinterpret_cast< bool*>(_v) = getUseDbPrecision(); break;
        case 10: *reinterpret_cast< bool*>(_v) = getLeadingZero(); break;
        case 11: *reinterpret_cast< bool*>(_v) = getTrailingZeros(); break;
        case 12: *reinterpret_cast< bool*>(_v) = getAddUnits(); break;
        case 13: *reinterpret_cast< Formats*>(_v) = getFormatProperty(); break;
        case 14: *reinterpret_cast< uint*>(_v) = getRadix(); break;
        case 15: *reinterpret_cast< Notations*>(_v) = getNotationProperty(); break;
        }
        _id -= 16;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setVariableNameProperty(*reinterpret_cast< QString*>(_v)); break;
        case 1: setVariableNameSubstitutionsProperty(*reinterpret_cast< QString*>(_v)); break;
        case 2: setWriteOnLoseFocus(*reinterpret_cast< bool*>(_v)); break;
        case 3: setWriteOnEnter(*reinterpret_cast< bool*>(_v)); break;
        case 4: setSubscribe(*reinterpret_cast< bool*>(_v)); break;
        case 5: setVariableAsToolTip(*reinterpret_cast< bool*>(_v)); break;
        case 6: setEnabled(*reinterpret_cast< bool*>(_v)); break;
        case 7: setConfirmWrite(*reinterpret_cast< bool*>(_v)); break;
        case 8: setPrecision(*reinterpret_cast< uint*>(_v)); break;
        case 9: setUseDbPrecision(*reinterpret_cast< bool*>(_v)); break;
        case 10: setLeadingZero(*reinterpret_cast< bool*>(_v)); break;
        case 11: setTrailingZeros(*reinterpret_cast< bool*>(_v)); break;
        case 12: setAddUnits(*reinterpret_cast< bool*>(_v)); break;
        case 13: setFormatProperty(*reinterpret_cast< Formats*>(_v)); break;
        case 14: setRadix(*reinterpret_cast< uint*>(_v)); break;
        case 15: setNotationProperty(*reinterpret_cast< Notations*>(_v)); break;
        }
        _id -= 16;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 16;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 16;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 16;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 16;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 16;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 16;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
