/****************************************************************************
** Meta object code from reading C++ file 'QCToken.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../qide/QCToken.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QCToken.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_QCToken_t {
    QByteArrayData data[13];
    char stringdata0[84];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QCToken_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QCToken_t qt_meta_stringdata_QCToken = {
    {
QT_MOC_LITERAL(0, 0, 7), // "QCToken"
QT_MOC_LITERAL(1, 8, 4), // "Kind"
QT_MOC_LITERAL(2, 13, 4), // "Type"
QT_MOC_LITERAL(3, 18, 2), // "Id"
QT_MOC_LITERAL(4, 21, 6), // "Number"
QT_MOC_LITERAL(5, 28, 6), // "String"
QT_MOC_LITERAL(6, 35, 4), // "Term"
QT_MOC_LITERAL(7, 40, 8), // "GlobalId"
QT_MOC_LITERAL(8, 49, 7), // "Comment"
QT_MOC_LITERAL(9, 57, 2), // "Op"
QT_MOC_LITERAL(10, 60, 9), // "EndOfFile"
QT_MOC_LITERAL(11, 70, 7), // "Unknown"
QT_MOC_LITERAL(12, 78, 5) // "count"

    },
    "QCToken\0Kind\0Type\0Id\0Number\0String\0"
    "Term\0GlobalId\0Comment\0Op\0EndOfFile\0"
    "Unknown\0count"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QCToken[] = {

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
       1,    1, 0x0,   11,   19,

 // enum data: key, value
       2, uint(QCToken::Type),
       3, uint(QCToken::Id),
       4, uint(QCToken::Number),
       5, uint(QCToken::String),
       6, uint(QCToken::Term),
       7, uint(QCToken::GlobalId),
       8, uint(QCToken::Comment),
       9, uint(QCToken::Op),
      10, uint(QCToken::EndOfFile),
      11, uint(QCToken::Unknown),
      12, uint(QCToken::count),

       0        // eod
};

void QCToken::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

QT_INIT_METAOBJECT const QMetaObject QCToken::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_QCToken.data,
    qt_meta_data_QCToken,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *QCToken::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QCToken::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_QCToken.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int QCToken::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
