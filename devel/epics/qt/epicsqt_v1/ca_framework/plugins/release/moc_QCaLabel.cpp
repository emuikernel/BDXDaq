/****************************************************************************
** Meta object code from reading C++ file 'QCaLabel.h'
**
** Created: Wed Oct 16 16:50:30 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../widgets/include/QCaLabel.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QCaLabel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QCaLabel[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      14,   10,    9,    9, 0x05,
      38,    9,    9,    9, 0x05,

 // slots: signature, parameters, type, tag, flags
      69,   54,    9,    9, 0x08,
     115,  107,    9,    9, 0x08,
     175,  169,    9,    9, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_QCaLabel[] = {
    "QCaLabel\0\0out\0dbValueChanged(QString)\0"
    "requestResend()\0connectionInfo\0"
    "connectionChanged(QCaConnectionInfo&)\0"
    "text,,,\0setLabelText(QString,QCaAlarmInfo&,QCaDateTime&,uint)\0"
    "state\0requestEnabled(bool)\0"
};

void QCaLabel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        QCaLabel *_t = static_cast<QCaLabel *>(_o);
        switch (_id) {
        case 0: _t->dbValueChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->requestResend(); break;
        case 2: _t->connectionChanged((*reinterpret_cast< QCaConnectionInfo(*)>(_a[1]))); break;
        case 3: _t->setLabelText((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< QCaAlarmInfo(*)>(_a[2])),(*reinterpret_cast< QCaDateTime(*)>(_a[3])),(*reinterpret_cast< const uint(*)>(_a[4]))); break;
        case 4: _t->requestEnabled((*reinterpret_cast< const bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData QCaLabel::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QCaLabel::staticMetaObject = {
    { &QLabel::staticMetaObject, qt_meta_stringdata_QCaLabel,
      qt_meta_data_QCaLabel, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QCaLabel::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QCaLabel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QCaLabel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QCaLabel))
        return static_cast<void*>(const_cast< QCaLabel*>(this));
    if (!strcmp(_clname, "QCaWidget"))
        return static_cast< QCaWidget*>(const_cast< QCaLabel*>(this));
    return QLabel::qt_metacast(_clname);
}

int QCaLabel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QLabel::qt_metacall(_c, _id, _a);
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
void QCaLabel::dbValueChanged(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QCaLabel::requestResend()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
QT_END_MOC_NAMESPACE
