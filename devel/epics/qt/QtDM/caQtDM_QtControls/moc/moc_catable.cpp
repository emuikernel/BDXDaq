/****************************************************************************
** Meta object code from reading C++ file 'catable.h'
**
** Created: Wed Oct 2 14:11:28 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/catable.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'catable.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_caTable[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       8,   19, // properties
       2,   43, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
       9,    8,    8,    8, 0x08,

 // properties: name, type, flags
      24,   16, 0x0a095003,
      33,   16, 0x0a095103,
      53,   45, 0x0009510b,
      67,   63, 0x02095103,
      88,   77, 0x0009510b,
     102,   77, 0x0009510b,
     120,  113, 0x06095103,
     129,  113, 0x06095103,

 // enums: name, flags, count, data
      45, 0x0,    2,   51,
      77, 0x0,    2,   55,

 // enum data: key, value
     138, uint(caTable::Static),
     145, uint(caTable::Alarm),
     151, uint(caTable::Channel),
     159, uint(caTable::User),

       0        // eod
};

static const char qt_meta_stringdata_caTable[] = {
    "caTable\0\0copy()\0QString\0channels\0"
    "columnSizes\0colMode\0colorMode\0int\0"
    "precision\0SourceMode\0precisionMode\0"
    "limitsMode\0double\0maxValue\0minValue\0"
    "Static\0Alarm\0Channel\0User\0"
};

void caTable::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        caTable *_t = static_cast<caTable *>(_o);
        switch (_id) {
        case 0: _t->copy(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData caTable::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject caTable::staticMetaObject = {
    { &QTableWidget::staticMetaObject, qt_meta_stringdata_caTable,
      qt_meta_data_caTable, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &caTable::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *caTable::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *caTable::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_caTable))
        return static_cast<void*>(const_cast< caTable*>(this));
    return QTableWidget::qt_metacast(_clname);
}

int caTable::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTableWidget::qt_metacall(_c, _id, _a);
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
        case 0: *reinterpret_cast< QString*>(_v) = getPVS(); break;
        case 1: *reinterpret_cast< QString*>(_v) = getColumnSizes(); break;
        case 2: *reinterpret_cast< colMode*>(_v) = getColorMode(); break;
        case 3: *reinterpret_cast< int*>(_v) = getPrecision(); break;
        case 4: *reinterpret_cast< SourceMode*>(_v) = getPrecisionMode(); break;
        case 5: *reinterpret_cast< SourceMode*>(_v) = getLimitsMode(); break;
        case 6: *reinterpret_cast< double*>(_v) = getMaxValue(); break;
        case 7: *reinterpret_cast< double*>(_v) = getMinValue(); break;
        }
        _id -= 8;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setPVS(*reinterpret_cast< QString*>(_v)); break;
        case 1: setColumnSizes(*reinterpret_cast< QString*>(_v)); break;
        case 2: setColorMode(*reinterpret_cast< colMode*>(_v)); break;
        case 3: setPrecision(*reinterpret_cast< int*>(_v)); break;
        case 4: setPrecisionMode(*reinterpret_cast< SourceMode*>(_v)); break;
        case 5: setLimitsMode(*reinterpret_cast< SourceMode*>(_v)); break;
        case 6: setMaxValue(*reinterpret_cast< double*>(_v)); break;
        case 7: setMinValue(*reinterpret_cast< double*>(_v)); break;
        }
        _id -= 8;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 8;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 8;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 8;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 8;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 8;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 8;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
