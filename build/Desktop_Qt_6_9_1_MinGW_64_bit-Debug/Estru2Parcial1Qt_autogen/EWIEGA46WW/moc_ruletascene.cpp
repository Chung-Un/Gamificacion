/****************************************************************************
** Meta object code from reading C++ file 'ruletascene.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../ruletascene.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ruletascene.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN11RuletaSceneE_t {};
} // unnamed namespace

template <> constexpr inline auto RuletaScene::qt_create_metaobjectdata<qt_meta_tag_ZN11RuletaSceneE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "RuletaScene",
        "bandoSeleccionado",
        "",
        "esEmpirista",
        "vidaInicial",
        "irAlMapaTrasBando",
        "confirmarBandoYVolverAlMapa",
        "actualizarAnimacionRuleta",
        "updateHudPos",
        "mostrarSeleccionBando",
        "seleccionarBando"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'bandoSeleccionado'
        QtMocHelpers::SignalData<void(bool, int)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 3 }, { QMetaType::Int, 4 },
        }}),
        // Signal 'irAlMapaTrasBando'
        QtMocHelpers::SignalData<void(bool, int)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 3 }, { QMetaType::Int, 4 },
        }}),
        // Slot 'confirmarBandoYVolverAlMapa'
        QtMocHelpers::SlotData<void()>(6, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'actualizarAnimacionRuleta'
        QtMocHelpers::SlotData<void()>(7, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'updateHudPos'
        QtMocHelpers::SlotData<void()>(8, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'mostrarSeleccionBando'
        QtMocHelpers::SlotData<void()>(9, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'seleccionarBando'
        QtMocHelpers::SlotData<void(bool)>(10, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Bool, 3 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<RuletaScene, qt_meta_tag_ZN11RuletaSceneE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject RuletaScene::staticMetaObject = { {
    QMetaObject::SuperData::link<SceneBase::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11RuletaSceneE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11RuletaSceneE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN11RuletaSceneE_t>.metaTypes,
    nullptr
} };

void RuletaScene::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<RuletaScene *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->bandoSeleccionado((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 1: _t->irAlMapaTrasBando((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 2: _t->confirmarBandoYVolverAlMapa(); break;
        case 3: _t->actualizarAnimacionRuleta(); break;
        case 4: _t->updateHudPos(); break;
        case 5: _t->mostrarSeleccionBando(); break;
        case 6: _t->seleccionarBando((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (RuletaScene::*)(bool , int )>(_a, &RuletaScene::bandoSeleccionado, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (RuletaScene::*)(bool , int )>(_a, &RuletaScene::irAlMapaTrasBando, 1))
            return;
    }
}

const QMetaObject *RuletaScene::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RuletaScene::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11RuletaSceneE_t>.strings))
        return static_cast<void*>(this);
    return SceneBase::qt_metacast(_clname);
}

int RuletaScene::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = SceneBase::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void RuletaScene::bandoSeleccionado(bool _t1, int _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1, _t2);
}

// SIGNAL 1
void RuletaScene::irAlMapaTrasBando(bool _t1, int _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1, _t2);
}
QT_WARNING_POP
