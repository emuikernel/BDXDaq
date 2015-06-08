/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created: Fri May 30 14:40:36 2014
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../hv_control_3crates/mainwindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MainWindow[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x08,
      27,   11,   11,   11, 0x08,
      36,   11,   11,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MainWindow[] = {
    "MainWindow\0\0s_destructor()\0s_back()\0"
    "s_crateListDelete()\0"
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        MainWindow *_t = static_cast<MainWindow *>(_o);
        switch (_id) {
        case 0: _t->s_destructor(); break;
        case 1: _t->s_back(); break;
        case 2: _t->s_crateListDelete(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData MainWindow::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow,
      qt_meta_data_MainWindow, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MainWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow))
        return static_cast<void*>(const_cast< MainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}
static const uint qt_meta_data_MyQCaLabel[] = {

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
      16,   12,   11,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MyQCaLabel[] = {
    "MyQCaLabel\0\0str\0s_dbValueChanged(QString)\0"
};

void MyQCaLabel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        MyQCaLabel *_t = static_cast<MyQCaLabel *>(_o);
        switch (_id) {
        case 0: _t->s_dbValueChanged((*reinterpret_cast< QString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData MyQCaLabel::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject MyQCaLabel::staticMetaObject = {
    { &QCaLabel::staticMetaObject, qt_meta_stringdata_MyQCaLabel,
      qt_meta_data_MyQCaLabel, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MyQCaLabel::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MyQCaLabel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MyQCaLabel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MyQCaLabel))
        return static_cast<void*>(const_cast< MyQCaLabel*>(this));
    return QCaLabel::qt_metacast(_clname);
}

int MyQCaLabel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QCaLabel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}
static const uint qt_meta_data_CrateButton[] = {

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

static const char qt_meta_stringdata_CrateButton[] = {
    "CrateButton\0\0s_crateDisplay()\0"
};

void CrateButton::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        CrateButton *_t = static_cast<CrateButton *>(_o);
        switch (_id) {
        case 0: _t->s_crateDisplay(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData CrateButton::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject CrateButton::staticMetaObject = {
    { &QPushButton::staticMetaObject, qt_meta_stringdata_CrateButton,
      qt_meta_data_CrateButton, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &CrateButton::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *CrateButton::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *CrateButton::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CrateButton))
        return static_cast<void*>(const_cast< CrateButton*>(this));
    return QPushButton::qt_metacast(_clname);
}

int CrateButton::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
static const uint qt_meta_data_MyQCaComboBox[] = {

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
      19,   15,   14,   14, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MyQCaComboBox[] = {
    "MyQCaComboBox\0\0out\0s_dbValueChanged(qlonglong)\0"
};

void MyQCaComboBox::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        MyQCaComboBox *_t = static_cast<MyQCaComboBox *>(_o);
        switch (_id) {
        case 0: _t->s_dbValueChanged((*reinterpret_cast< qlonglong(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData MyQCaComboBox::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject MyQCaComboBox::staticMetaObject = {
    { &QCaComboBox::staticMetaObject, qt_meta_stringdata_MyQCaComboBox,
      qt_meta_data_MyQCaComboBox, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MyQCaComboBox::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MyQCaComboBox::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MyQCaComboBox::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MyQCaComboBox))
        return static_cast<void*>(const_cast< MyQCaComboBox*>(this));
    return QCaComboBox::qt_metacast(_clname);
}

int MyQCaComboBox::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QCaComboBox::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}
static const uint qt_meta_data_HvCrate[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
       9,    8,    8,    8, 0x08,
      24,    8,    8,    8, 0x08,
      46,    8,    8,    8, 0x08,
      69,    8,    8,    8, 0x08,
      79,    8,    8,    8, 0x08,
      88,    8,    8,    8, 0x08,
     103,    8,    8,    8, 0x08,
     115,    8,    8,    8, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_HvCrate[] = {
    "HvCrate\0\0s_destructor()\0s_turnonallchannels()\0"
    "s_turnoffallchannels()\0s_store()\0"
    "s_load()\0cb_update_db()\0s_crateon()\0"
    "s_crateoff()\0"
};

void HvCrate::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        HvCrate *_t = static_cast<HvCrate *>(_o);
        switch (_id) {
        case 0: _t->s_destructor(); break;
        case 1: _t->s_turnonallchannels(); break;
        case 2: _t->s_turnoffallchannels(); break;
        case 3: _t->s_store(); break;
        case 4: _t->s_load(); break;
        case 5: _t->cb_update_db(); break;
        case 6: _t->s_crateon(); break;
        case 7: _t->s_crateoff(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData HvCrate::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject HvCrate::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_HvCrate,
      qt_meta_data_HvCrate, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &HvCrate::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *HvCrate::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *HvCrate::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_HvCrate))
        return static_cast<void*>(const_cast< HvCrate*>(this));
    return QObject::qt_metacast(_clname);
}

int HvCrate::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    return _id;
}
static const uint qt_meta_data_BoardButton[] = {

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

static const char qt_meta_stringdata_BoardButton[] = {
    "BoardButton\0\0s_boardDisplay()\0"
};

void BoardButton::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        BoardButton *_t = static_cast<BoardButton *>(_o);
        switch (_id) {
        case 0: _t->s_boardDisplay(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData BoardButton::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject BoardButton::staticMetaObject = {
    { &QPushButton::staticMetaObject, qt_meta_stringdata_BoardButton,
      qt_meta_data_BoardButton, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &BoardButton::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *BoardButton::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *BoardButton::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_BoardButton))
        return static_cast<void*>(const_cast< BoardButton*>(this));
    return QPushButton::qt_metacast(_clname);
}

int BoardButton::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
QT_END_MOC_NAMESPACE
