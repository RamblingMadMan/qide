/****************************************************************************
** Meta object code from reading C++ file 'QCType.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../qide/QCType.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QCType.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_QCType_t {
    QByteArrayData data[18];
    char stringdata0[121];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QCType_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QCType_t qt_meta_stringdata_QCType = {
    {
QT_MOC_LITERAL(0, 0, 6), // "QCType"
QT_MOC_LITERAL(1, 7, 12), // "valueChanged"
QT_MOC_LITERAL(2, 20, 0), // ""
QT_MOC_LITERAL(3, 21, 12), // "innerChanged"
QT_MOC_LITERAL(4, 34, 10), // "strChanged"
QT_MOC_LITERAL(5, 45, 5), // "value"
QT_MOC_LITERAL(6, 51, 5), // "Value"
QT_MOC_LITERAL(7, 57, 5), // "inner"
QT_MOC_LITERAL(8, 63, 3), // "str"
QT_MOC_LITERAL(9, 67, 4), // "Void"
QT_MOC_LITERAL(10, 72, 3), // "Int"
QT_MOC_LITERAL(11, 76, 5), // "Float"
QT_MOC_LITERAL(12, 82, 6), // "Vector"
QT_MOC_LITERAL(13, 89, 6), // "String"
QT_MOC_LITERAL(14, 96, 6), // "Entity"
QT_MOC_LITERAL(15, 103, 5), // "Field"
QT_MOC_LITERAL(16, 109, 5), // "Array"
QT_MOC_LITERAL(17, 115, 5) // "count"

    },
    "QCType\0valueChanged\0\0innerChanged\0"
    "strChanged\0value\0Value\0inner\0str\0Void\0"
    "Int\0Float\0Vector\0String\0Entity\0Field\0"
    "Array\0count"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QCType[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       3,   32, // properties
       1,   44, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   29,    2, 0x06 /* Public */,
       3,    0,   30,    2, 0x06 /* Public */,
       4,    0,   31,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // properties: name, type, flags
       5, 0x80000000 | 6, 0x00495009,
       7, QMetaType::QVariant, 0x00495001,
       8, QMetaType::QString, 0x00495001,

 // properties: notify_signal_id
       0,
       1,
       2,

 // enums: name, alias, flags, count, data
       6,    6, 0x0,    9,   49,

 // enum data: key, value
       9, uint(QCType::Void),
      10, uint(QCType::Int),
      11, uint(QCType::Float),
      12, uint(QCType::Vector),
      13, uint(QCType::String),
      14, uint(QCType::Entity),
      15, uint(QCType::Field),
      16, uint(QCType::Array),
      17, uint(QCType::count),

       0        // eod
};

void QCType::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<QCType *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->valueChanged(); break;
        case 1: _t->innerChanged(); break;
        case 2: _t->strChanged(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (QCType::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QCType::valueChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (QCType::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QCType::innerChanged)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (QCType::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QCType::strChanged)) {
                *result = 2;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<QCType *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< Value*>(_v) = _t->value(); break;
        case 1: *reinterpret_cast< QVariant*>(_v) = _t->inner(); break;
        case 2: *reinterpret_cast< QString*>(_v) = _t->str(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
    (void)_a;
}

QT_INIT_METAOBJECT const QMetaObject QCType::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_QCType.data,
    qt_meta_data_QCType,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *QCType::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QCType::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_QCType.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int QCType::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 3;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void QCType::valueChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void QCType::innerChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void QCType::strChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
