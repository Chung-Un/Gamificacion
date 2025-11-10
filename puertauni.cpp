#include "puertauni.h"
#include <QPixmap>
#include <QGraphicsPixmapItem>

PuertaUni::PuertaUni(QObject* parent)
    : SceneBase(parent)
{
    // Tamaño grande de escena (como antes)
    setSceneRect(0, 0, 1000, 1000);

    // Cargar el fondo y centrarlo dentro de la escena
    QPixmap bg("://res/puerta_uni.png");
    if (!bg.isNull()) {
        // Escala proporcional manteniendo calidad
        QPixmap scaled = bg.scaled(1000, 1000, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        // Calcular posición centrada
        int x = (sceneRect().width() - scaled.width()) / 2;
        int y = (sceneRect().height() - scaled.height()) / 2;

        // Crear un item para el fondo centrado
        QGraphicsPixmapItem* bgItem = new QGraphicsPixmapItem(scaled);
        bgItem->setPos(x, y);
        bgItem->setZValue(-100);
        addItem(bgItem);
        backgroundItem = bgItem; // opcional, si SceneBase lo usa
    } else {
        QPixmap solid(sceneRect().width(), sceneRect().height());
        solid.fill(QColor(20, 20, 40));
        setBackground(solid);
    }

    // Área interactiva de la puerta (ajusta coordenadas si cambias la imagen)
    addInteractiveArea(QRectF(390, 500, 390, 70),
                       "Presiona E para abrir la puerta a la universidad");

// En PuertaUni::PuertaUni, después de addInteractiveArea(...)
#ifdef QT_DEBUG
    {
        auto dbg = new QGraphicsRectItem(QRectF(390,739,390,120));
        dbg->setBrush(QColor(0,255,0,60));
        dbg->setPen(QPen(Qt::green, 2, Qt::DashLine));
        dbg->setZValue(999);
        addItem(dbg);
    }
#endif


    addCollisionArea(QRectF(0,520,1000,10));
    addCollisionArea(QRectF(0,800,1000,10));
}
