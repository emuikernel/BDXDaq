/****************************************************************************
** Meta object code from reading C++ file 'smibrowse.h'
**
** Created: Tue Oct 29 16:35:36 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../smi_browse_old/smibrowse.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'smibrowse.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_SmiQPushButton[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      16,   15,   15,   15, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_SmiQPushButton[] = {
    "SmiQPushButton\0\0s_to_browse_this_smi_object()\0"
};

void SmiQPushButton::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        SmiQPushButton *_t = static_cast<SmiQPushButton *>(_o);
        switch (_id) {
        case 0: _t->s_to_browse_this_smi_object(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData SmiQPushButton::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject SmiQPushButton::staticMetaObject = {
    { &QPushButton::staticMetaObject, qt_meta_stringdata_SmiQPushButton,
      qt_meta_data_SmiQPushButton, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SmiQPushButton::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SmiQPushButton::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SmiQPushButton::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SmiQPushButton))
        return static_cast<void*>(const_cast< SmiQPushButton*>(this));
    if (!strcmp(_clname, "SmiQWidget"))
        return static_cast< SmiQWidget*>(const_cast< SmiQPushButton*>(this));
    return QPushButton::qt_metacast(_clname);
}

int SmiQPushButton::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QPushButton::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}
static const uint qt_meta_data_SmiQComboBox[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      20,   14,   13,   13, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_SmiQComboBox[] = {
    "SmiQComboBox\0\0index\0"
    "s_to_send_command_for_this_smi_object(int)\0"
};

void SmiQComboBox::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        SmiQComboBox *_t = static_cast<SmiQComboBox *>(_o);
        switch (_id) {
        case 0: _t->s_to_send_command_for_this_smi_object((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData SmiQComboBox::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject SmiQComboBox::staticMetaObject = {
    { &QComboBox::staticMetaObject, qt_meta_stringdata_SmiQComboBox,
      qt_meta_data_SmiQComboBox, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SmiQComboBox::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SmiQComboBox::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SmiQComboBox::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SmiQComboBox))
        return static_cast<void*>(const_cast< SmiQComboBox*>(this));
    if (!strcmp(_clname, "SmiQWidget"))
        return static_cast< SmiQWidget*>(const_cast< SmiQComboBox*>(this));
    return QComboBox::qt_metacast(_clname);
}

int SmiQComboBox::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QComboBox::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}
static const uint qt_meta_data_SmiQLabel[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

static const char qt_meta_stringdata_SmiQLabel[] = {
    "SmiQLabel\0"
};

void SmiQLabel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData SmiQLabel::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject SmiQLabel::staticMetaObject = {
    { &QLabel::staticMetaObject, qt_meta_stringdata_SmiQLabel,
      qt_meta_data_SmiQLabel, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SmiQLabel::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SmiQLabel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SmiQLabel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SmiQLabel))
        return static_cast<void*>(const_cast< SmiQLabel*>(this));
    if (!strcmp(_clname, "SmiQWidget"))
        return static_cast< SmiQWidget*>(const_cast< SmiQLabel*>(this));
    return QLabel::qt_metacast(_clname);
}

int SmiQLabel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QLabel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_HbSmiBrowse[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      13,   12,   12,   12, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_HbSmiBrowse[] = {
    "HbSmiBrowse\0\0cb_update()\0"
};

void HbSmiBrowse::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        HbSmiBrowse *_t = static_cast<HbSmiBrowse *>(_o);
        switch (_id) {
        case 0: _t->cb_update(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData HbSmiBrowse::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject HbSmiBrowse::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_HbSmiBrowse,
      qt_meta_data_HbSmiBrowse, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &HbSmiBrowse::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *HbSmiBrowse::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *HbSmiBrowse::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_HbSmiBrowse))
        return static_cast<void*>(const_cast< HbSmiBrowse*>(this));
    return QObject::qt_metacast(_clname);
}

int HbSmiBrowse::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
