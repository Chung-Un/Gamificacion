#ifndef SCENEBASE_H
#define SCENEBASE_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QList>
#include <QRectF>
#include <QString>
#include <QPair>
#include <QGraphicsSceneMouseEvent>

class SceneBase : public QGraphicsScene
{
    Q_OBJECT

public:
    SceneBase(QObject *parent = nullptr);
    virtual ~SceneBase() = default;

    QString name;


    // Métodos virtuales puros
    virtual void loadScene();
    virtual QString getSceneName() const = 0;

    // Métodos comunes
    void addCollisionArea(const QRectF &area);
    void addInteractiveArea(const QRectF &area, const QString &message);
    void addObject(QGraphicsItem *item);
    void setBackground(const QPixmap &background);
    void clearInteractiveAreas();


    // Getters


    QList<QRectF> getCollisionAreas() const { return collisionAreas; }
    QList<QRectF> getInteractiveAreas() const;
    QString getInteractionMessage(const QRectF &area) const;
    QGraphicsPixmapItem* getBackgroundItem() const { return backgroundItem; }
    QList<QGraphicsItem*> getSceneObjects() const { return sceneObjects; }

protected:
    QGraphicsPixmapItem *backgroundItem;
    QList<QRectF> collisionAreas;
    QList<QPair<QRectF, QString>> interactiveAreas; // Cambiado a QList de pares
    QList<QGraphicsItem*> sceneObjects;

};

#endif // SCENEBASE_H
