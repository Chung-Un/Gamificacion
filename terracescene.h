#ifndef TERRACESCENE_H
#define TERRACESCENE_H

#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QGraphicsProxyWidget>
#include <QPushButton>
#include <QTimer>
#include "scenebase.h"
#include "statsfile.h"
#include "playerstats.h"
#include "startscene.h"

class TerraceScene : public SceneBase
{
    Q_OBJECT
public:
    explicit TerraceScene(QObject* parent = nullptr);
    void loadScene() override;
    QString getSceneName() const override { return "TerraceScene"; }
    void setProgreso(bool kant, bool descartes);

signals:
    void volverAlInicio();

private:
    bool kantOk = false;
    bool descartesOk = false;

    QGraphicsRectItem* felicRect = nullptr;
    QGraphicsTextItem* felicText = nullptr;
    QGraphicsRectItem* rankingRect = nullptr;
    QGraphicsTextItem* rankingText = nullptr;

    QGraphicsProxyWidget* btnSalirProxy = nullptr;
    QGraphicsProxyWidget* btnInicioProxy = nullptr;
    QPushButton* btnSalir = nullptr;
    QPushButton* btnInicio = nullptr;

    QTimer* overlayTimer = nullptr;

    void setupCollisions();
    void setupInteractions();
    void mostrarFelicidades();
    void mostrarRanking();
    void mostrarBotonesFin();
    void startOverlayFollow();
    QPointF viewCenter() const;

};

#endif
