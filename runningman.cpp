#include "runningman.h"
#include <QDebug>
#include <QGraphicsScene>

RunningMan::RunningMan(QObject *parent)
    : QObject(parent)
    , QGraphicsPixmapItem()
    , currentFrame(0)
    , velocidad(5.0)
    , isRunning(false)
    , runningManSize(60, 79)
{
    setZValue(50);

    loadSprites();

    animationTimer = new QTimer(this);
    connect(animationTimer, &QTimer::timeout, this, &RunningMan::onAnimationTimer);

    if (!runRightFrames.isEmpty()) {
        setPixmap(runRightFrames.first());
    }
}

void RunningMan::loadSprites()
{
    runRightFrames.clear();

    int w = runningManSize.width();
    int h = runningManSize.height();

    for (int i = 1; i <= 8; i++) {
        QString path = QString("://res/RunningMan/run%1.png").arg(i);
        QPixmap frame(path);

        if (!frame.isNull()) {
            runRightFrames << frame.scaled(w, h);
            qDebug() << "✅ Sprite cargado:" << path;
        } else {
            qDebug() << "❌ No se pudo cargar sprite:" << path;
            QPixmap placeholder(w, h);
            placeholder.fill(QColor(255, 0, 0, 128));
            runRightFrames << placeholder;
        }
    }

    qDebug() << "Sprites cargados:" << runRightFrames.size();
}

void RunningMan::onAnimationTimer()
{
    if (!runRightFrames.isEmpty()) {
        currentFrame = (currentFrame + 1) % runRightFrames.size();
        setPixmap(runRightFrames[currentFrame]);
    }

    if (isRunning) {
        mover();
    }
}

void RunningMan::startRunning()
{
    if (!isRunning) {
        isRunning = true;
        animationTimer->start(100);
    }
}

void RunningMan::stopRunning()
{
    if (isRunning) {
        isRunning = false;
        animationTimer->stop();
    }
}

void RunningMan::setVelocidad(double nuevaVelocidad)
{
    velocidad = nuevaVelocidad;
}

void RunningMan::mover()
{
    if (!isRunning) return;

    double nuevaX = x() + velocidad;
    setX(nuevaX);

    if (scene()) {
        QRectF sceneRect = scene()->sceneRect();
        double limiteDerecho = sceneRect.width() - boundingRect().width();

        if (nuevaX >= limiteDerecho) {
            stopRunning();
            emit llegadaAlFinal(true);
        }
    }

    static int lastDebugX = 0;
    if (static_cast<int>(nuevaX) / 100 != lastDebugX / 100) {
        lastDebugX = nuevaX;
    }
}
