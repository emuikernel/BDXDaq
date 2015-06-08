/****************************************************************************
** Meta object code from reading C++ file 'GuiPushButton.h'
**
** Created: Wed Oct 16 16:50:32 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../widgets/include/GuiPushButton.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'GuiPushButton.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GuiPushButton[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      52,   15,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
     103,   14,   14,   14, 0x0a,
     125,  117,   14,   14, 0x0a,
     157,  151,   14,   14, 0x0a,
     178,   15,   14,   14, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_GuiPushButton[] = {
    "GuiPushButton\0\0guiName,substitutions,creationOption\0"
    "newGui(QString,QString,ASguiForm::creationOptions)\0"
    "userClicked()\0message\0onGeneralMessage(QString)\0"
    "state\0requestEnabled(bool)\0"
    "launchGui(QString,QString,ASguiForm::creationOptions)\0"
};

void GuiPushButton::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        GuiPushButton *_t = static_cast<GuiPushButton *>(_o);
        switch (_id) {
        case 0: _t->newGui((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< ASguiForm::creationOptions(*)>(_a[3]))); break;
        case 1: _t->userClicked(); break;
        case 2: _t->onGeneralMessage((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 3: _t->requestEnabled((*reinterpret_cast< const bool(*)>(_a[1]))); break;
        case 4: _t->launchGui((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< ASguiForm::creationOptions(*)>(_a[3]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData GuiPushButton::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject GuiPushButton::staticMetaObject = {
    { &QPushButton::staticMetaObject, qt_meta_stringdata_GuiPushButton,
      qt_meta_data_GuiPushButton, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GuiPushButton::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GuiPushButton::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GuiPushButton::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GuiPushButton))
        return static_cast<void*>(const_cast< GuiPushButton*>(this));
    return QPushButton::qt_metacast(_clname);
}

int GuiPushButton::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QPushButton::qt_metacall(_c, _id, _a);
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
void GuiPushButton::newGui(QString _t1, QString _t2, ASguiForm::creationOptions _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
