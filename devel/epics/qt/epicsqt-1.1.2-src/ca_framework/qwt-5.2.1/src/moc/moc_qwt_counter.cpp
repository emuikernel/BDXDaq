/****************************************************************************
** Meta object code from reading C++ file 'qwt_counter.h'
**
** Created: Wed Oct 16 16:47:54 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../qwt_counter.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qwt_counter.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QwtCounter[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       9,   39, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      18,   12,   11,   11, 0x05,
      41,   12,   11,   11, 0x05,

 // slots: signature, parameters, type, tag, flags
      62,   11,   11,   11, 0x08,
      76,   11,   11,   11, 0x08,
      89,   11,   11,   11, 0x08,

 // properties: name, type, flags
     107,  103, 0x02095103,
     125,  118, 0x06095003,
     135,  118, 0x06095103,
     144,  118, 0x06095103,
     153,  103, 0x02095103,
     165,  103, 0x02095103,
     177,  103, 0x02095103,
      12,  118, 0x06095103,
     194,  189, 0x01095103,

       0        // eod
};

static const char qt_meta_stringdata_QwtCounter[] = {
    "QwtCounter\0\0value\0buttonReleased(double)\0"
    "valueChanged(double)\0btnReleased()\0"
    "btnClicked()\0textChanged()\0int\0"
    "numButtons\0double\0basicstep\0minValue\0"
    "maxValue\0stepButton1\0stepButton2\0"
    "stepButton3\0bool\0editable\0"
};

void QwtCounter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        QwtCounter *_t = static_cast<QwtCounter *>(_o);
        switch (_id) {
        case 0: _t->buttonReleased((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 1: _t->valueChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 2: _t->btnReleased(); break;
        case 3: _t->btnClicked(); break;
        case 4: _t->textChanged(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData QwtCounter::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QwtCounter::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QwtCounter,
      qt_meta_data_QwtCounter, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QwtCounter::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QwtCounter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QwtCounter::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QwtCounter))
        return static_cast<void*>(const_cast< QwtCounter*>(this));
    if (!strcmp(_clname, "QwtDoubleRange"))
        return static_cast< QwtDoubleRange*>(const_cast< QwtCounter*>(this));
    return QWidget::qt_metacast(_clname);
}

int QwtCounter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = numButtons(); break;
        case 1: *reinterpret_cast< double*>(_v) = step(); break;
        case 2: *reinterpret_cast< double*>(_v) = minVal(); break;
        case 3: *reinterpret_cast< double*>(_v) = maxVal(); break;
        case 4: *reinterpret_cast< int*>(_v) = stepButton1(); break;
        case 5: *reinterpret_cast< int*>(_v) = stepButton2(); break;
        case 6: *reinterpret_cast< int*>(_v) = stepButton3(); break;
        case 7: *reinterpret_cast< double*>(_v) = value(); break;
        case 8: *reinterpret_cast< bool*>(_v) = editable(); break;
        }
        _id -= 9;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setNumButtons(*reinterpret_cast< int*>(_v)); break;
        case 1: setStep(*reinterpret_cast< double*>(_v)); break;
        case 2: setMinValue(*reinterpret_cast< double*>(_v)); break;
        case 3: setMaxValue(*reinterpret_cast< double*>(_v)); break;
        case 4: setStepButton1(*reinterpret_cast< int*>(_v)); break;
        case 5: setStepButton2(*reinterpret_cast< int*>(_v)); break;
        case 6: setStepButton3(*reinterpret_cast< int*>(_v)); break;
        case 7: setValue(*reinterpret_cast< double*>(_v)); break;
        case 8: setEditable(*reinterpret_cast< bool*>(_v)); break;
        }
        _id -= 9;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 9;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 9;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 9;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 9;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 9;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 9;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void QwtCounter::buttonReleased(double _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QwtCounter::valueChanged(double _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
