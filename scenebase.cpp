#include "scenebase.h"
#include <QGraphicsSceneMouseEvent>
#include <QDebug>

SceneBase::SceneBase(QObject *parent)
    : QGraphicsScene(parent)
    , backgroundItem(nullptr)
{
}

void SceneBase::loadScene()
{
}

void SceneBase::addCollisionArea(const QRectF &area)
{
    collisionAreas.append(area);
}

void SceneBase::addInteractiveArea(const QRectF &area, const QString &message)
{
    interactiveAreas.append(qMakePair(area, message));
}

void SceneBase::addObject(QGraphicsItem *item)
{
    sceneObjects.append(item);
    this->addItem(item);
}

void SceneBase::setBackground(const QPixmap &background)
{
    if (!backgroundItem) {
        backgroundItem = new QGraphicsPixmapItem();
        this->addItem(backgroundItem);
    }
    backgroundItem->setPixmap(background);
    backgroundItem->setZValue(-100);
    backgroundItem->setAcceptedMouseButtons(Qt::NoButton);
    backgroundItem->setFlag(QGraphicsItem::ItemIsSelectable,false);
}

QString SceneBase::getInteractionMessage(const QRectF &area) const
{
    for (const auto &pair : interactiveAreas) {
        if (pair.first == area) {
            return pair.second;
        }
    }
    return "";
}

QList<QRectF> SceneBase::getInteractiveAreas() const
{
    QList<QRectF> areas;
    for (const auto &pair : interactiveAreas) {
        areas.append(pair.first);
    }
    return areas;
}

void SceneBase::clearInteractiveAreas() {
    interactiveAreas.clear();

}

