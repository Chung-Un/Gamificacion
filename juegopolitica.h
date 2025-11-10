#ifndef JUEGOPOLITICA_H
#define JUEGOPOLITICA_H

#include "scenebase.h"
#include <QString>
#include <QQueue>
#include <QVector>
#include <QTimer>
#include <QStringList>
#include <QGraphicsTextItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>

class JuegoPolitica : public SceneBase
{
    Q_OBJECT

public:
    explicit JuegoPolitica(QObject* parent = nullptr);
    QString getSceneName() const override { return "JuegoPolitica"; }
    void terminarJuego();

signals:
    void juegoTerminado(bool ganado);

private slots:
    void generarItem();
    void moverItems();
    void siguientePregunta();

private:
    struct ItemConTexto {
        QGraphicsPixmapItem* pixmap;
        QGraphicsTextItem* texto;
        QString letra;
    };

    void loadScene() override;
    void setupCollisions();
    void setupInteractiveObjects();
    void setupInteractions();
    void mostrarPregunta();
    void mostrarOpciones();
    void limpiarItemsCayendo();
    void hacerCaerResultados();
    void mostrarResultadoFinal();
    void ocultarOpcionesYFondo();

    // Variables del juego
    QQueue<QString> preguntas;
    QQueue<QString> respuestasCorrectas;
    QQueue<QStringList> opciones;

    QVector<QGraphicsTextItem*> textosOpciones;
    QList<ItemConTexto> itemsCayendo;

    bool esperandoRespuesta;

    QTimer* timerItems;
    QTimer* timerMovimiento;

    QGraphicsTextItem* textPregunta;
    QGraphicsRectItem* barraPregunta;
    QGraphicsTextItem* textoOp;
    QGraphicsRectItem* fondoOpciones;

    int cantCorrectas;
    int errores;
    int noPregunta;
};

#endif // JUEGOPOLITICA_H
