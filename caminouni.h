#ifndef CAMINOUNI_H
#define CAMINOUNI_H

#include "scenebase.h"
#include <QPixmap>

class CaminoUni : public SceneBase {
    Q_OBJECT
public:
    explicit CaminoUni(QObject* parent = nullptr);

    // ← OBLIGATORIO: implementa el puro virtual de SceneBase
    QString getSceneName() const override { return "CaminoUni"; }
};

#endif // CAMINOUNI_H
