#ifndef PLAYERSTATS_H
#define PLAYERSTATS_H

#include <QString>

struct PlayerRecord {
    QString name;
    qint32  band;   // 0 = Empirista, 1 = Racionalista
    double  l2Ms;   // último tiempo Nivel 2 (Ruleta), -1 si no hay
    double  l3Ms;   // último tiempo Nivel 3 (Batalla), -1 si no hay
    double  l4Ms;   // último tiempo Nivel 4 (CaminoUni u otro), -1 si no hay

    // Promedio considerando L2, L3, L4 disponibles
    double avgMs() const;
};

// ms -> "mm:ss.cc"
QString formatMs(double ms);

#endif // PLAYERSTATS_H
