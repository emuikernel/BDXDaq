/****************************************************************************
** Meta object code from reading C++ file 'catextentry.h'
**
** Created: Wed Oct 2 14:11:17 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/catextentry.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'catextentry.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_caTextEntry[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       1,   24, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      13,   12,   12,   12, 0x05,

 // slots: signature, parameters, type, tag, flags
      39,   12,   12,   12, 0x08,

 // properties: name, type, flags
      56,   51, 0x01094103,

       0        // eod
};

static const char qt_meta_stringdata_caTextEntry[] = {
    "caTextEntry\0\0TextEntryChanged(QString)\0"
    "dataInput()\0bool\0unitsEnabled\0"
};

void caTextEntry::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        caTextEntry *_t = static_cast<caTextEntry *>(_o);
        switch (_id) {
        case 0: _t->TextEntryChanged((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->dataInput(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData caTextEntry::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject caTextEntry::staticMetaObject = {
    { &caLineEdit::staticMetaObject, qt_meta_stringdata_caTextEntry,
      qt_meta_data_caTextEntry, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &caTextEntry::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *caTextEntry::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *caTextEntry::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_caTextEntry))
        return static_cast<void*>(const_cast< caTextEntry*>(this));
    return caLineEdit::qt_metacast(_clname);
}

int caTextEntry::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = caLineEdit::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = getUnitsEnabled(); break;
        }
        _id -= 1;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setUnitsEnabled(*reinterpret_cast< bool*>(_v)); break;
        }
        _id -= 1;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 1;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void caTextEntry::TextEntryChanged(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
