/****************************************************************************
** Meta object code from reading C++ file 'juegoarte.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../juegoarte.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'juegoarte.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.9.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN9JuegoArteE_t {};
} // unnamed namespace

template <> constexpr inline auto JuegoArte::qt_create_metaobjectdata<qt_meta_tag_ZN9JuegoArteE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "JuegoArte",
        "juegoTerminado",
        "",
        "ganado",
        "onTimeout",
        "actualizarTimer",
        "reiniciarTimer"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'juegoTerminado'
        QtMocHelpers::SignalData<void(bool)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 3 },
        }}),
        // Slot 'onTimeout'
        QtMocHelpers::SlotData<void()>(4, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'actualizarTimer'
        QtMocHelpers::SlotData<void()>(5, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'reiniciarTimer'
        QtMocHelpers::SlotData<void()>(6, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<JuegoArte, qt_meta_tag_ZN9JuegoArteE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject JuegoArte::staticMetaObject = { {
    QMetaObject::SuperData::link<SceneBase::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9JuegoArteE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9JuegoArteE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN9JuegoArteE_t>.metaTypes,
    nullptr
} };

void JuegoArte::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<JuegoArte *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->juegoTerminado((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 1: _t->onTimeout(); break;
        case 2: _t->actualizarTimer(); break;
        case 3: _t->reiniciarTimer(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (JuegoArte::*)(bool )>(_a, &JuegoArte::juegoTerminado, 0))
            return;
    }
}

const QMetaObject *JuegoArte::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *JuegoArte::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9JuegoArteE_t>.strings))
        return static_cast<void*>(this);
    return SceneBase::qt_metacast(_clname);
}

int JuegoArte::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = SceneBase::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void JuegoArte::juegoTerminado(bool _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}
QT_WARNING_POP
