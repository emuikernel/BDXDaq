/****************************************************************************
** Meta object code from reading C++ file 'eapplynumeric.h'
**
** Created: Wed Oct 2 14:11:14 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/eapplynumeric.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'eapplynumeric.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_EApplyNumeric[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       8,   49, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x05,
      37,   31,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
      58,   14,   14,   14, 0x0a,
      71,   14,   14,   14, 0x0a,
      87,   14,   14,   14, 0x0a,
     102,   14,   14,   14, 0x0a,
     112,   14,   14,   14, 0x08,

 // properties: name, type, flags
      31,  140, 0x06095103,
     151,  147, 0x02095003,
     165,  147, 0x02095003,
     195,  179, 0x0009500b,
     213,  205, 0x0a095103,
     230,  224, 0x40095103,
     247,  242, 0x01095103,
     265,  242, 0x01095103,

       0        // eod
};

static const char qt_meta_stringdata_EApplyNumeric[] = {
    "EApplyNumeric\0\0clicked(double)\0value\0"
    "valueChanged(double)\0applyValue()\0"
    "clearModified()\0setFont(QFont)\0disable()\0"
    "numericValueChanged(double)\0double\0"
    "int\0integerDigits\0decimalDigits\0"
    "Qt::Orientation\0buttonPos\0QString\0"
    "buttonText\0QFont\0numericFont\0bool\0"
    "applyButtonActive\0digitsFontScaleEnabled\0"
};

void EApplyNumeric::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        EApplyNumeric *_t = static_cast<EApplyNumeric *>(_o);
        switch (_id) {
        case 0: _t->clicked((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 1: _t->valueChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 2: _t->applyValue(); break;
        case 3: _t->clearModified(); break;
        case 4: _t->setFont((*reinterpret_cast< const QFont(*)>(_a[1]))); break;
        case 5: _t->disable(); break;
        case 6: _t->numericValueChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData EApplyNumeric::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject EApplyNumeric::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_EApplyNumeric,
      qt_meta_data_EApplyNumeric, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &EApplyNumeric::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *EApplyNumeric::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *EApplyNumeric::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_EApplyNumeric))
        return static_cast<void*>(const_cast< EApplyNumeric*>(this));
    if (!strcmp(_clname, "FloatDelegate"))
        return static_cast< FloatDelegate*>(const_cast< EApplyNumeric*>(this));
    return QWidget::qt_metacast(_clname);
}

int EApplyNumeric::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< double*>(_v) = value(); break;
        case 1: *reinterpret_cast< int*>(_v) = intDigits(); break;
        case 2: *reinterpret_cast< int*>(_v) = decDigits(); break;
        case 3: *reinterpret_cast< Qt::Orientation*>(_v) = buttonPosition(); break;
        case 4: *reinterpret_cast< QString*>(_v) = buttonText(); break;
        case 5: *reinterpret_cast< QFont*>(_v) = numericFont(); break;
        case 6: *reinterpret_cast< bool*>(_v) = applyButtonActive(); break;
        case 7: *reinterpret_cast< bool*>(_v) = digitsFontScaleEnabled(); break;
        }
        _id -= 8;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setValue(*reinterpret_cast< double*>(_v)); break;
        case 1: setIntDigits(*reinterpret_cast< int*>(_v)); break;
        case 2: setDecDigits(*reinterpret_cast< int*>(_v)); break;
        case 3: setButtonPosition(*reinterpret_cast< Qt::Orientation*>(_v)); break;
        case 4: setButtonText(*reinterpret_cast< QString*>(_v)); break;
        case 5: setNumericFont(*reinterpret_cast< QFont*>(_v)); break;
        case 6: setApplyButtonActive(*reinterpret_cast< bool*>(_v)); break;
        case 7: setDigitsFontScaleEnabled(*reinterpret_cast< bool*>(_v)); break;
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

// SIGNAL 0
void EApplyNumeric::clicked(double _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void EApplyNumeric::valueChanged(double _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
