#include "startscene.h"
#include <QGraphicsSceneMouseEvent>
#include <QFont>
#include <QBrush>
#include <QPen>

StartScene::StartScene(QObject* parent)
    : SceneBase(parent)
{
    setSceneRect(0,0,800,600);

    QPixmap bg("://res/start.png");
    setBackground(bg.scaled(800,600, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
    loadScene();
}

void StartScene::loadScene()
{
    titulo = new QGraphicsTextItem("GAMIFICACIÓN");
    titulo->setDefaultTextColor(Qt::white);
    titulo->setFont(QFont("Minecraft", 42, QFont::Bold));
    titulo->setPos(160, 120);
    titulo->setZValue(10);
    addObject(titulo);

    btnIngresar = new QGraphicsRectItem(0,0, 320, 60);
    btnIngresar->setBrush(QColor(70, 23, 180));
    btnIngresar->setPen(QPen(Qt::white, 3));
    btnIngresar->setPos(240, 260);
    btnIngresar->setZValue(10);
    addObject(btnIngresar);

    txtIngresar = new QGraphicsTextItem("Ingresar usuario");
    txtIngresar->setDefaultTextColor(Qt::white);
    txtIngresar->setFont(QFont("Minecraft", 16, QFont::Bold));
    txtIngresar->setPos(270, 275);
    txtIngresar->setZValue(11);
    addObject(txtIngresar);

    // Botón Empezar (abre popup Nueva/Cargar)
    btnEmpezar = new QGraphicsRectItem(0,0, 320, 60);
    btnEmpezar->setBrush(QColor(80, 16, 86));
    btnEmpezar->setPen(QPen(Qt::white,3));
    btnEmpezar->setPos(240, 340);
    btnEmpezar->setZValue(10);
    addObject(btnEmpezar);

    txtEmpezar = new QGraphicsTextItem("Empezar");
    txtEmpezar->setDefaultTextColor(Qt::white);
    txtEmpezar->setFont(QFont("Minecraft", 16, QFont::Bold));
    txtEmpezar->setPos(350, 355);
    txtEmpezar->setZValue(11);
    addObject(txtEmpezar);
}

void StartScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsItem* it = itemAt(event->scenePos(), QTransform());
    if (!it) return;

    if (it == btnIngresar || it == txtIngresar) {
        emit pedirNombreUsuario();
        event->accept();
        return;
    }
    if (it == btnEmpezar || it == txtEmpezar) {
        emit pedirTipoPartida();
        event->accept();
        return;
    }

    SceneBase::mousePressEvent(event);
}
