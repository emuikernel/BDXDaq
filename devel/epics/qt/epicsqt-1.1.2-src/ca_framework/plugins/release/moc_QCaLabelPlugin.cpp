/****************************************************************************
** Meta object code from reading C++ file 'QCaLabelPlugin.h'
**
** Created: Wed Oct 16 16:50:14 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../include/QCaLabelPlugin.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QCaLabelPlugin.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QCaLabelPlugin[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
      14,   19, // properties
       2,   61, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      73,   16,   15,   15, 0x08,

 // properties: name, type, flags
     130,  122, 0x0a095003,
     139,  122, 0x0a095003,
     166,  161, 0x01095103,
     184,  161, 0x01095103,
     197,  192, 0x03095103,
     207,  161, 0x01095103,
     222,  161, 0x01095103,
     234,  161, 0x01095103,
     248,  161, 0x01095103,
     257,  122, 0x0a095103,
     282,  274, 0x0009500b,
     289,  192, 0x03095103,
     305,  295, 0x0009500b,
     314,  161, 0x01095003,

 // enums: name, flags, count, data
     274, 0x0,    6,   69,
     295, 0x0,    3,   81,

 // enum data: key, value
     322, uint(QCaLabelPlugin::Default),
     330, uint(QCaLabelPlugin::Floating),
     339, uint(QCaLabelPlugin::Integer),
     347, uint(QCaLabelPlugin::UnsignedInteger),
     363, uint(QCaLabelPlugin::Time),
     368, uint(QCaLabelPlugin::LocalEnumeration),
     385, uint(QCaLabelPlugin::Fixed),
     391, uint(QCaLabelPlugin::Scientific),
     402, uint(QCaLabelPlugin::Automatic),

       0        // eod
};

static const char qt_meta_stringdata_QCaLabelPlugin[] = {
    "QCaLabelPlugin\0\0"
    "variableNameIn,variableNameSubstitutionsIn,variableIndex\0"
    "useNewVariableNameProperty(QString,QString,uint)\0"
    "QString\0variable\0variableSubstitutions\0"
    "bool\0variableAsToolTip\0enabled\0uint\0"
    "precision\0useDbPrecision\0leadingZero\0"
    "trailingZeros\0addUnits\0localEnumeration\0"
    "Formats\0format\0radix\0Notations\0notation\0"
    "visible\0Default\0Floating\0Integer\0"
    "UnsignedInteger\0Time\0LocalEnumeration\0"
    "Fixed\0Scientific\0Automatic\0"
};

void QCaLabelPlugin::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        QCaLabelPlugin *_t = static_cast<QCaLabelPlugin *>(_o);
        switch (_id) {
        case 0: _t->useNewVariableNameProperty((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< uint(*)>(_a[3]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData QCaLabelPlugin::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QCaLabelPlugin::staticMetaObject = {
    { &QCaLabel::staticMetaObject, qt_meta_stringdata_QCaLabelPlugin,
      qt_meta_data_QCaLabelPlugin, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QCaLabelPlugin::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QCaLabelPlugin::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QCaLabelPlugin::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QCaLabelPlugin))
        return static_cast<void*>(const_cast< QCaLabelPlugin*>(this));
    return QCaLabel::qt_metacast(_clname);
}

int QCaLabelPlugin::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QCaLabel::qt_metacall(_c, _id, _a);
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
        case 2: *reinterpret_cast< bool*>(_v) = getVariableAsToolTip(); break;
        case 3: *reinterpret_cast< bool*>(_v) = isEnabled(); break;
        case 4: *reinterpret_cast< uint*>(_v) = getPrecision(); break;
        case 5: *reinterpret_cast< bool*>(_v) = getUseDbPrecision(); break;
        case 6: *reinterpret_cast< bool*>(_v) = getLeadingZero(); break;
        case 7: *reinterpret_cast< bool*>(_v) = getTrailingZeros(); break;
        case 8: *reinterpret_cast< bool*>(_v) = getAddUnits(); break;
        case 9: *reinterpret_cast< QString*>(_v) = getLocalEnumeration(); break;
        case 10: *reinterpret_cast< Formats*>(_v) = getFormatProperty(); break;
        case 11: *reinterpret_cast< uint*>(_v) = getRadix(); break;
        case 12: *reinterpret_cast< Notations*>(_v) = getNotationProperty(); break;
        case 13: *reinterpret_cast< bool*>(_v) = getRunVisible(); break;
        }
        _id -= 14;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setVariableNameProperty(*reinterpret_cast< QString*>(_v)); break;
        case 1: setVariableNameSubstitutionsProperty(*reinterpret_cast< QString*>(_v)); break;
        case 2: setVariableAsToolTip(*reinterpret_cast< bool*>(_v)); break;
        case 3: setEnabled(*reinterpret_cast< bool*>(_v)); break;
        case 4: setPrecision(*reinterpret_cast< uint*>(_v)); break;
        case 5: setUseDbPrecision(*reinterpret_cast< bool*>(_v)); break;
        case 6: setLeadingZero(*reinterpret_cast< bool*>(_v)); break;
        case 7: setTrailingZeros(*reinterpret_cast< bool*>(_v)); break;
        case 8: setAddUnits(*reinterpret_cast< bool*>(_v)); break;
        case 9: setLocalEnumeration(*reinterpret_cast< QString*>(_v)); break;
        case 10: setFormatProperty(*reinterpret_cast< Formats*>(_v)); break;
        case 11: setRadix(*reinterpret_cast< uint*>(_v)); break;
        case 12: setNotationProperty(*reinterpret_cast< Notations*>(_v)); break;
        case 13: setRunVisible(*reinterpret_cast< bool*>(_v)); break;
        }
        _id -= 14;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 14;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 14;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 14;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 14;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 14;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 14;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
