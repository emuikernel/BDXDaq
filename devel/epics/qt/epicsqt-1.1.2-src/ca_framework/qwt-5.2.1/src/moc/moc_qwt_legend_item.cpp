/****************************************************************************
** Meta object code from reading C++ file 'qwt_legend_item.h'
**
** Created: Wed Oct 16 16:47:45 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../qwt_legend_item.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qwt_legend_item.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QwtLegendItem[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x05,
      25,   14,   14,   14, 0x05,
      35,   14,   14,   14, 0x05,
      46,   14,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
      63,   60,   14,   14, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_QwtLegendItem[] = {
    "QwtLegendItem\0\0clicked()\0pressed()\0"
    "released()\0checked(bool)\0on\0"
    "setChecked(bool)\0"
};

void QwtLegendItem::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        QwtLegendItem *_t = static_cast<QwtLegendItem *>(_o);
        switch (_id) {
        case 0: _t->clicked(); break;
        case 1: _t->pressed(); break;
        case 2: _t->released(); break;
        case 3: _t->checked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->setChecked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData QwtLegendItem::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QwtLegendItem::staticMetaObject = {
    { &QwtTextLabel::staticMetaObject, qt_meta_stringdata_QwtLegendItem,
      qt_meta_data_QwtLegendItem, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QwtLegendItem::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QwtLegendItem::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QwtLegendItem::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QwtLegendItem))
        return static_cast<void*>(const_cast< QwtLegendItem*>(this));
    return QwtTextLabel::qt_metacast(_clname);
}

int QwtLegendItem::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QwtTextLabel::qt_metacall(_c, _id, _a);
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
void QwtLegendItem::clicked()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void QwtLegendItem::pressed()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void QwtLegendItem::released()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void QwtLegendItem::checked(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_END_MOC_NAMESPACE
