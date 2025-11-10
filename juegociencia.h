#ifndef JUEGOCIENCIA_H
#define JUEGOCIENCIA_H

#include "scenebase.h"
#include "runningman.h"
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QGraphicsRectItem>
#include <QTimer>
#include <QQueue>

class JuegoCiencia : public SceneBase
{
    Q_OBJECT

public:
    explicit JuegoCiencia(QObject *parent = nullptr);
    QString getSceneName() const override { return "JuegoCiencia"; }


signals:
    void juegoTerminado(bool ganado);

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    void loadScene();
    void iniciarCarrera();
    void actualizarCarrera();
    void mostrarPregunta();
    void ocultarPregunta();
    void procesarRespuesta(const QString& respuesta);
    void darBoostAlPlayer();
    void verificarGanador();
    void terminarJuego(bool gano);

    void actualizarPosicionPlayer();

    // Player y rival
    RunningMan* runningMan;
    double playerPosX;
    double playerVelocidadBase;
    double playerVelocidadActual;
    bool playerConBoost;
    int boostDuracion;
    int respuestasCorrectasCount;
    int respuestasMalas;

    // Preguntas
    QQueue<QString> preguntas;
    QQueue<QString> respuestasCorrectas;
    QQueue<QStringList> opciones;
    int preguntaActual;
    bool esperandoRespuesta;
    QString respuestaCorrectaActual; // ← NUEVO: Guardar respuesta actual

    // UI de preguntas
    QGraphicsRectItem* barraPregunta;
    QGraphicsTextItem* textPregunta;
    QList<QGraphicsTextItem*> textosOpciones;

    // Timers
    QTimer* carreraTimer;
    QTimer* preguntaTimer;

    // Configuración de la carrera
    static const double META_X;
    static const int TIEMPO_PREGUNTA = 2000;

    void mostrarMensajeFinal(const QString& mensaje);

    // Estado del juego
    bool juegoIniciado;
    bool terminado;

    // UI de carrera
    QGraphicsTextItem* textoDistanciaPlayer;
    QGraphicsTextItem* textoDistanciaRival;
    QGraphicsRectItem* barraProgresoPlayer;
    QGraphicsRectItem* barraProgresoRival;
};

#endif // JUEGOCIENCIA_H
