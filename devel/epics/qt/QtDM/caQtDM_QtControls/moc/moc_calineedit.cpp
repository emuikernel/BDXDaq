/****************************************************************************
** Meta object code from reading C++ file 'calineedit.h'
**
** Created: Wed Oct 2 14:11:19 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/calineedit.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'calineedit.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_caLineEdit[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
      14,   19, // properties
       4,   61, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      20,   12,   11,   11, 0x08,

 // properties: name, type, flags
      49,   41, 0x0a095003,
      64,   57, 0x43095103,
      75,   57, 0x43095103,
      94,   86, 0x0009510b,
     108,  104, 0x02095103,
     129,  118, 0x0009510b,
     143,  118, 0x0009510b,
     161,  154, 0x06095103,
     170,  154, 0x06095103,
     184,  179, 0x01094001,
     211,  201, 0x0009510b,
     225,  154, 0x06094103,
     241,  179, 0x01095103,
     265,  254, 0x0009510b,

 // enums: name, flags, count, data
      86, 0x0,    4,   77,
     118, 0x0,    2,   85,
     254, 0x0,   11,   89,
     201, 0x0,    3,  111,

 // enum data: key, value
     276, uint(caLineEdit::Default),
     284, uint(caLineEdit::Static),
     291, uint(caLineEdit::Alarm_Default),
     305, uint(caLineEdit::Alarm_Static),
     318, uint(caLineEdit::Channel),
     326, uint(caLineEdit::User),
     331, uint(caLineEdit::decimal),
     339, uint(caLineEdit::exponential),
     351, uint(caLineEdit::engr_notation),
     365, uint(caLineEdit::compact),
     373, uint(caLineEdit::truncated),
     383, uint(caLineEdit::hexadecimal),
     395, uint(caLineEdit::octal),
     401, uint(caLineEdit::string),
     408, uint(caLineEdit::sexagesimal),
     420, uint(caLineEdit::sexagesimal_hms),
     436, uint(caLineEdit::sexagesimal_dms),
     452, uint(caLineEdit::None),
     457, uint(caLineEdit::Height),
     464, uint(caLineEdit::WidthAndHeight),

       0        // eod
};

static const char qt_meta_stringdata_caLineEdit[] = {
    "caLineEdit\0\0newText\0rescaleFont(QString)\0"
    "QString\0channel\0QColor\0foreground\0"
    "background\0colMode\0colorMode\0int\0"
    "precision\0SourceMode\0precisionMode\0"
    "limitsMode\0double\0maxValue\0minValue\0"
    "bool\0fontScaleEnabled\0ScaleMode\0"
    "fontScaleMode\0fontScaleFactor\0"
    "unitsEnabled\0FormatType\0formatType\0"
    "Default\0Static\0Alarm_Default\0Alarm_Static\0"
    "Channel\0User\0decimal\0exponential\0"
    "engr_notation\0compact\0truncated\0"
    "hexadecimal\0octal\0string\0sexagesimal\0"
    "sexagesimal_hms\0sexagesimal_dms\0None\0"
    "Height\0WidthAndHeight\0"
};

void caLineEdit::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        caLineEdit *_t = static_cast<caLineEdit *>(_o);
        switch (_id) {
        case 0: _t->rescaleFont((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData caLineEdit::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject caLineEdit::staticMetaObject = {
    { &QLineEdit::staticMetaObject, qt_meta_stringdata_caLineEdit,
      qt_meta_data_caLineEdit, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &caLineEdit::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *caLineEdit::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *caLineEdit::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_caLineEdit))
        return static_cast<void*>(const_cast< caLineEdit*>(this));
    if (!strcmp(_clname, "FontScalingWidget"))
        return static_cast< FontScalingWidget*>(const_cast< caLineEdit*>(this));
    return QLineEdit::qt_metacast(_clname);
}

int caLineEdit::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QLineEdit::qt_metacall(_c, _id, _a);
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
        case 0: *reinterpret_cast< QString*>(_v) = getPV(); break;
        case 1: *reinterpret_cast< QColor*>(_v) = getForeground(); break;
        case 2: *reinterpret_cast< QColor*>(_v) = getBackground(); break;
        case 3: *reinterpret_cast< colMode*>(_v) = getColorMode(); break;
        case 4: *reinterpret_cast< int*>(_v) = getPrecision(); break;
        case 5: *reinterpret_cast< SourceMode*>(_v) = getPrecisionMode(); break;
        case 6: *reinterpret_cast< SourceMode*>(_v) = getLimitsMode(); break;
        case 7: *reinterpret_cast< double*>(_v) = getMaxValue(); break;
        case 8: *reinterpret_cast< double*>(_v) = getMinValue(); break;
        case 9: *reinterpret_cast< bool*>(_v) = fontScaleEnabled(); break;
        case 10: *reinterpret_cast< ScaleMode*>(_v) = fontScaleMode(); break;
        case 11: *reinterpret_cast< double*>(_v) = fontScaleFactor(); break;
        case 12: *reinterpret_cast< bool*>(_v) = getUnitsEnabled(); break;
        case 13: *reinterpret_cast< FormatType*>(_v) = getFormatType(); break;
        }
        _id -= 14;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setPV(*reinterpret_cast< QString*>(_v)); break;
        case 1: setForeground(*reinterpret_cast< QColor*>(_v)); break;
        case 2: setBackground(*reinterpret_cast< QColor*>(_v)); break;
        case 3: setColorMode(*reinterpret_cast< colMode*>(_v)); break;
        case 4: setPrecision(*reinterpret_cast< int*>(_v)); break;
        case 5: setPrecisionMode(*reinterpret_cast< SourceMode*>(_v)); break;
        case 6: setLimitsMode(*reinterpret_cast< SourceMode*>(_v)); break;
        case 7: setMaxValue(*reinterpret_cast< double*>(_v)); break;
        case 8: setMinValue(*reinterpret_cast< double*>(_v)); break;
        case 10: setFontScaleMode(*reinterpret_cast< ScaleMode*>(_v)); break;
        case 11: setFontScaleFactor(*reinterpret_cast< double*>(_v)); break;
        case 12: setUnitsEnabled(*reinterpret_cast< bool*>(_v)); break;
        case 13: setFormatType(*reinterpret_cast< FormatType*>(_v)); break;
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
