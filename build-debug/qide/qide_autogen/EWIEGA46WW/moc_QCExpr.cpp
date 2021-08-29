/****************************************************************************
** Meta object code from reading C++ file 'QCExpr.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../qide/QCExpr.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QCExpr.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_QCExpr_t {
    QByteArrayData data[7];
    char stringdata0[47];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QCExpr_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QCExpr_t qt_meta_stringdata_QCExpr = {
    {
QT_MOC_LITERAL(0, 0, 6), // "QCExpr"
QT_MOC_LITERAL(1, 7, 4), // "Kind"
QT_MOC_LITERAL(2, 12, 6), // "VarDef"
QT_MOC_LITERAL(3, 19, 3), // "Ref"
QT_MOC_LITERAL(4, 23, 9), // "EndOfFile"
QT_MOC_LITERAL(5, 33, 7), // "Unknown"
QT_MOC_LITERAL(6, 41, 5) // "count"

    },
    "QCExpr\0Kind\0VarDef\0Ref\0EndOfFile\0"
    "Unknown\0count"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QCExpr[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       1,   14, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // enums: name, alias, flags, count, data
       1,    1, 0x0,    5,   19,

 // enum data: key, value
       2, uint(QCExpr::VarDef),
       3, uint(QCExpr::Ref),
       4, uint(QCExpr::EndOfFile),
       5, uint(QCExpr::Unknown),
       6, uint(QCExpr::count),

       0        // eod
};

void QCExpr::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

QT_INIT_METAOBJECT const QMetaObject QCExpr::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_QCExpr.data,
    qt_meta_data_QCExpr,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *QCExpr::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QCExpr::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_QCExpr.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int QCExpr::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
