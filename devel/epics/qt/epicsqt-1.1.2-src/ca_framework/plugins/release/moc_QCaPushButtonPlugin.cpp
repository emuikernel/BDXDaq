/****************************************************************************
** Meta object code from reading C++ file 'QCaPushButtonPlugin.h'
**
** Created: Wed Oct 16 16:50:03 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../include/QCaPushButtonPlugin.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QCaPushButtonPlugin.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QCaPushButtonPlugin[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
      18,   19, // properties
       2,   73, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      78,   21,   20,   20, 0x08,

 // properties: name, type, flags
     135,  127, 0x0a095003,
     144,  127, 0x0a095003,
     171,  166, 0x01095103,
     181,  166, 0x01095103,
     199,  166, 0x01095103,
     212,  207, 0x03095103,
     222,  166, 0x01095103,
     237,  166, 0x01095103,
     249,  166, 0x01095103,
     263,  166, 0x01095103,
     280,  272, 0x0009500b,
     297,  287, 0x0009500b,
     306,  166, 0x01095103,
     319,  166, 0x01095103,
     334,  166, 0x01095103,
     347,  127, 0x0a095103,
     357,  127, 0x0a095103,
     369,  127, 0x0a095103,

 // enums: name, flags, count, data
     272, 0x0,    5,   81,
     287, 0x0,    3,   91,

 // enum data: key, value
     379, uint(QCaPushButtonPlugin::Default),
     387, uint(QCaPushButtonPlugin::Floating),
     396, uint(QCaPushButtonPlugin::Integer),
     404, uint(QCaPushButtonPlugin::UnsignedInteger),
     420, uint(QCaPushButtonPlugin::Time),
     425, uint(QCaPushButtonPlugin::Fixed),
     431, uint(QCaPushButtonPlugin::Scientific),
     442, uint(QCaPushButtonPlugin::Automatic),

       0        // eod
};

static const char qt_meta_stringdata_QCaPushButtonPlugin[] = {
    "QCaPushButtonPlugin\0\0"
    "variableNameIn,variableNameSubstitutionsIn,variableIndex\0"
    "useNewVariableNameProperty(QString,QString,uint)\0"
    "QString\0variable\0variableSubstitutions\0"
    "bool\0subscribe\0variableAsToolTip\0"
    "enabled\0uint\0precision\0useDbPrecision\0"
    "leadingZero\0trailingZeros\0addUnits\0"
    "Formats\0format\0Notations\0notation\0"
    "writeOnPress\0writeOnRelease\0writeOnClick\0"
    "pressText\0releaseText\0clickText\0Default\0"
    "Floating\0Integer\0UnsignedInteger\0Time\0"
    "Fixed\0Scientific\0Automatic\0"
};

void QCaPushButtonPlugin::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        QCaPushButtonPlugin *_t = static_cast<QCaPushButtonPlugin *>(_o);
        switch (_id) {
        case 0: _t->useNewVariableNameProperty((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< uint(*)>(_a[3]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData QCaPushButtonPlugin::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QCaPushButtonPlugin::staticMetaObject = {
    { &QCaPushButton::staticMetaObject, qt_meta_stringdata_QCaPushButtonPlugin,
      qt_meta_data_QCaPushButtonPlugin, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QCaPushButtonPlugin::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QCaPushButtonPlugin::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QCaPushButtonPlugin::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QCaPushButtonPlugin))
        return static_cast<void*>(const_cast< QCaPushButtonPlugin*>(this));
    return QCaPushButton::qt_metacast(_clname);
}

int QCaPushButtonPlugin::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QCaPushButton::qt_metacall(_c, _id, _a);
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
        case 5: *reinterpret_cast< uint*>(_v) = getPrecision(); break;
        case 6: *reinterpret_cast< bool*>(_v) = getUseDbPrecision(); break;
        case 7: *reinterpret_cast< bool*>(_v) = getLeadingZero(); break;
        case 8: *reinterpret_cast< bool*>(_v) = getTrailingZeros(); break;
        case 9: *reinterpret_cast< bool*>(_v) = getAddUnits(); break;
        case 10: *reinterpret_cast< Formats*>(_v) = getFormatProperty(); break;
        case 11: *reinterpret_cast< Notations*>(_v) = getNotationProperty(); break;
        case 12: *reinterpret_cast< bool*>(_v) = getWriteOnPress(); break;
        case 13: *reinterpret_cast< bool*>(_v) = getWriteOnRelease(); break;
        case 14: *reinterpret_cast< bool*>(_v) = getWriteOnClick(); break;
        case 15: *reinterpret_cast< QString*>(_v) = getPressText(); break;
        case 16: *reinterpret_cast< QString*>(_v) = getReleaseText(); break;
        case 17: *reinterpret_cast< QString*>(_v) = getClickText(); break;
        }
        _id -= 18;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setVariableNameProperty(*reinterpret_cast< QString*>(_v)); break;
        case 1: setVariableNameSubstitutionsProperty(*reinterpret_cast< QString*>(_v)); break;
        case 2: setSubscribe(*reinterpret_cast< bool*>(_v)); break;
        case 3: setVariableAsToolTip(*reinterpret_cast< bool*>(_v)); break;
        case 4: setEnabled(*reinterpret_cast< bool*>(_v)); break;
        case 5: setPrecision(*reinterpret_cast< uint*>(_v)); break;
        case 6: setUseDbPrecision(*reinterpret_cast< bool*>(_v)); break;
        case 7: setLeadingZero(*reinterpret_cast< bool*>(_v)); break;
        case 8: setTrailingZeros(*reinterpret_cast< bool*>(_v)); break;
        case 9: setAddUnits(*reinterpret_cast< bool*>(_v)); break;
        case 10: setFormatProperty(*reinterpret_cast< Formats*>(_v)); break;
        case 11: setNotationProperty(*reinterpret_cast< Notations*>(_v)); break;
        case 12: setWriteOnPress(*reinterpret_cast< bool*>(_v)); break;
        case 13: setWriteOnRelease(*reinterpret_cast< bool*>(_v)); break;
        case 14: setWriteOnClick(*reinterpret_cast< bool*>(_v)); break;
        case 15: setPressText(*reinterpret_cast< QString*>(_v)); break;
        case 16: setReleaseText(*reinterpret_cast< QString*>(_v)); break;
        case 17: setClickText(*reinterpret_cast< QString*>(_v)); break;
        }
        _id -= 18;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 18;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 18;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 18;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 18;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 18;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 18;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
