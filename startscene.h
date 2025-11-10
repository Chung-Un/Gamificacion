#ifndef STARTSCENE_H
#define STARTSCENE_H

#include "scenebase.h"
#include <QGraphicsTextItem>
#include <QGraphicsRectItem>

class StartScene : public SceneBase
{
    Q_OBJECT
public:
    explicit StartScene(QObject* parent = nullptr);
    QString getSceneName() const override { return "StartScene"; }

signals:
    void pedirNombreUsuario();        // para abrir input de usuario
    void pedirTipoPartida();          // para abrir popup Nueva/Cargar

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

private:
    void loadScene();

    QGraphicsTextItem* titulo = nullptr;
    QGraphicsRectItem* btnIngresar = nullptr;
    QGraphicsTextItem* txtIngresar = nullptr;
    QGraphicsRectItem* btnEmpezar = nullptr;
    QGraphicsTextItem* txtEmpezar = nullptr;


};

#endif // STARTSCENE_H
