#include "npcbatalla.h"
#include <QDebug>
#include <QGraphicsScene>
#include <QPropertyAnimation>
#include <functional>

NpcBatalla::NpcBatalla(Player::Bando bando, QObject* parent)
    : QObject(parent)
    , animationTimer(new QTimer(this))
    , attackTimer(new QTimer(this))
    , deathTimer(new QTimer(this))  // NUEVO: Inicializar timer de muerte
    , movementAnimation(new QPropertyAnimation(this, "pos", this))
    , currentFrame(0)
    , isAttacking(false)
    , isTakingDamage(false)
    , isMoving(false)
    , isDyingAnimation(false)  // NUEVO: Inicializar estados
    , isDeadState(false)
    , originalPos(100, 200)
    ,bloodSplattered(false)
    , bandoActual(bando)
{
    loadSprites();
    setPos(originalPos);
    setZValue(10);

    connect(animationTimer, &QTimer::timeout, this, &NpcBatalla::updateAnimation);
    connect(attackTimer, &QTimer::timeout, this, &NpcBatalla::onAttackFinished);
    connect(deathTimer, &QTimer::timeout, this, &NpcBatalla::onDeathFinished);  // NUEVO: Conectar timer de muerte
    connect(movementAnimation, &QPropertyAnimation::finished, this, &NpcBatalla::onMovementFinished);

    animationTimer->start(100);

    if (!idleFrames.isEmpty()) {
        setPixmap(idleFrames[0]);
    } else {
    }
}

NpcBatalla::~NpcBatalla()
{
    stopAllAnimations();
}

QString NpcBatalla::getSpriteFolder() const
{
    switch(bandoActual) {
    case Player::Bando::EMPIRISTA:
        return "Empirista";
    case Player::Bando::RACIONALISTA:
        return "Racionalista";
    case Player::Bando::NULO:
    default:
        return "Empirista";
    }
}

void NpcBatalla::loadSprites()
{
    QString folder = getSpriteFolder();

    int w = npcSize().width();
    int h = npcSize().height();

    // Limpiar frames anteriores
    idleFrames.clear();
    attackFrames.clear();
    damageFrames.clear();
    deathFrames.clear();  // NUEVO: Limpiar frames de muerte
    bloodFrames.clear();

    // Cargar sprite idle
    QString idlePath = QString("://res/%1/idle.png").arg(folder);
    QPixmap idleFrame(idlePath);
    if (!idleFrame.isNull()) {
        idleFrames << idleFrame.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    } else {
        QPixmap placeholder(w, h);
        placeholder.fill(bandoActual == Player::Bando::EMPIRISTA ? Qt::blue : Qt::red);
        idleFrames << placeholder;
    }

    for (int i = 1; i <= 6; i++) {
        QString path = QString("://res/%1/attack%2.png").arg(folder).arg(i);
        QPixmap frame(path);
        if (!frame.isNull()) {
            attackFrames << frame.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        } else {
        }
    }

    if (attackFrames.isEmpty()) {
        attackFrames = idleFrames;
    }

    // Daño (2 frames)
    for (int i = 1; i <= 2; i++) {
        QString path = QString("://res/%1/hurt%2.png").arg(folder).arg(i);
        QPixmap frame(path);
        if (!frame.isNull()) {
            damageFrames << frame.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        } else {
        }
    }

    if (damageFrames.isEmpty()) {
        damageFrames = idleFrames;
    }

    for (int i = 1; i <= 8; i++) {
        QString path = QString("://res/%1/dead%2.png").arg(folder).arg(i);
        QPixmap frame(path);
        if (!frame.isNull()) {
            deathFrames << frame.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        } else {
            if (!damageFrames.isEmpty() && i <= damageFrames.size()) {
                deathFrames << damageFrames[i-1];
            }
        }
    }

    if (deathFrames.isEmpty()) {
        deathFrames = damageFrames;
    }

    // Sangre (3 frames)
    for (int i = 1; i <= 3; i++) {
        QString path = QString("://res/Objects/blood%1.png").arg(i);
        QPixmap frame(path);
        if (!frame.isNull()) {
            bloodFrames << frame.scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        } else {
        }
    }


}

// NUEVO: Animación de muerte
void NpcBatalla::playDeathAnimation()
{
    if (isDyingAnimation || isDeadState) return;


    // NO llamar stopAllAnimations() aquí - eso detiene la animación recién iniciada
    // stopAllAnimations();  // ← ELIMINAR ESTA LÍNEA

    // Solo detener animaciones conflictivas, pero no la de muerte
    isAttacking = false;
    isTakingDamage = false;
    isMoving = false;

    if (attackTimer->isActive()) {
        attackTimer->stop();
    }

    if (movementAnimation->state() == QPropertyAnimation::Running) {
        movementAnimation->stop();
    }

    isDyingAnimation = true;
    isDeadState = false;
    currentFrame = 0;

    if (deathFrames.isEmpty()) {
        isDyingAnimation = false;
        isDeadState = true;
        emit deathCompleted();
        return;
    }

    // Mostrar primer frame inmediatamente
    setPixmap(deathFrames[currentFrame]);

    // Usar QTimer para la animación en lugar de recursión
    deathTimer->start(150); // 150ms entre frames
}

void NpcBatalla::onDeathFinished()
{
    if (!isDyingAnimation) return;

    currentFrame++;

    if (currentFrame < deathFrames.size()) {
        setPixmap(deathFrames[currentFrame]);
    } else {
        // Fin de la animación
        deathTimer->stop();
        isDyingAnimation = false;
        isDeadState = true;
        emit deathCompleted();
    }
}

void NpcBatalla::stopAllAnimations()
{
    // NO detener la animación de muerte si está en progreso
    if (!isDyingAnimation) {
        isAttacking = false;
        isTakingDamage = false;
        isMoving = false;
        currentFrame = 0;

        if (movementAnimation->state() == QPropertyAnimation::Running) {
            movementAnimation->stop();
        }

        if (attackTimer->isActive()) {
            attackTimer->stop();
        }

        if (!idleFrames.isEmpty() && !isDyingAnimation) {
            setPixmap(idleFrames[0]);
        }
    }

    // Death timer se detiene separadamente si es necesario
    // deathTimer->stop(); // No detenerlo aquí
}
void NpcBatalla::updateAnimation()
{
    if (idleFrames.isEmpty()) {
        return;
    }

    // NUEVO: Si está muriendo, no procesar otras animaciones
    if (isDyingAnimation) {
        return;
    }

    QVector<QPixmap>* currentAnimation = &idleFrames;

    if (isTakingDamage) {
        if (!damageFrames.isEmpty()) {
            currentAnimation = &damageFrames;
            setPixmap((*currentAnimation)[currentFrame % currentAnimation->size()]);
            currentFrame++;
            if (currentFrame >= 5) {
                isTakingDamage = false;
                currentFrame = 0;
            }
        }
        return;
    }

    if (isAttacking) {
        if (!attackFrames.isEmpty()) {
            currentAnimation = &attackFrames;
            int frameIndex = currentFrame % currentAnimation->size();
            setPixmap((*currentAnimation)[frameIndex]);

            if (frameIndex == 2 && !bloodFrames.isEmpty() && !bloodSplattered) {
                // dentro de NpcBatalla::updateAnimation(), justo donde emites la sangre
                QPointF bloodPos = pos() + QPointF(boundingRect().width() * 0.55,
                                                   boundingRect().height() * 0.65);
                emit bloodSplatter(bloodPos);

                bloodSplattered = true;
            }

            currentFrame++;
            if (currentFrame >= currentAnimation->size() * 2) {
                currentFrame = currentAnimation->size() - 1;
            }
        }
        return;
    }

    if (isMoving) {
        setPixmap(idleFrames[currentFrame % idleFrames.size()]);
        currentFrame++;
        return;
    }

    // Animación idle por defecto
    setPixmap(idleFrames[currentFrame % idleFrames.size()]);
    currentFrame = (currentFrame + 1) % (idleFrames.size() * 10);
}


void NpcBatalla::startAttack()
{
    if (isAttacking || isDyingAnimation || isDeadState) return;

    isAttacking = true;
    isMoving = false;
    isTakingDamage = false;
    currentFrame = 0;
    bloodSplattered=false;

    QString folder = getSpriteFolder();

    attackTimer->start(1000);
}

void NpcBatalla::takeDamage()
{
    if (isDyingAnimation || isDeadState) return;

    isTakingDamage = true;
    isAttacking = false;
    currentFrame = 0;

    QString folder = getSpriteFolder();

    // Pequeño retroceso visual
    QPropertyAnimation* damageAnim = new QPropertyAnimation(this, "pos", this);
    damageAnim->setDuration(200);
    damageAnim->setStartValue(pos());
    damageAnim->setEndValue(pos() + QPointF(-20, 0));
    damageAnim->start(QAbstractAnimation::DeleteWhenStopped);
}

void NpcBatalla::moveToPosition(int targetX)
{
    if (isMoving || isDyingAnimation || isDeadState) return;

    isMoving = true;
    isAttacking = false;
    isTakingDamage = false;
    currentFrame = 0;

    QString folder = getSpriteFolder();

    movementAnimation->setDuration(500);
    movementAnimation->setStartValue(pos());
    movementAnimation->setEndValue(QPointF(targetX, y()));
    movementAnimation->start();
}

void NpcBatalla::retrieveToOriginalPos()
{
    moveToPosition(originalPos.x());
}


void NpcBatalla::onAttackFinished()
{
    isAttacking = false;
    currentFrame = 0;
    bloodSplattered=false;
    attackTimer->stop();

    if (!idleFrames.isEmpty() && !isDyingAnimation && !isDeadState) {
        setPixmap(idleFrames[0]);
    }

    QString folder = getSpriteFolder();
    emit attackCompleted();
}

void NpcBatalla::onMovementFinished()
{
    isMoving = false;
    currentFrame = 0;

    if (!idleFrames.isEmpty() && !isDyingAnimation && !isDeadState) {
        setPixmap(idleFrames[0]);
    }

}
