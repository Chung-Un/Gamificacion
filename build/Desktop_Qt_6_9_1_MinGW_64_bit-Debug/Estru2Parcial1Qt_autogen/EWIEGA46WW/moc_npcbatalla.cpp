/****************************************************************************
** Meta object code from reading C++ file 'npcbatalla.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../npcbatalla.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'npcbatalla.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN10NpcBatallaE_t {};
} // unnamed namespace

template <> constexpr inline auto NpcBatalla::qt_create_metaobjectdata<qt_meta_tag_ZN10NpcBatallaE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "NpcBatalla",
        "attackCompleted",
        "",
        "bloodSplatter",
        "position",
        "deathCompleted",
        "startAttack",
        "takeDamage",
        "playDeathAnimation",
        "moveToPosition",
        "targetX",
        "retrieveToOriginalPos",
        "stopAllAnimations",
        "updateAnimation",
        "onAttackFinished",
        "onDeathFinished",
        "onMovementFinished",
        "pos"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'attackCompleted'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'bloodSplatter'
        QtMocHelpers::SignalData<void(QPointF)>(3, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QPointF, 4 },
        }}),
        // Signal 'deathCompleted'
        QtMocHelpers::SignalData<void()>(5, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'startAttack'
        QtMocHelpers::SlotData<void()>(6, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'takeDamage'
        QtMocHelpers::SlotData<void()>(7, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'playDeathAnimation'
        QtMocHelpers::SlotData<void()>(8, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'moveToPosition'
        QtMocHelpers::SlotData<void(int)>(9, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 10 },
        }}),
        // Slot 'retrieveToOriginalPos'
        QtMocHelpers::SlotData<void()>(11, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'stopAllAnimations'
        QtMocHelpers::SlotData<void()>(12, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'updateAnimation'
        QtMocHelpers::SlotData<void()>(13, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onAttackFinished'
        QtMocHelpers::SlotData<void()>(14, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onDeathFinished'
        QtMocHelpers::SlotData<void()>(15, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onMovementFinished'
        QtMocHelpers::SlotData<void()>(16, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'pos'
        QtMocHelpers::PropertyData<QPointF>(17, QMetaType::QPointF, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet),
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<NpcBatalla, qt_meta_tag_ZN10NpcBatallaE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject NpcBatalla::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10NpcBatallaE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10NpcBatallaE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10NpcBatallaE_t>.metaTypes,
    nullptr
} };

void NpcBatalla::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<NpcBatalla *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->attackCompleted(); break;
        case 1: _t->bloodSplatter((*reinterpret_cast< std::add_pointer_t<QPointF>>(_a[1]))); break;
        case 2: _t->deathCompleted(); break;
        case 3: _t->startAttack(); break;
        case 4: _t->takeDamage(); break;
        case 5: _t->playDeathAnimation(); break;
        case 6: _t->moveToPosition((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 7: _t->retrieveToOriginalPos(); break;
        case 8: _t->stopAllAnimations(); break;
        case 9: _t->updateAnimation(); break;
        case 10: _t->onAttackFinished(); break;
        case 11: _t->onDeathFinished(); break;
        case 12: _t->onMovementFinished(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (NpcBatalla::*)()>(_a, &NpcBatalla::attackCompleted, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (NpcBatalla::*)(QPointF )>(_a, &NpcBatalla::bloodSplatter, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (NpcBatalla::*)()>(_a, &NpcBatalla::deathCompleted, 2))
            return;
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<QPointF*>(_v) = _t->pos(); break;
        default: break;
        }
    }
    if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setPos(*reinterpret_cast<QPointF*>(_v)); break;
        default: break;
        }
    }
}

const QMetaObject *NpcBatalla::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *NpcBatalla::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10NpcBatallaE_t>.strings))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "QGraphicsPixmapItem"))
        return static_cast< QGraphicsPixmapItem*>(this);
    return QObject::qt_metacast(_clname);
}

int NpcBatalla::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 13;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void NpcBatalla::attackCompleted()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void NpcBatalla::bloodSplatter(QPointF _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void NpcBatalla::deathCompleted()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}
QT_WARNING_POP
