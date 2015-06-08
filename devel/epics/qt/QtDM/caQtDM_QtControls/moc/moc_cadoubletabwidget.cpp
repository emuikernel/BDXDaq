/****************************************************************************
** Meta object code from reading C++ file 'cadoubletabwidget.h'
**
** Created: Wed Oct 2 14:11:50 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/cadoubletabwidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'cadoubletabwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_caDoubleTabWidget[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       4,   54, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      24,   19,   18,   18, 0x05,
      50,   44,   18,   18, 0x05,

 // slots: signature, parameters, type, tag, flags
      75,   44,   18,   18, 0x0a,
      98,   96,   18,   18, 0x0a,
     112,  110,   18,   18, 0x0a,
     129,  124,   18,   18, 0x0a,
     158,  147,   18,   18, 0x0a,
     183,   44,   18,   18, 0x0a,

 // properties: name, type, flags
     207,  199, 0x0a095003,
     223,  199, 0x0a095003,
     237,  199, 0x0a095003,
     262,  258, 0x02095103,

       0        // eod
};

static const char qt_meta_stringdata_caDoubleTabWidget[] = {
    "caDoubleTabWidget\0\0indx\0currentChanged(int)\0"
    "index\0currentIndexChanged(int)\0"
    "setCurrentIndex(int)\0r\0setRow(int)\0c\0"
    "setCol(int)\0page\0addPage(QWidget*)\0"
    "index,page\0insertPage(int,QWidget*)\0"
    "removePage(int)\0QString\0itemsHorizontal\0"
    "itemsVertical\0itemsVerticalPadding\0"
    "int\0currentIndex\0"
};

void caDoubleTabWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        caDoubleTabWidget *_t = static_cast<caDoubleTabWidget *>(_o);
        switch (_id) {
        case 0: _t->currentChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->setCurrentIndex((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->setRow((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->setCol((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->addPage((*reinterpret_cast< QWidget*(*)>(_a[1]))); break;
        case 6: _t->insertPage((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QWidget*(*)>(_a[2]))); break;
        case 7: _t->removePage((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData caDoubleTabWidget::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject caDoubleTabWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_caDoubleTabWidget,
      qt_meta_data_caDoubleTabWidget, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &caDoubleTabWidget::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *caDoubleTabWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *caDoubleTabWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_caDoubleTabWidget))
        return static_cast<void*>(const_cast< caDoubleTabWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int caDoubleTabWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = getItemsHor(); break;
        case 1: *reinterpret_cast< QString*>(_v) = getItemsVer(); break;
        case 2: *reinterpret_cast< QString*>(_v) = getItemsPadding(); break;
        case 3: *reinterpret_cast< int*>(_v) = currentIndex(); break;
        }
        _id -= 4;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setItemsHor(*reinterpret_cast< QString*>(_v)); break;
        case 1: setItemsVer(*reinterpret_cast< QString*>(_v)); break;
        case 2: setItemsPadding(*reinterpret_cast< QString*>(_v)); break;
        case 3: setCurrentIndex(*reinterpret_cast< int*>(_v)); break;
        }
        _id -= 4;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 4;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void caDoubleTabWidget::currentChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void caDoubleTabWidget::currentIndexChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
