#include "caminouni.h"

CaminoUni::CaminoUni(QObject* parent)
    : SceneBase(parent)
{
    setSceneRect(0,0,1000,700);

    QPixmap bg("://res/pista.png");
    if (!bg.isNull()) {
        setBackground(bg.scaled(sceneRect().size().toSize(),
                                Qt::KeepAspectRatioByExpanding,
                                Qt::SmoothTransformation));
    } else {
        QPixmap solid(sceneRect().width(), sceneRect().height());
        solid.fill(QColor(40,40,60));
        setBackground(solid);
    }

    addCollisionArea(QRectF(0,165,374,10));
    addCollisionArea(QRectF(650,165,499,10));
    addCollisionArea(QRectF(405,90,195,10));

    addInteractiveArea(QRectF(405, 165, 195, 10),
                       "Presiona E para entrar a la universidad");
}
