#ifndef PLAYER_H
#define PLAYER_H

#include <QGraphicsPixmapItem>
#include <QObject>
#include <QTimer>
#include <QVector>
#include <QPixmap>
#include <QStringList>
#include <QSize>

class Player : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    enum class Bando { NULO, EMPIRISTA, RACIONALISTA };
    enum class NivelEpistemologico { APRENDIZ, DISCIPULO, ALUMNO, MASTER };
    enum class Direction { IDLE, UP, DOWN, LEFT, RIGHT };
    enum class AnimationType { WALK, RUN, DISINTEGRATE, ATTACK, HURT };  // ← AGREGAR: RUN

    explicit Player(QObject *parent = nullptr);

    // Animación
    void setAnimation(bool moving, Direction direction);
    void playDisintegrateAnimation();
    void playAttackAnimation();
    void resetAnimation();
    void playHurtAnimation();
    void startRunning();  // ← AGREGAR: Método para empezar a correr
    void stopRunning();   // ← AGREGAR: Método para parar de correr

    Bando bandoActual = Bando::NULO;

    // Tamaño
    void setPlayerSize(int width, int height);
    void resetPlayerSize();
    QSize getCurrentSize() const { return currentSize; }

    // Sistema filosófico
    void unirseABando(Bando bando);
    void ganarPuntos(int puntos, Bando bando = Bando::NULO);
    void desbloquearConcepto(const QString& concepto);

    void cambiarBando(Bando nuevoBando);
    void cargarSpritesBando();

    Bando getBandoActual() const { return bandoActual; }
    NivelEpistemologico getNivel() const { return nivelEpis; }
    AnimationType getCurrentAnimType() const {return currentAnimType;}
    int getCurrentFrame() const {return currentFrame;}

signals:
    void animationFinished();

private slots:
    void onAnimationTimer();

private:
    void loadSprites();
    void loadAttackSprites();
    void loadDisintegrateSprites();
    void loadRunSprites();
    void updateNivelEpis();
    void loadHurtSprites();
    void loadWalkSprites(const QString& prefix, int w, int h);

    // Sistema filosófico
    NivelEpistemologico nivelEpis;
    int puntosRacionalistas;
    int puntosEmpiristas;
    int puntosCriticos;
    int totalXP;
    int vidasEpistemologicas;
    QStringList conceptosDesbloqueados;
    QStringList insigniasObtenidas;

    // Animación
    QVector<QPixmap> idleFrames;
    QVector<QPixmap>attackFrames;
    QVector<QPixmap> walkUpFrames;
    QVector<QPixmap> walkDownFrames;
    QVector<QPixmap> walkLeftFrames;
    QVector<QPixmap> walkRightFrames;
    QVector<QPixmap> disintegrateFrames;
    QVector<QPixmap> hurtFrames;
    QVector<QPixmap> runFrames;  // ← AGREGAR: Frames de correr

    int currentFrame;
    QTimer* animationTimer;
    bool isMovingAnimation;
    Direction currentDirection;
    AnimationType currentAnimType;

    // Tamaño
    QSize originalSize;
    QSize currentSize;


    QString spriteBaseEmpirista = "://res/Player/empirista_";
    QString spriteBaseRacionalista = "://res/Player/racionalista_";
};

#endif // PLAYER_H
