/****************************************************************************
** Meta object code from reading C++ file 'scenebatalla.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../scenebatalla.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'scenebatalla.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN12SceneBatallaE_t {};
} // unnamed namespace

template <> constexpr inline auto SceneBatalla::qt_create_metaobjectdata<qt_meta_tag_ZN12SceneBatallaE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "SceneBatalla",
        "juegoTerminado",
        "",
        "ganado",
        "loadScene",
        "setupUI",
        "setupPreguntas",
        "actualizarBatalla",
        "mostrarPregunta",
        "ocultarPregunta",
        "procesarRespuestaJugador",
        "respuesta",
        "procesarRespuestaNPC",
        "atacarJugador",
        "atacarNPC",
        "terminarBatalla",
        "ganoPlayer",
        "onAtaqueNPCCompletado",
        "onAtaquePlayerCompletado",
        "procesarTurno",
        "onSangreLanzada",
        "posicion",
        "crearEfectoSangre",
        "verificarGanador",
        "alternarTurnoFijo",
        "loadBloodFrames"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'juegoTerminado'
        QtMocHelpers::SignalData<void(bool)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 3 },
        }}),
        // Slot 'loadScene'
        QtMocHelpers::SlotData<void()>(4, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'setupUI'
        QtMocHelpers::SlotData<void()>(5, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'setupPreguntas'
        QtMocHelpers::SlotData<void()>(6, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'actualizarBatalla'
        QtMocHelpers::SlotData<void()>(7, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'mostrarPregunta'
        QtMocHelpers::SlotData<void()>(8, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'ocultarPregunta'
        QtMocHelpers::SlotData<void()>(9, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'procesarRespuestaJugador'
        QtMocHelpers::SlotData<void(const QString &)>(10, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 11 },
        }}),
        // Slot 'procesarRespuestaNPC'
        QtMocHelpers::SlotData<void()>(12, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'atacarJugador'
        QtMocHelpers::SlotData<void()>(13, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'atacarNPC'
        QtMocHelpers::SlotData<void()>(14, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'terminarBatalla'
        QtMocHelpers::SlotData<void(bool)>(15, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Bool, 16 },
        }}),
        // Slot 'onAtaqueNPCCompletado'
        QtMocHelpers::SlotData<void()>(17, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onAtaquePlayerCompletado'
        QtMocHelpers::SlotData<void()>(18, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'procesarTurno'
        QtMocHelpers::SlotData<void()>(19, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSangreLanzada'
        QtMocHelpers::SlotData<void(QPointF)>(20, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QPointF, 21 },
        }}),
        // Slot 'crearEfectoSangre'
        QtMocHelpers::SlotData<void(QPointF)>(22, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QPointF, 21 },
        }}),
        // Slot 'verificarGanador'
        QtMocHelpers::SlotData<void()>(23, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'alternarTurnoFijo'
        QtMocHelpers::SlotData<void()>(24, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'loadBloodFrames'
        QtMocHelpers::SlotData<void()>(25, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<SceneBatalla, qt_meta_tag_ZN12SceneBatallaE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject SceneBatalla::staticMetaObject = { {
    QMetaObject::SuperData::link<SceneBase::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12SceneBatallaE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12SceneBatallaE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN12SceneBatallaE_t>.metaTypes,
    nullptr
} };

void SceneBatalla::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<SceneBatalla *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->juegoTerminado((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 1: _t->loadScene(); break;
        case 2: _t->setupUI(); break;
        case 3: _t->setupPreguntas(); break;
        case 4: _t->actualizarBatalla(); break;
        case 5: _t->mostrarPregunta(); break;
        case 6: _t->ocultarPregunta(); break;
        case 7: _t->procesarRespuestaJugador((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 8: _t->procesarRespuestaNPC(); break;
        case 9: _t->atacarJugador(); break;
        case 10: _t->atacarNPC(); break;
        case 11: _t->terminarBatalla((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 12: _t->onAtaqueNPCCompletado(); break;
        case 13: _t->onAtaquePlayerCompletado(); break;
        case 14: _t->procesarTurno(); break;
        case 15: _t->onSangreLanzada((*reinterpret_cast< std::add_pointer_t<QPointF>>(_a[1]))); break;
        case 16: _t->crearEfectoSangre((*reinterpret_cast< std::add_pointer_t<QPointF>>(_a[1]))); break;
        case 17: _t->verificarGanador(); break;
        case 18: _t->alternarTurnoFijo(); break;
        case 19: _t->loadBloodFrames(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (SceneBatalla::*)(bool )>(_a, &SceneBatalla::juegoTerminado, 0))
            return;
    }
}

const QMetaObject *SceneBatalla::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SceneBatalla::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12SceneBatallaE_t>.strings))
        return static_cast<void*>(this);
    return SceneBase::qt_metacast(_clname);
}

int SceneBatalla::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = SceneBase::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 20)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 20;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 20)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 20;
    }
    return _id;
}

// SIGNAL 0
void SceneBatalla::juegoTerminado(bool _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}
QT_WARNING_POP
