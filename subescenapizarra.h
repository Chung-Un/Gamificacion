#ifndef SUBESCENAPIZARRA_H
#define SUBESCENAPIZARRA_H

#include "scenebase.h"
#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>

class SubEscenaPizarra : public SceneBase {
    Q_OBJECT
public:
    explicit SubEscenaPizarra(QObject* parent=nullptr);
    bool manejarTecla(int key);
    QString getSceneName() const override { return "SubEscenaPizarra"; }


signals:
    void pistaDesbloqueada(const QString& pista);
    void subescenaTerminada();

protected:
    void loadScene() override;

private:
    QString fondoPath;
    QGraphicsPixmapItem* bgItem = nullptr;
    void setBackgroundImage(const QString& resPath);
    void layoutBackground();

    bool respondida = false;
    bool pistaOtorgada = false;

    QGraphicsRectItem* feedbackPanel = nullptr;
    QGraphicsTextItem* feedbackText  = nullptr;
    void showFeedback(const QString& msg);

    void resolver(char opcion);
};

#endif
