#ifndef JUEGOARTE_H
#define JUEGOARTE_H

#include "scenebase.h"
#include <QString>
#include <QQueue>
#include <QVector>
#include <QGraphicsPixmapItem>
#include <QTimer>
#include <QGraphicsItem>
#include <QStringList>
#include <QPointF>

class JuegoArte : public SceneBase
{
    Q_OBJECT
public:
    explicit JuegoArte(QObject *parent = nullptr);
    QString getSceneName() const override { return "JuegoArte"; }

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event);

private slots:
    void onTimeout();
    void actualizarTimer();
    void reiniciarTimer();

signals:
    void juegoTerminado(bool ganado);
private:
    void loadScene();
    void setupCollisions();
    void setupInteractiveObjects();
    void setupInteractions();
    void cambiarPregunta();
    void cambiarOpciones();
    void mostrarResultado(bool correcta, int index);
    void terminarJuego();

    void onBolsaClicked(int index);

    QQueue <QString> preguntas;
    QQueue<QString> respuestasCorrectas;
    QQueue<QStringList> opciones;
    QQueue<bool> respondidasCorrectamente;

    QTimer* timer;
    QTimer* timerVisual;
    QGraphicsTextItem* textTiempo;
    QGraphicsTextItem* textPregunta;
    QGraphicsTextItem* textoInstrucciones;
    QGraphicsPixmapItem* glass;
    QVector<QGraphicsPixmapItem*> bolsasCerradas;
    QVector<QGraphicsPixmapItem*>bolsasAbiertas;
    QVector<QGraphicsTextItem*> textosOpciones;
    QVector<QPointF> originalPositionsCerradas;
    QVector<QPointF> originalPositionsAbiertas;
    QVector<QPointF> originalPositionsTextos;

    int noPregunta;
    int time;
    int cantCorrectas;
    bool instruccionesLeidas;
    bool interactionBlocked;

private:

};

#endif
