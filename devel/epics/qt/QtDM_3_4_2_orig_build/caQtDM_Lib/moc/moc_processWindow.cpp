/****************************************************************************
** Meta object code from reading C++ file 'processWindow.h'
**
** Created: Wed Oct 2 14:13:13 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/processWindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'processWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_processWindow[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
      35,   14,   30,   14, 0x0a,
      50,   14,   14,   14, 0x09,
      68,   14,   14,   14, 0x09,
      89,   85,   14,   14, 0x09,
     126,   14,   14,   14, 0x09,
     147,   14,   14,   14, 0x09,
     161,   14,   14,   14, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_processWindow[] = {
    "processWindow\0\0processClose()\0bool\0"
    "tryTerminate()\0processFinished()\0"
    "processStarted()\0err\0"
    "processError(QProcess::ProcessError)\0"
    "closeButtonClicked()\0updateError()\0"
    "updateText()\0"
};

void processWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        processWindow *_t = static_cast<processWindow *>(_o);
        switch (_id) {
        case 0: _t->processClose(); break;
        case 1: { bool _r = _t->tryTerminate();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 2: _t->processFinished(); break;
        case 3: _t->processStarted(); break;
        case 4: _t->processError((*reinterpret_cast< QProcess::ProcessError(*)>(_a[1]))); break;
        case 5: _t->closeButtonClicked(); break;
        case 6: _t->updateError(); break;
        case 7: _t->updateText(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData processWindow::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject processWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_processWindow,
      qt_meta_data_processWindow, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &processWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *processWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *processWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_processWindow))
        return static_cast<void*>(const_cast< processWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int processWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void processWindow::processClose()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
