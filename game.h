#ifndef GAME_H
#define GAME_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QTimer>
#include <QKeyEvent>
#include <QString>
#include <QList>
#include <QElapsedTimer>
#include <QVector>
#include "statsfile.h"
#include "playerstats.h"
#include "player.h"
#include "scenebase.h"
#include "startscene.h"
#include "mapa.h"
#include "ruletascene.h"
#include "videoscene.h"
#include "scenebatalla.h"
#include "terracescene.h"
#include "caminouni.h"
#include "juegoarte.h"
#include "juegociencia.h"
#include "juegopolitica.h"
#include "juegohistoria.h"
#include "puertajuegohistoria.h"
#include "aulakantscene.h"
#include "subescenapizarra.h"
#include "subescenalibro.h"
#include "labdescartes.h"

class Game : public QMainWindow {
    Q_OBJECT
public:
    explicit Game(QWidget* parent = nullptr);
    ~Game();

    bool isKantCompletado() const { return kantCompletado; }
    bool isDescartesCompletado() const { return descartesCompletado; }

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

private slots:
    void updateMovement();

private:
    // --- helpers UI / movimiento ---
    void setupUI();
    void mostrarCollisionsDebug();
    void checkInteractions();
    void showInteractionText(const QString& text);
    void hideInteractionText();
    void checkCollisions();

    // --- escena ---
    void cambiarEscena(SceneBase* nuevaEscena);
    void cambiarEscena(SceneBase* nuevaEscena, const QPointF& spawn);

    QGraphicsView* view = nullptr;
    Player*        player = nullptr;
    SceneBase*     currentScene = nullptr;

    // movimiento
    double playerX = 400.0, playerY = 500.0;
    bool movingUp = false, movingDown = false, movingLeft = false, movingRight = false;
    bool movimientoHabilitado = true;

    // UI
    QGraphicsTextItem* interactionText = nullptr;
    bool isInteracting = false;
    QString currentInteraction;
    QRectF currentInteractiveArea;
    QList<QGraphicsRectItem*> debugCollisionRects;
    QTimer* movementTimer = nullptr;

    // progreso
    Mapa* mapa = nullptr;
    int   ganados = 0;
    bool  kantCompletado = false;
    bool  descartesCompletado = false;
    QString nombreUsuario;
    void iniciarBatallaL3(int vidaInicial);
    SceneBatalla* combateActual = nullptr;  // <--- NUEVO
    int vidaParaL3 = 0;

    void conectarTerraza(TerraceScene* terraza);
    void conectarStartScene(StartScene* start);
    void volverAlInicioDesdeTerraza();

    QElapsedTimer nivelTimer;
    bool nivelTimerOn = false;
    QVector <qint64> tiemposMs;
    void startLevelTimer(){
        nivelTimer.restart();
        nivelTimerOn = true;
    }
    void stopLevelTimerAndStore(){
        if(nivelTimerOn){
            tiemposMs.push_back(nivelTimer.elapsed());
            nivelTimerOn = false;
        }
    }
    void trySaveRankingIfComplete();
};

#endif // GAME_H
