/****************************************************************************
** Meta object code from reading C++ file 'castripplotthread.h'
**
** Created: Mon Sep 16 16:38:53 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/castripplotthread.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'castripplotthread.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_castripplotthread[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      19,   18,   18,   18, 0x05,

 // slots: signature, parameters, type, tag, flags
      28,   18,   18,   18, 0x08,
      38,   18,   18,   18, 0x08,
      48,   18,   18,   18, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_castripplotthread[] = {
    "castripplotthread\0\0update()\0TimeOut()\0"
    "runStop()\0quitFromOtherThread()\0"
};

void castripplotthread::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        castripplotthread *_t = static_cast<castripplotthread *>(_o);
        switch (_id) {
        case 0: _t->update(); break;
        case 1: _t->TimeOut(); break;
        case 2: _t->runStop(); break;
        case 3: _t->quitFromOtherThread(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData castripplotthread::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject castripplotthread::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_castripplotthread,
      qt_meta_data_castripplotthread, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &castripplotthread::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *castripplotthread::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *castripplotthread::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_castripplotthread))
        return static_cast<void*>(const_cast< castripplotthread*>(this));
    return QThread::qt_metacast(_clname);
}

int castripplotthread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void castripplotthread::update()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
