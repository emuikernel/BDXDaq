/****************************************************************************
** Meta object code from reading C++ file 'QCaVariableNamePropertyManager.h'
**
** Created: Wed Oct 16 16:50:19 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../data/include/QCaVariableNamePropertyManager.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QCaVariableNamePropertyManager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QCaVariableNamePropertyManager[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      69,   32,   31,   31, 0x05,

 // slots: signature, parameters, type, tag, flags
     115,   31,   31,   31, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QCaVariableNamePropertyManager[] = {
    "QCaVariableNamePropertyManager\0\0"
    "variable,Substitutions,variableIndex\0"
    "newVariableNameProperty(QString,QString,uint)\0"
    "subscribeDelayExpired()\0"
};

void QCaVariableNamePropertyManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        QCaVariableNamePropertyManager *_t = static_cast<QCaVariableNamePropertyManager *>(_o);
        switch (_id) {
        case 0: _t->newVariableNameProperty((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< uint(*)>(_a[3]))); break;
        case 1: _t->subscribeDelayExpired(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData QCaVariableNamePropertyManager::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QCaVariableNamePropertyManager::staticMetaObject = {
    { &QTimer::staticMetaObject, qt_meta_stringdata_QCaVariableNamePropertyManager,
      qt_meta_data_QCaVariableNamePropertyManager, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QCaVariableNamePropertyManager::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QCaVariableNamePropertyManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QCaVariableNamePropertyManager::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QCaVariableNamePropertyManager))
        return static_cast<void*>(const_cast< QCaVariableNamePropertyManager*>(this));
    return QTimer::qt_metacast(_clname);
}

int QCaVariableNamePropertyManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTimer::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void QCaVariableNamePropertyManager::newVariableNameProperty(QString _t1, QString _t2, unsigned int _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
