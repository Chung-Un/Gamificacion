#ifndef RUNNINGMAN_H
#define RUNNINGMAN_H

#include <QGraphicsPixmapItem>
#include <QObject>
#include <QTimer>
#include <QVector>
#include <QPixmap>

class RunningMan : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    explicit RunningMan(QObject *parent = nullptr);

    // Métodos principales
    void startRunning();
    void stopRunning();
    void setVelocidad(double velocidad);
    double getVelocidad() const { return velocidad; }
    double getPosicionX() const { return x(); }

    // Método para mover automáticamente
    void mover();

signals:
    void llegadaAlFinal(bool gano); // Se emite cuando llega al final

private slots:
    void onAnimationTimer();

private:
    void loadSprites();

    // Animación
    QVector<QPixmap> runRightFrames;  // Sprites corriendo a la derecha
    QTimer* animationTimer;
    int currentFrame;

    // Movimiento
    double velocidad;                  // Píxeles por frame
    bool isRunning;

    // Tamaño
    QSize runningManSize;
};

#endif // RUNNINGMAN_H
