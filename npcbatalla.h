#ifndef NPCBATALLA_H
#define NPCBATALLA_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QTimer>
#include <QPropertyAnimation>
#include "player.h"

class NpcBatalla : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
    Q_PROPERTY(QPointF pos READ pos WRITE setPos)

public:
    explicit NpcBatalla(Player::Bando bando, QObject* parent = nullptr);
    ~NpcBatalla();

    QVector<QPixmap> bloodFrames;


    // Señales
signals:
    void attackCompleted();
    void bloodSplatter(QPointF position);
    void deathCompleted();

public slots:
    void startAttack();
    void takeDamage();
    void playDeathAnimation();
    void moveToPosition(int targetX);
    void retrieveToOriginalPos();
    void stopAllAnimations();

private slots:
    void updateAnimation();
    void onAttackFinished();
    void onDeathFinished();
    void onMovementFinished();

private:
    void loadSprites();
    QString getSpriteFolder() const;
    QSize npcSize() const { return QSize(150, 150); }

    // Timers y animaciones
    QTimer* animationTimer;
    QTimer* attackTimer;
    QTimer* deathTimer;
    QPropertyAnimation* movementAnimation;

    // Frames de animación
    QVector<QPixmap> idleFrames;
    QVector<QPixmap> attackFrames;
    QVector<QPixmap> damageFrames;
    QVector<QPixmap> deathFrames;

    // Estados
    int currentFrame;
    bool isAttacking;
    bool isTakingDamage;
    bool isMoving;
    bool isDyingAnimation;
    bool isDeadState;

    QPointF originalPos;
    Player::Bando bandoActual;

    // NUEVO: Control de sangre
    bool bloodSplattered;
};

#endif // NPCBATALLA_H
