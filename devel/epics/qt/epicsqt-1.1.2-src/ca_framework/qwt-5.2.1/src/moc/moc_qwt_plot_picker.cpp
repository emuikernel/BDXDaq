/****************************************************************************
** Meta object code from reading C++ file 'qwt_plot_picker.h'
**
** Created: Wed Oct 16 16:47:48 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../qwt_plot_picker.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qwt_plot_picker.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QwtPlotPicker[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: signature, parameters, type, tag, flags
      19,   15,   14,   14, 0x05,
      49,   44,   14,   14, 0x05,
      76,   73,   14,   14, 0x05,
     111,   15,   14,   14, 0x05,
     136,   15,   14,   14, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_QwtPlotPicker[] = {
    "QwtPlotPicker\0\0pos\0selected(QwtDoublePoint)\0"
    "rect\0selected(QwtDoubleRect)\0pa\0"
    "selected(QwtArray<QwtDoublePoint>)\0"
    "appended(QwtDoublePoint)\0moved(QwtDoublePoint)\0"
};

void QwtPlotPicker::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        QwtPlotPicker *_t = static_cast<QwtPlotPicker *>(_o);
        switch (_id) {
        case 0: _t->selected((*reinterpret_cast< const QwtDoublePoint(*)>(_a[1]))); break;
        case 1: _t->selected((*reinterpret_cast< const QwtDoubleRect(*)>(_a[1]))); break;
        case 2: _t->selected((*reinterpret_cast< const QwtArray<QwtDoublePoint>(*)>(_a[1]))); break;
        case 3: _t->appended((*reinterpret_cast< const QwtDoublePoint(*)>(_a[1]))); break;
        case 4: _t->moved((*reinterpret_cast< const QwtDoublePoint(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData QwtPlotPicker::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QwtPlotPicker::staticMetaObject = {
    { &QwtPicker::staticMetaObject, qt_meta_stringdata_QwtPlotPicker,
      qt_meta_data_QwtPlotPicker, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QwtPlotPicker::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QwtPlotPicker::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QwtPlotPicker::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QwtPlotPicker))
        return static_cast<void*>(const_cast< QwtPlotPicker*>(this));
    return QwtPicker::qt_metacast(_clname);
}

int QwtPlotPicker::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QwtPicker::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void QwtPlotPicker::selected(const QwtDoublePoint & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QwtPlotPicker::selected(const QwtDoubleRect & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QwtPlotPicker::selected(const QwtArray<QwtDoublePoint> & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void QwtPlotPicker::appended(const QwtDoublePoint & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void QwtPlotPicker::moved(const QwtDoublePoint & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_END_MOC_NAMESPACE
