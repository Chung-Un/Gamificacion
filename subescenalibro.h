#ifndef SUBESCENALIBRO_H
#define SUBESCENALIBRO_H

#include "scenebase.h"
#include <QRectF>
#include <QSet>
#include <QVector>
#include <QStringList>

struct LibroPregunta {
    QString enunciado;
    QStringList opciones;
    QChar correcta;
    QString pista;
};

class SubEscenaLibro : public SceneBase
{
    Q_OBJECT
public:
    explicit SubEscenaLibro(QObject* parent = nullptr);
    QString getSceneName() const override { return "SubEscenaLibro"; }

    void setBackgroundImage(const QString& resPath);
    void setPreguntas(const LibroPregunta& p1, const LibroPregunta& p2);

    bool manejarTecla(int key);

    void toggleDebugAreas();

signals:
    void pistaDesbloqueada(const QString& pista);
    void subescenaTerminada();

protected:
    void loadScene() override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

private:
    void setupInteractions();
    void ensureHoverHint();
    void mostrarPreguntaDeLibro(int cual);
    void ocultarPregunta();
    void resolver(bool correcta, const QString& pista);

    void drawDebugAreas();
    void clearDebugAreas();

private:
    QString fondoPath;
    QRectF hotspotLibro1;
    QRectF hotspotLibro2;

    bool preguntasCargadas = false;
    LibroPregunta libQ1, libQ2;

    bool preguntaActiva = false;
    int  libroActual = 0;

    QSet<int> librosResueltos;
    QSet<int> librosConPista;

    QGraphicsTextItem* hoverHint = nullptr;
    QGraphicsRectItem* panelPregunta = nullptr;
    QGraphicsTextItem* txtEnunciado = nullptr;
    QVector<QGraphicsTextItem*> txtOpciones;

    bool debugOn = false;
    QList<QGraphicsRectItem*> debugRects;
};

#endif // SUBESCENALIBRO_H
