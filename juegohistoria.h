#ifndef JUEGOHISTORIA_H
#define JUEGOHISTORIA_H

#include "scenebase.h"
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsTextItem>
#include <QGraphicsRectItem>
#include <QTimer>
#include <QQueue>

class JuegoHistoria : public SceneBase
{
    Q_OBJECT

public:
    explicit JuegoHistoria(QObject *parent = nullptr);
    void verificarLlavesCercanas();
    QString getSceneName() const override { return "JuegoHistoria"; }
    int getLlavesRecolectadas() const{return llavesConseguidas;}


signals:
    void juegoTerminado(bool ganado);

public slots:
    void verificarCambioEscena(); // ← AGREGAR: Declarar como slot público

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    void loadScene();
    void setupCollisions();
    void setupInteractions();
    void setupInteractiveObjects();
    void mostrarPregunta(int indiceLlave);
    void mostrarOpciones();
    void ocultarPreguntaYOpciones();
    void procesarRespuesta(const QString& respuesta);
    void recogerLlave(int indice);
    void actualizarEstado();
    void actualizarTimer();
    void onTimeout();
    void reiniciarTimer();
    void terminarJuego();

    QList<QGraphicsPixmapItem*> llaves;
    QList<bool> llavesRecogidas;
    QGraphicsTextItem* textoEstado;
    QGraphicsTextItem* textoInteraccion;
    QGraphicsTextItem* textoSalida; // ← AGREGAR: Para el mensaje de salida

    QQueue<QString> preguntas;
    QQueue<QString> respuestasCorrectas;
    QQueue<QStringList> opciones;

    QTimer* timer;
    QTimer* timerVisual;
    int time;

    int cantCorrectas;
    int noPregunta;
    int llavesConseguidas;
    int llaveActual;

    bool interactionBlocked;
    bool esperandoRespuesta;

    QGraphicsTextItem* textPregunta;
    QGraphicsRectItem* barraPregunta;
    QGraphicsTextItem* textTiempo;
    QList<QGraphicsTextItem*> textosOpciones;

    QPointF posicionCambioEscena;
};

#endif // JUEGOHISTORIA_H
