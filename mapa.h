#ifndef MAPA_H
#define MAPA_H

#include "scenebase.h"
#include "Grafos.h"
#include <QGraphicsObject>
#include <QMap>
#include <QSet>

class QGraphicsSceneMouseEvent;
class QPainter;

class LevelNode;

// Escena de Mapa (click con mouse)
class Mapa : public SceneBase {
    Q_OBJECT
public:
    explicit Mapa(QObject* parent=nullptr);

    QString getSceneName() const override { return "Mapa"; }

    void setUnlocked(const QSet<QString>& set); // "L1","L2","L3","L4"
    void unlock(const QString& name);
    void unlockVecinos(const QString& name);

signals:
    void levelChosen(const QString& name);

protected:
    void loadScene() override;
    void setupCollisions()  {}
    void setupInteractions()  {}

private:
    void buildBackground();
    void buildTitle();
    void buildGraph();     // L1->L2->L3->L4
    void layoutNodes();
    void drawEdges();
    void refreshLocks();

    Grafos grafo;                                   // usa tu clase
    QMap<QString, LevelNode*> nodes;                // "L1"... "L4"
    QSet<QString> unlocked;
};

// Nodo clickable
class LevelNode : public QGraphicsObject {
    Q_OBJECT
public:
    LevelNode(const QString& name,
              const QString& label,
              const QPointF& pos,
              const QString& iconRes,
              QGraphicsItem* parent=nullptr);

    QRectF boundingRect() const override { return QRectF(-90,-90,180,180); }
    void paint(QPainter* p, const QStyleOptionGraphicsItem*, QWidget*) override;

    void setLocked(bool b){ locked = b; update(); }
    bool isLocked() const { return locked; }
    QString name() const  { return id; }

signals:
    void clicked(const QString& name);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* ev) override;

private:
    QString id;     // "L1"... "L4"
    QString label;
    bool locked = true;
    QPixmap icon;
};

#endif // MAPA_H
