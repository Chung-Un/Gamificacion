#ifndef STATSFILE_H
#define STATSFILE_H

#include "playerstats.h"
#include <QVector>
#include <QString>

class StatsFile {
public:
    static QString statsPath();

    // Carga todos los registros (migra v1/v2 -> v3 si hace falta).
    static bool loadAll(QVector<PlayerRecord>& out);

    // Inserta/actualiza el último tiempo de un nivel (2, 3 o 4).
    static bool upsertRun(const QString& playerName, qint32 band, int level /*2|3|4*/, double ms);
};

#endif // STATSFILE_H
