/****************************************************************************
** Meta object code from reading C++ file 'stripplotthread.h'
**
** Created: Wed Oct 2 14:11:56 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/stripplotthread.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'stripplotthread.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_stripplotthread[] = {

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
      17,   16,   16,   16, 0x05,

 // slots: signature, parameters, type, tag, flags
      26,   16,   16,   16, 0x08,
      36,   16,   16,   16, 0x08,
      46,   16,   16,   16, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_stripplotthread[] = {
    "stripplotthread\0\0update()\0TimeOut()\0"
    "runStop()\0quitFromOtherThread()\0"
};

void stripplotthread::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        stripplotthread *_t = static_cast<stripplotthread *>(_o);
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

const QMetaObjectExtraData stripplotthread::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject stripplotthread::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_stripplotthread,
      qt_meta_data_stripplotthread, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &stripplotthread::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *stripplotthread::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *stripplotthread::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_stripplotthread))
        return static_cast<void*>(const_cast< stripplotthread*>(this));
    return QThread::qt_metacast(_clname);
}

int stripplotthread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void stripplotthread::update()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
