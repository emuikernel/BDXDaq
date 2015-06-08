/****************************************************************************
** Meta object code from reading C++ file 'capolylinedialog.h'
**
** Created: Wed Oct 2 14:11:45 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/capolylinedialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'capolylinedialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_caPolyLineDialog[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      18,   17,   17,   17, 0x08,
      31,   17,   17,   17, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_caPolyLineDialog[] = {
    "caPolyLineDialog\0\0resetState()\0"
    "saveState()\0"
};

void caPolyLineDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        caPolyLineDialog *_t = static_cast<caPolyLineDialog *>(_o);
        switch (_id) {
        case 0: _t->resetState(); break;
        case 1: _t->saveState(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData caPolyLineDialog::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject caPolyLineDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_caPolyLineDialog,
      qt_meta_data_caPolyLineDialog, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &caPolyLineDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *caPolyLineDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *caPolyLineDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_caPolyLineDialog))
        return static_cast<void*>(const_cast< caPolyLineDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int caPolyLineDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
