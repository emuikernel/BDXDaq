/****************************************************************************
** Meta object code from reading C++ file 'limitsCartesianplotDialog.h'
**
** Created: Wed Oct 2 14:13:10 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/limitsCartesianplotDialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'limitsCartesianplotDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_limitsCartesianplotDialog[] = {

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
      27,   26,   26,   26, 0x0a,
      43,   26,   26,   26, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_limitsCartesianplotDialog[] = {
    "limitsCartesianplotDialog\0\0cancelClicked()\0"
    "applyClicked()\0"
};

void limitsCartesianplotDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        limitsCartesianplotDialog *_t = static_cast<limitsCartesianplotDialog *>(_o);
        switch (_id) {
        case 0: _t->cancelClicked(); break;
        case 1: _t->applyClicked(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData limitsCartesianplotDialog::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject limitsCartesianplotDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_limitsCartesianplotDialog,
      qt_meta_data_limitsCartesianplotDialog, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &limitsCartesianplotDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *limitsCartesianplotDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *limitsCartesianplotDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_limitsCartesianplotDialog))
        return static_cast<void*>(const_cast< limitsCartesianplotDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int limitsCartesianplotDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
