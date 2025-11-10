#ifndef SCENEBATALLA_H
#define SCENEBATALLA_H

#include "scenebase.h"
#include "player.h"
#include "npcbatalla.h"
#include <QTimer>
#include <QQueue>
#include <QGraphicsRectItem>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QGraphicsTextItem>

// Agregar estas constantes al inicio
const int DANIO_ATAQUE = 25;
const int MAX_PREGUNTAS_POR_JUGADOR = 4;
const int TIEMPO_TURNO = 15000;
const int PORCENTAJE_ACIERTO_NPC = 70;

class SceneBatalla : public SceneBase
{
    Q_OBJECT

public:
    QString getSceneName() const override { return "SceneBatalla"; }
    // Debug: ganar batalla al instante
    void debugGanarBatalla();
    void mostrarMensajeFinal(bool ganoPlayer);


    explicit SceneBatalla(Player* existingPlayer,Player::Bando bandoPlayer,int vidaInicialPlayer,QObject* parent = nullptr);

signals:
    void juegoTerminado(bool ganado);
private:
    // Variables existentes...
    Player* player;
    Player::Bando bandoPlayer;
    Player::Bando bandoNPC;
    int vidaNPC;
    int vidaPlayer;
    int vidaMaxima;
    bool turnoPlayer;
    bool esperandoRespuesta;
    bool batallaTerminada;
    int preguntasRespondidasPlayer;
    int preguntasRespondidasNPC;

    // UI elements...
    NpcBatalla* npc;

    // NUEVO: Agregar variable para verificación
    bool verificandoEnProceso;

    // NUEVO: Frames de sangre locales
    QVector<QPixmap> bloodFrames;

    QVector<QPixmap> waveFrames;
    void loadWaveFrames();
    void crearEfectoOnda(QPointF posicion);
    void playWinMusicForBand(Player::Bando band);


private slots:
    void loadScene();
    void setupUI();
    void setupPreguntas();
    void actualizarBatalla();
    void mostrarPregunta();
    void ocultarPregunta();
    void procesarRespuestaJugador(const QString& respuesta);
    void procesarRespuestaNPC();
    void atacarJugador();
    void atacarNPC(); // NUEVO: Separar el daño al NPC
    void terminarBatalla(bool ganoPlayer);
    void onAtaqueNPCCompletado();
    void onAtaquePlayerCompletado();
    void procesarTurno();
    void onSangreLanzada(QPointF posicion);
    void crearEfectoSangre(QPointF posicion);
    void verificarGanador();
    void alternarTurnoFijo();

    // NUEVO: Método para cargar frames de sangre
    void loadBloodFrames();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    // Timers
    QTimer* batallaTimer;
    QTimer* turnoTimer;

    // Sistema de preguntas
    QQueue<QString> preguntas;
    QQueue<QStringList> opciones;
    QQueue<QString> respuestasCorrectas;
    QString respuestaCorrectaActual;

    // UI elements
    QGraphicsRectItem* barraVidaPlayer;
    QGraphicsRectItem* barraVidaNPC;
    QGraphicsTextItem* textoVidaPlayer;
    QGraphicsTextItem* textoVidaNPC;
    QGraphicsTextItem* textoTurno;
    QGraphicsTextItem* textoContador;
    QGraphicsRectItem* barraPregunta;
    QGraphicsTextItem* textPregunta;
    QList<QGraphicsTextItem*> textosOpciones;

    QMediaPlayer* winPlayer= nullptr;
    QAudioOutput* winAudio= nullptr;

    bool mensajeFinalMostrado = false;
    bool terminandoEnCurso = false;

};

#endif // SCENEBATALLA_H
