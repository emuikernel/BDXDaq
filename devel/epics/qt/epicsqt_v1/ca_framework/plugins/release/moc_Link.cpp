/****************************************************************************
** Meta object code from reading C++ file 'Link.h'
**
** Created: Wed Oct 16 16:50:31 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../widgets/include/Link.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Link.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Link[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: signature, parameters, type, tag, flags
      10,    6,    5,    5, 0x05,
      20,    6,    5,    5, 0x05,
      35,    6,    5,    5, 0x05,
      47,    6,    5,    5, 0x05,

 // slots: signature, parameters, type, tag, flags
      63,   60,    5,    5, 0x0a,
      72,   60,    5,    5, 0x0a,
      86,   60,    5,    5, 0x0a,
      97,   60,    5,    5, 0x0a,
     116,  109,    5,    5, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_Link[] = {
    "Link\0\0out\0out(bool)\0out(qlonglong)\0"
    "out(double)\0out(QString)\0in\0in(bool)\0"
    "in(qlonglong)\0in(double)\0in(QString)\0"
    "enable\0autoFillBackground(bool)\0"
};

void Link::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        Link *_t = static_cast<Link *>(_o);
        switch (_id) {
        case 0: _t->out((*reinterpret_cast< const bool(*)>(_a[1]))); break;
        case 1: _t->out((*reinterpret_cast< const qlonglong(*)>(_a[1]))); break;
        case 2: _t->out((*reinterpret_cast< const double(*)>(_a[1]))); break;
        case 3: _t->out((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->in((*reinterpret_cast< const bool(*)>(_a[1]))); break;
        case 5: _t->in((*reinterpret_cast< const qlonglong(*)>(_a[1]))); break;
        case 6: _t->in((*reinterpret_cast< const double(*)>(_a[1]))); break;
        case 7: _t->in((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 8: _t->autoFillBackground((*reinterpret_cast< const bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData Link::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Link::staticMetaObject = {
    { &QLabel::staticMetaObject, qt_meta_stringdata_Link,
      qt_meta_data_Link, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Link::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Link::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Link::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Link))
        return static_cast<void*>(const_cast< Link*>(this));
    return QLabel::qt_metacast(_clname);
}

int Link::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QLabel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void Link::out(const bool & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Link::out(const qlonglong & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Link::out(const double & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void Link::out(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_END_MOC_NAMESPACE
