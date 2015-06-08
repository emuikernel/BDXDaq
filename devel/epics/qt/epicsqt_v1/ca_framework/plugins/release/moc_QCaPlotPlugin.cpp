/****************************************************************************
** Meta object code from reading C++ file 'QCaPlotPlugin.h'
**
** Created: Wed Oct 16 16:50:17 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../include/QCaPlotPlugin.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QCaPlotPlugin.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QCaPlotPlugin[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
      17,   19, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      72,   15,   14,   14, 0x08,

 // properties: name, type, flags
     129,  121, 0x0a095003,
     138,  121, 0x0a095003,
     165,  160, 0x01095103,
     175,  160, 0x01095103,
     200,  193, 0x06095103,
     205,  193, 0x06095103,
     210,  160, 0x01095103,
     220,  121, 0x0a095103,
     233,  226, 0x43095103,
     249,  226, 0x43095103,
     260,  121, 0x0a095103,
     272,  121, 0x0a095103,
     278,  121, 0x0a095103,
     284,  193, 0x06095103,
     291,  193, 0x06095103,
     307,  302, 0x03095103,
     316,  302, 0x03095103,

       0        // eod
};

static const char qt_meta_stringdata_QCaPlotPlugin[] = {
    "QCaPlotPlugin\0\0"
    "variableNameIn,variableNameSubstitutionsIn,variableIndex\0"
    "useNewVariableNameProperty(QString,QString,uint)\0"
    "QString\0variable\0variableSubstitutions\0"
    "bool\0subscribe\0variableAsToolTip\0"
    "double\0yMin\0yMax\0autoScale\0title\0"
    "QColor\0backgroundColor\0traceColor\0"
    "traceLegend\0xUnit\0yUnit\0xStart\0"
    "xIncrement\0uint\0timeSpan\0tickRate\0"
};

void QCaPlotPlugin::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        QCaPlotPlugin *_t = static_cast<QCaPlotPlugin *>(_o);
        switch (_id) {
        case 0: _t->useNewVariableNameProperty((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< uint(*)>(_a[3]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData QCaPlotPlugin::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QCaPlotPlugin::staticMetaObject = {
    { &QCaPlot::staticMetaObject, qt_meta_stringdata_QCaPlotPlugin,
      qt_meta_data_QCaPlotPlugin, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QCaPlotPlugin::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QCaPlotPlugin::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QCaPlotPlugin::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QCaPlotPlugin))
        return static_cast<void*>(const_cast< QCaPlotPlugin*>(this));
    return QCaPlot::qt_metacast(_clname);
}

int QCaPlotPlugin::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QCaPlot::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = getVariableNameProperty(); break;
        case 1: *reinterpret_cast< QString*>(_v) = getVariableNameSubstitutionsProperty(); break;
        case 2: *reinterpret_cast< bool*>(_v) = getSubscribe(); break;
        case 3: *reinterpret_cast< bool*>(_v) = getVariableAsToolTip(); break;
        case 4: *reinterpret_cast< double*>(_v) = getYMin(); break;
        case 5: *reinterpret_cast< double*>(_v) = getYMax(); break;
        case 6: *reinterpret_cast< bool*>(_v) = getAutoScale(); break;
        case 7: *reinterpret_cast< QString*>(_v) = getTitle(); break;
        case 8: *reinterpret_cast< QColor*>(_v) = getBackgroundColor(); break;
        case 9: *reinterpret_cast< QColor*>(_v) = getTraceColor(); break;
        case 10: *reinterpret_cast< QString*>(_v) = getTraceLegend(); break;
        case 11: *reinterpret_cast< QString*>(_v) = getXUnit(); break;
        case 12: *reinterpret_cast< QString*>(_v) = getYUnit(); break;
        case 13: *reinterpret_cast< double*>(_v) = getXStart(); break;
        case 14: *reinterpret_cast< double*>(_v) = getXIncrement(); break;
        case 15: *reinterpret_cast< uint*>(_v) = getTimeSpan(); break;
        case 16: *reinterpret_cast< uint*>(_v) = getTickRate(); break;
        }
        _id -= 17;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setVariableNameProperty(*reinterpret_cast< QString*>(_v)); break;
        case 1: setVariableNameSubstitutionsProperty(*reinterpret_cast< QString*>(_v)); break;
        case 2: setSubscribe(*reinterpret_cast< bool*>(_v)); break;
        case 3: setVariableAsToolTip(*reinterpret_cast< bool*>(_v)); break;
        case 4: setYMin(*reinterpret_cast< double*>(_v)); break;
        case 5: setYMax(*reinterpret_cast< double*>(_v)); break;
        case 6: setAutoScale(*reinterpret_cast< bool*>(_v)); break;
        case 7: setTitle(*reinterpret_cast< QString*>(_v)); break;
        case 8: setBackgroundColor(*reinterpret_cast< QColor*>(_v)); break;
        case 9: setTraceColor(*reinterpret_cast< QColor*>(_v)); break;
        case 10: setTraceLegend(*reinterpret_cast< QString*>(_v)); break;
        case 11: setXUnit(*reinterpret_cast< QString*>(_v)); break;
        case 12: setYUnit(*reinterpret_cast< QString*>(_v)); break;
        case 13: setXStart(*reinterpret_cast< double*>(_v)); break;
        case 14: setXIncrement(*reinterpret_cast< double*>(_v)); break;
        case 15: setTimeSpan(*reinterpret_cast< uint*>(_v)); break;
        case 16: setTickRate(*reinterpret_cast< uint*>(_v)); break;
        }
        _id -= 17;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 17;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 17;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 17;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 17;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 17;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 17;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
