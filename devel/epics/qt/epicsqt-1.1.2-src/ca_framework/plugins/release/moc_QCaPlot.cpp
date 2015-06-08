/****************************************************************************
** Meta object code from reading C++ file 'QCaPlot.h'
**
** Created: Wed Oct 16 16:50:34 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../widgets/include/QCaPlot.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QCaPlot.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QCaPlot[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      13,    9,    8,    8, 0x05,
      36,    9,    8,    8, 0x05,

 // slots: signature, parameters, type, tag, flags
      83,   68,    8,    8, 0x08,
     131,  121,    8,    8, 0x08,
     201,  192,    8,    8, 0x08,
     253,    8,    8,    8, 0x08,
     273,  267,    8,    8, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_QCaPlot[] = {
    "QCaPlot\0\0out\0dbValueChanged(double)\0"
    "dbValueChanged(QVector<double>)\0"
    "connectionInfo\0connectionChanged(QCaConnectionInfo&)\0"
    "values,,,\0"
    "setPlotData(QVector<double>,QCaAlarmInfo&,QCaDateTime&,uint)\0"
    "value,,,\0setPlotData(double,QCaAlarmInfo&,QCaDateTime&,uint)\0"
    "tickTimeout()\0state\0requestEnabled(bool)\0"
};

void QCaPlot::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        QCaPlot *_t = static_cast<QCaPlot *>(_o);
        switch (_id) {
        case 0: _t->dbValueChanged((*reinterpret_cast< const double(*)>(_a[1]))); break;
        case 1: _t->dbValueChanged((*reinterpret_cast< const QVector<double>(*)>(_a[1]))); break;
        case 2: _t->connectionChanged((*reinterpret_cast< QCaConnectionInfo(*)>(_a[1]))); break;
        case 3: _t->setPlotData((*reinterpret_cast< const QVector<double>(*)>(_a[1])),(*reinterpret_cast< QCaAlarmInfo(*)>(_a[2])),(*reinterpret_cast< QCaDateTime(*)>(_a[3])),(*reinterpret_cast< const uint(*)>(_a[4]))); break;
        case 4: _t->setPlotData((*reinterpret_cast< const double(*)>(_a[1])),(*reinterpret_cast< QCaAlarmInfo(*)>(_a[2])),(*reinterpret_cast< QCaDateTime(*)>(_a[3])),(*reinterpret_cast< const uint(*)>(_a[4]))); break;
        case 5: _t->tickTimeout(); break;
        case 6: _t->requestEnabled((*reinterpret_cast< const bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData QCaPlot::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QCaPlot::staticMetaObject = {
    { &QwtPlot::staticMetaObject, qt_meta_stringdata_QCaPlot,
      qt_meta_data_QCaPlot, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QCaPlot::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QCaPlot::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QCaPlot::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QCaPlot))
        return static_cast<void*>(const_cast< QCaPlot*>(this));
    if (!strcmp(_clname, "QCaWidget"))
        return static_cast< QCaWidget*>(const_cast< QCaPlot*>(this));
    return QwtPlot::qt_metacast(_clname);
}

int QCaPlot::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QwtPlot::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void QCaPlot::dbValueChanged(const double & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QCaPlot::dbValueChanged(const QVector<double> & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
