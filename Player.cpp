#include "player.h"
#include <QDebug>
#include <QTimer>

Player::Player(QObject *parent)
    : QObject(parent)
    , bandoActual(Bando::NULO)
    , nivelEpis(NivelEpistemologico::APRENDIZ)
    , puntosRacionalistas(0)
    , puntosEmpiristas(0)
    , puntosCriticos(0)
    , totalXP(0)
    , vidasEpistemologicas(3)
    , currentFrame(0)
    , animationTimer(new QTimer(this))
    , isMovingAnimation(false)
    , currentDirection(Direction::IDLE)
    , currentAnimType(AnimationType::WALK)
    , originalSize(110, 150)
    , currentSize(110, 150)
{
    loadSprites();
    loadDisintegrateSprites();
    loadHurtSprites();  // ADD THIS LINE
    loadAttackSprites();
    loadRunSprites(); // Nueva animación correr
    setZValue(20);

    connect(animationTimer, &QTimer::timeout, this, &Player::onAnimationTimer);
    animationTimer->start(150);

    if (!idleFrames.isEmpty()) {
        setPixmap(idleFrames[0]);
    }
}
void Player::loadSprites()
{
    idleFrames.clear();
    walkUpFrames.clear();
    walkDownFrames.clear();
    walkLeftFrames.clear();
    walkRightFrames.clear();

    int w = currentSize.width();
    int h = currentSize.height();

    // Determinar el prefijo según el bando
    QString prefix;
    switch(bandoActual) {
    case Bando::EMPIRISTA:
        prefix = "empirista_";
        break;
    case Bando::RACIONALISTA:
        prefix = "racionalista_";
        break;
    case Bando::NULO:
    default:
        prefix = ""; // Sprites neutrales por defecto
        break;
    }


    // Idle - SOLO 1 SPRITE
    QString idlePath = QString("://res/Player/%1idleGirl.png").arg(prefix);
    QPixmap idleFrame(idlePath);
    if (!idleFrame.isNull()) {
        idleFrames << idleFrame.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    } else {
        // Fallback a sprite neutral
        QString fallbackPath = "://res/Player/idleGirl.png";
        QPixmap fallbackFrame(fallbackPath);
        if (!fallbackFrame.isNull()) {
            idleFrames << fallbackFrame.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        } else {
        }
    }

    // Cargar sprites de caminar con mejor manejo de errores
    loadWalkSprites(prefix, w, h);
}

void Player::loadWalkSprites(const QString& prefix, int w, int h)
{
    // Izquierda - 6 SPRITES
    for (int i = 1; i <= 6; i++) {
        QString path = QString("://res/Player/%1leftGirl%2.png").arg(prefix).arg(i);
        QPixmap frame(path);
        if (!frame.isNull()) {
            walkLeftFrames << frame.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        } else {
            // Fallback sin prefijo
            QString fallbackPath = QString("://res/Player/leftGirl%1.png").arg(i);
            QPixmap fallbackFrame(fallbackPath);
            if (!fallbackFrame.isNull()) {
                walkLeftFrames << fallbackFrame.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            }
        }
    }

    // Derecha - 6 SPRITES
    for (int i = 1; i <= 6; i++) {
        QString path = QString("://res/Player/%1rightGirl%2.png").arg(prefix).arg(i);
        QPixmap frame(path);
        if (!frame.isNull()) {
            walkRightFrames << frame.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        } else {
            QString fallbackPath = QString("://res/Player/rightGirl%1.png").arg(i);
            QPixmap fallbackFrame(fallbackPath);
            if (!fallbackFrame.isNull()) {
                walkRightFrames << fallbackFrame.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            }
        }
    }

    // Arriba - usar derecha como placeholder si no hay sprites específicos
    if (walkUpFrames.isEmpty()) {
        walkUpFrames = walkRightFrames;
    }

    // Abajo - usar izquierda como placeholder si no hay sprites específicos
    if (walkDownFrames.isEmpty()) {
        walkDownFrames = walkLeftFrames;
    }
}
void Player::loadAttackSprites()
{
    attackFrames.clear();

    // Guardar tamaño original temporalmente
    QSize originalTemp = currentSize;

    // Tamaño para ataque
    int w = 204;
    int h = 132;

    QString prefix;
    switch(bandoActual) {
    case Bando::EMPIRISTA:
        prefix = "empirista_";
        break;
    case Bando::RACIONALISTA:
        prefix = "racionalista_";
        break;
    case Bando::NULO:
    default:
        prefix = "";
        break;
    }

    for(int i = 1; i <= 10; i++){
        // CORREGIDO: Formato correcto del string
        QString path = QString("://res/Player/%1attackGirl%2.png").arg(prefix).arg(i);
        QPixmap frame(path);
        if (!frame.isNull()) {
            attackFrames << frame.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        } else {
            // Fallback
            QString fallbackPath = QString("://res/Player/attackGirl%1.png").arg(i);
            QPixmap fallbackFrame(fallbackPath);
            if (!fallbackFrame.isNull()) {
                attackFrames << fallbackFrame.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            }
        }
    }

    // Restaurar tamaño original
    currentSize = originalTemp;
}

void Player::loadRunSprites()
{
    runFrames.clear();
    int w = currentSize.width();
    int h = currentSize.height();

    for (int i = 1; i <= 6; i++) {
        QString path = QString("://res/Player/run%1.png").arg(i);
        QPixmap frame(path);
        if (!frame.isNull()) {
            runFrames << frame.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }
    }

    if (runFrames.isEmpty()) {
        runFrames = walkRightFrames; // Usar walk como fallback
    }
}

void Player::loadDisintegrateSprites()
{
    disintegrateFrames.clear();
    int w = currentSize.width();
    int h = currentSize.height();

    QString prefix;
    switch(bandoActual) {
    case Bando::EMPIRISTA:
        prefix = "empirista_";
        break;
    case Bando::RACIONALISTA:
        prefix = "racionalista_";
        break;
    case Bando::NULO:
    default:
        prefix = "";
        break;
    }

    for(int i = 1; i <= 10; i++){
        QString path = QString("://res/Player/%1dead%2.png").arg(prefix).arg(i);
        QPixmap frame(path);
        if (!frame.isNull()) {
            disintegrateFrames << frame.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        } else {
            QString fallbackPath = QString("://res/Player/dead%1.png").arg(i);
            QPixmap fallbackFrame(fallbackPath);
            if (!fallbackFrame.isNull()) {
                disintegrateFrames << fallbackFrame.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            }
        }
    }
}
void Player::setPlayerSize(int width, int height)
{
    currentSize = QSize(width, height);
    loadSprites();
    loadAttackSprites();
    loadDisintegrateSprites();
    loadHurtSprites();
    loadRunSprites();

    if(!idleFrames.isEmpty()){
        setPixmap(idleFrames[0]);
    }
}
void Player::resetPlayerSize()
{
    currentSize = originalSize;
    loadSprites();
    loadAttackSprites();
    loadDisintegrateSprites();
    loadHurtSprites();
    loadRunSprites();

    if(!idleFrames.isEmpty()){
        setPixmap(idleFrames[0]);
    }
}

void Player::onAnimationTimer()
{
    // Si no hay sprites, no hacer nada
    if (idleFrames.isEmpty() && attackFrames.isEmpty() && hurtFrames.isEmpty() &&
        disintegrateFrames.isEmpty() && runFrames.isEmpty()) {
        return;
    }

    // Manejar animaciones especiales primero
    if (currentAnimType == AnimationType::HURT && !hurtFrames.isEmpty()) {
        if (currentFrame < hurtFrames.size()) {
            setPixmap(hurtFrames[currentFrame]);
            currentFrame++;
            if (currentFrame >= hurtFrames.size()) {
                currentFrame = 0;
                currentAnimType = AnimationType::WALK; // Volver a walk después de hurt
                emit animationFinished();
            }
        }
        return;
    }

    if (currentAnimType == AnimationType::DISINTEGRATE && !disintegrateFrames.isEmpty()) {
        if (currentFrame < disintegrateFrames.size()) {
            setPixmap(disintegrateFrames[currentFrame]);
            currentFrame++;
            if (currentFrame >= disintegrateFrames.size()) {
                currentFrame = disintegrateFrames.size() - 1;
                emit animationFinished();
            }
        }
        return;
    }

    if (currentAnimType == AnimationType::ATTACK && !attackFrames.isEmpty()) {
        if (currentFrame < attackFrames.size()) {
            setPixmap(attackFrames[currentFrame]);
            currentFrame++;
            if (currentFrame >= attackFrames.size()) {
                currentFrame = 0;
                currentAnimType = AnimationType::WALK; // Volver a walk después de ataque
                emit animationFinished();
            }
        }
        return;
    }

    if (currentAnimType == AnimationType::RUN && !runFrames.isEmpty()) {
        setPixmap(runFrames[currentFrame]);
        currentFrame = (currentFrame + 1) % runFrames.size();
        return;
    }

    // Animación normal (WALK/IDLE)
    if (isMovingAnimation) {
        QVector<QPixmap>* currentAnimation = &walkRightFrames; // Default

        switch(currentDirection) {
        case Direction::UP:
            currentAnimation = &walkUpFrames;
            break;
        case Direction::DOWN:
            currentAnimation = &walkDownFrames;
            break;
        case Direction::LEFT:
            currentAnimation = &walkLeftFrames;
            break;
        case Direction::RIGHT:
            currentAnimation = &walkRightFrames;
            break;
        default:
            break;
        }

        if (!currentAnimation->isEmpty()) {
            setPixmap((*currentAnimation)[currentFrame]);
            currentFrame = (currentFrame + 1) % currentAnimation->size();
        }
    } else {
        // IDLE
        if (!idleFrames.isEmpty()) {
            setPixmap(idleFrames[0]); // Solo el primer frame para idle
        }
        currentFrame = 0;
    }
}

void Player::setAnimation(bool moving, Direction direction)
{
    if (currentAnimType != AnimationType::DISINTEGRATE && currentAnimType != AnimationType::RUN) {
        isMovingAnimation = moving;
        currentDirection = direction;
    }
}

// Animación correr
void Player::startRunning()
{
    // Si ya está en modo RUN y el timer está activo, no hacer nada
    if (currentAnimType == AnimationType::RUN && animationTimer->isActive()) {
        return; // Ya está corriendo, no reiniciar
    }


    currentAnimType = AnimationType::RUN;
    currentFrame = 0;

    // Solo reiniciar el timer si no está activo o si está en un modo diferente
    animationTimer->stop();
    animationTimer->start(100); // 100ms entre frames para animación rápida

}

void Player::stopRunning()
{
    currentAnimType = AnimationType::WALK;
    currentFrame = 0;
    isMovingAnimation = false;
    currentDirection = Direction::IDLE;
    if (!idleFrames.isEmpty()) setPixmap(idleFrames[0]);
    animationTimer->stop();
    animationTimer->start(150);
}

// Animación de desintegración
void Player::playDisintegrateAnimation()
{
    currentAnimType = AnimationType::DISINTEGRATE;
    currentFrame = 0;
    isMovingAnimation = false;
    animationTimer->stop();
    animationTimer->start(200);
}

void Player::playAttackAnimation(){
    currentAnimType = AnimationType::ATTACK;
    currentFrame= 0;
    isMovingAnimation = false;
    animationTimer->stop();
    animationTimer->start(200);
}
void Player::resetAnimation()
{
    currentAnimType = AnimationType::WALK;
    currentFrame = 0;
    isMovingAnimation = false;
    currentDirection = Direction::IDLE;
    if (!idleFrames.isEmpty()) setPixmap(idleFrames[0]);
    animationTimer->stop();
    animationTimer->start(150);
}
void Player::cambiarBando(Bando nuevoBando)
{
    if (bandoActual == nuevoBando) return;

    bandoActual = nuevoBando;

    // Recargar sprites con el nuevo bando
    cargarSpritesBando();

    // Aplicar beneficios del bando
    unirseABando(bandoActual);
}
void Player::cargarSpritesBando()
{
    QString spriteBase;

    switch(bandoActual) {
    case Bando::EMPIRISTA:
        spriteBase = spriteBaseEmpirista;
        break;
    case Bando::RACIONALISTA:
        spriteBase = spriteBaseRacionalista;
        break;
    case Bando::NULO:
    default:
        spriteBase = "://res/Player/"; // Sprites neutrales
        break;
    }

    // Recargar todos los sprites usando la nueva base
    loadSprites();
    loadAttackSprites();
    loadDisintegrateSprites();
    loadHurtSprites();
    loadRunSprites();

    // Restablecer sprite actual
    if (!idleFrames.isEmpty()) {
        setPixmap(idleFrames[0]);
    }
}

// Sistema filosófico
void Player::unirseABando(Bando bando)
{
    bandoActual = bando;
    switch(bando) {
    case Bando::RACIONALISTA:
        puntosRacionalistas += 50;
        conceptosDesbloqueados << "Ideas Innatas" << "Deducción Pura";
        insigniasObtenidas << "Iniciado Racionalista";
        break;
    case Bando::EMPIRISTA:
        puntosEmpiristas += 50;
        conceptosDesbloqueados << "Tabula Rasa" << "Experiencia Sensible";
        insigniasObtenidas << "Iniciado Empirista";
        break;
    default: break;
    }
}

void Player::ganarPuntos(int puntos, Bando bando)
{
    switch(bando) {
    case Bando::RACIONALISTA: puntosRacionalistas += puntos; break;
    case Bando::EMPIRISTA: puntosEmpiristas += puntos; break;
    default: puntosCriticos += puntos; break;
    }
    totalXP += puntos;
    updateNivelEpis();
}

void Player::desbloquearConcepto(const QString& concepto)
{
    if (!conceptosDesbloqueados.contains(concepto))
        conceptosDesbloqueados << concepto;
}

void Player::updateNivelEpis()
{
    NivelEpistemologico nuevoNivel;
    if (totalXP >= 300) nuevoNivel = NivelEpistemologico::MASTER;
    else if (totalXP >= 200) nuevoNivel = NivelEpistemologico::ALUMNO;
    else if (totalXP >= 100) nuevoNivel = NivelEpistemologico::DISCIPULO;
    else nuevoNivel = NivelEpistemologico::APRENDIZ;

    if (nuevoNivel != nivelEpis) nivelEpis = nuevoNivel;
}

void Player::loadHurtSprites()
{
    hurtFrames.clear();
    int w = currentSize.width();
    int h = currentSize.height();

    QString prefix;
    switch(bandoActual) {
    case Bando::EMPIRISTA:
        prefix = "empirista_";
        break;
    case Bando::RACIONALISTA:
        prefix = "racionalista_";
        break;
    case Bando::NULO:
    default:
        prefix = "";
        break;
    }

    for(int i = 1; i <= 3; i++){
        // CORREGIDO: Formato correcto del string
        QString path = QString("://res/Player/%1hurtGirl%2.png").arg(prefix).arg(i);
        QPixmap frame(path);
        if (!frame.isNull()) {
            hurtFrames << frame.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        } else {
            // Fallback
            QString fallbackPath = QString("://res/Player/hurtGirl%1.png").arg(i);
            QPixmap fallbackFrame(fallbackPath);
            if (!fallbackFrame.isNull()) {
                hurtFrames << fallbackFrame.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            }
        }
    }

    if (hurtFrames.isEmpty()) {
    }
}
void Player::playHurtAnimation()
{
    currentAnimType = AnimationType::HURT;
    currentFrame = 0;
    isMovingAnimation = false;
    animationTimer->stop();
    animationTimer->start(150);
}
