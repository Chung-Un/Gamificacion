#ifndef PUERTAUNI_H
#define PUERTAUNI_H

#include "scenebase.h"
#include <QPixmap>

class PuertaUni : public SceneBase {
    Q_OBJECT
public:
    explicit PuertaUni(QObject* parent = nullptr);

    // ← OBLIGATORIO
    QString getSceneName() const override { return "PuertaUni"; }
};

#endif // PUERTAUNI_H
