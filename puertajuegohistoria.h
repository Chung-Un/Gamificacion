#ifndef PUERTAJUEGOHISTORIA_H
#define PUERTAJUEGOHISTORIA_H

#include "scenebase.h"
#include <QGraphicsPixmapItem>
#include <QGraphicsTextItem>
#include <QTimer>

class PuertaJuegoHistoria : public SceneBase
{
    Q_OBJECT
public:
    explicit PuertaJuegoHistoria(int llavesObtenidas, QObject *parent = nullptr);
    QString getSceneName() const override { return "PuertaJuegoHistoria"; }


signals:
    void juegoTerminado(bool ganado);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

private:
    void loadScene() ;
    void verificarFin();
    void mostrarMensaje(bool ganado);

    int llaves;                                    // Llaves disponibles
    int candadosAbiertos;                          // Candados abiertos hasta ahora
    QList<QGraphicsPixmapItem*> candados;         // Lista de candados
    QGraphicsTextItem* mensajeFinal;              // Mensaje final
    QGraphicsRectItem* fondoMensaje;              // Fondo del mensaje
    QGraphicsRectItem* fondoLlave;
    QGraphicsRectItem* fondoInstru;
    QGraphicsTextItem* contadorLlaves;
};

#endif // PUERTAJUEGOHISTORIA_H
