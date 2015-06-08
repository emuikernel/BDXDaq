/****************************************************************************
** Meta object code from reading C++ file 'ASguiForm.h'
**
** Created: Wed Oct 16 16:50:34 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../widgets/include/ASguiForm.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ASguiForm.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ASguiForm[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      17,   11,   10,   10, 0x0a,
      38,   10,   10,   10, 0x0a,
      59,   51,   10,   10, 0x0a,
     120,   85,   10,   10, 0x08,
     170,  165,   10,   10, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_ASguiForm[] = {
    "ASguiForm\0\0state\0requestEnabled(bool)\0"
    "readUiFile()\0message\0onGeneralMessage(QString)\0"
    "guiName,substitutions,createOption\0"
    "onGuiLaunch(QString,QString,creationOptions)\0"
    "path\0fileChanged(QString)\0"
};

void ASguiForm::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ASguiForm *_t = static_cast<ASguiForm *>(_o);
        switch (_id) {
        case 0: _t->requestEnabled((*reinterpret_cast< const bool(*)>(_a[1]))); break;
        case 1: _t->readUiFile(); break;
        case 2: _t->onGeneralMessage((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 3: _t->onGuiLaunch((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< creationOptions(*)>(_a[3]))); break;
        case 4: _t->fileChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ASguiForm::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ASguiForm::staticMetaObject = {
    { &QScrollArea::staticMetaObject, qt_meta_stringdata_ASguiForm,
      qt_meta_data_ASguiForm, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ASguiForm::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ASguiForm::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ASguiForm::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ASguiForm))
        return static_cast<void*>(const_cast< ASguiForm*>(this));
    return QScrollArea::qt_metacast(_clname);
}

int ASguiForm::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QScrollArea::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
