/****************************************************************************
** Meta object code from reading C++ file 'aulakantscene.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../aulakantscene.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'aulakantscene.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN13AulaKantSceneE_t {};
} // unnamed namespace

template <> constexpr inline auto AulaKantScene::qt_create_metaobjectdata<qt_meta_tag_ZN13AulaKantSceneE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "AulaKantScene",
        "dialogoIniciado",
        "",
        "dialogoTerminado",
        "aulaResuelta",
        "updateHudPos",
        "mostrarBarraDialogo",
        "ocultarBarraDialogo",
        "refrescarLineaActual",
        "reconstruirOpcionesFinal",
        "opciones",
        "construirBancoPreguntas",
        "mostrarPregunta",
        "idx",
        "ocultarPregunta",
        "resolverPregunta",
        "correcta",
        "mostrarPreguntaFinal",
        "actualizarHudPistas",
        "mostrarOverlayPistas",
        "ocultarOverlayPistas"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'dialogoIniciado'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'dialogoTerminado'
        QtMocHelpers::SignalData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'aulaResuelta'
        QtMocHelpers::SignalData<void()>(4, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'updateHudPos'
        QtMocHelpers::SlotData<void()>(5, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'mostrarBarraDialogo'
        QtMocHelpers::SlotData<void()>(6, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'ocultarBarraDialogo'
        QtMocHelpers::SlotData<void()>(7, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'refrescarLineaActual'
        QtMocHelpers::SlotData<void()>(8, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'reconstruirOpcionesFinal'
        QtMocHelpers::SlotData<void(const QStringList &)>(9, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QStringList, 10 },
        }}),
        // Slot 'construirBancoPreguntas'
        QtMocHelpers::SlotData<void()>(11, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'mostrarPregunta'
        QtMocHelpers::SlotData<void(int)>(12, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 13 },
        }}),
        // Slot 'ocultarPregunta'
        QtMocHelpers::SlotData<void()>(14, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'resolverPregunta'
        QtMocHelpers::SlotData<void(bool)>(15, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Bool, 16 },
        }}),
        // Slot 'mostrarPreguntaFinal'
        QtMocHelpers::SlotData<void()>(17, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'actualizarHudPistas'
        QtMocHelpers::SlotData<void()>(18, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'mostrarOverlayPistas'
        QtMocHelpers::SlotData<void()>(19, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'ocultarOverlayPistas'
        QtMocHelpers::SlotData<void()>(20, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<AulaKantScene, qt_meta_tag_ZN13AulaKantSceneE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject AulaKantScene::staticMetaObject = { {
    QMetaObject::SuperData::link<SceneBase::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13AulaKantSceneE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13AulaKantSceneE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN13AulaKantSceneE_t>.metaTypes,
    nullptr
} };

void AulaKantScene::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<AulaKantScene *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->dialogoIniciado(); break;
        case 1: _t->dialogoTerminado(); break;
        case 2: _t->aulaResuelta(); break;
        case 3: _t->updateHudPos(); break;
        case 4: _t->mostrarBarraDialogo(); break;
        case 5: _t->ocultarBarraDialogo(); break;
        case 6: _t->refrescarLineaActual(); break;
        case 7: _t->reconstruirOpcionesFinal((*reinterpret_cast< std::add_pointer_t<QStringList>>(_a[1]))); break;
        case 8: _t->construirBancoPreguntas(); break;
        case 9: _t->mostrarPregunta((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 10: _t->ocultarPregunta(); break;
        case 11: _t->resolverPregunta((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 12: _t->mostrarPreguntaFinal(); break;
        case 13: _t->actualizarHudPistas(); break;
        case 14: _t->mostrarOverlayPistas(); break;
        case 15: _t->ocultarOverlayPistas(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (AulaKantScene::*)()>(_a, &AulaKantScene::dialogoIniciado, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (AulaKantScene::*)()>(_a, &AulaKantScene::dialogoTerminado, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (AulaKantScene::*)()>(_a, &AulaKantScene::aulaResuelta, 2))
            return;
    }
}

const QMetaObject *AulaKantScene::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AulaKantScene::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13AulaKantSceneE_t>.strings))
        return static_cast<void*>(this);
    return SceneBase::qt_metacast(_clname);
}

int AulaKantScene::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = SceneBase::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 16)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 16;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 16)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 16;
    }
    return _id;
}

// SIGNAL 0
void AulaKantScene::dialogoIniciado()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void AulaKantScene::dialogoTerminado()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void AulaKantScene::aulaResuelta()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}
QT_WARNING_POP
