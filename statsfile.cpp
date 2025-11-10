#include "statsfile.h"
#include <QFile>
#include <QStandardPaths>
#include <QDir>
#include <QDataStream>

static const quint32 MAGIC   = 0xBADA5513;
static const qint32  VERSION = 3; // v3: l2Ms, l3Ms, l4Ms

QString StatsFile::statsPath() {
    const QString base = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(base);
    return base + "/stats.dat";
}

static bool saveAllV3(const QVector<PlayerRecord>& in) {
    QFile f(StatsFile::statsPath());
    if (!f.open(QIODevice::WriteOnly)) return false;

    QDataStream s(&f);
    s.setVersion(QDataStream::Qt_6_0);

    s << MAGIC << VERSION << qint32(in.size());
    for (const auto& r : in) {
        s << r.name << r.band << r.l2Ms << r.l3Ms << r.l4Ms;
        if (s.status() != QDataStream::Ok) return false;
    }
    return true;
}

bool StatsFile::loadAll(QVector<PlayerRecord>& out) {
    out.clear();
    QFile f(statsPath());
    if (!f.exists()) return true;
    if (!f.open(QIODevice::ReadOnly)) return false;

    QDataStream s(&f);
    s.setVersion(QDataStream::Qt_6_0);

    quint32 magic; qint32 ver; qint32 count;
    s >> magic >> ver >> count;
    if (magic != MAGIC || count < 0 || count > 100000) return false;

    out.reserve(count);

    if (ver == 3) {
        for (int i = 0; i < count; ++i) {
            PlayerRecord r;
            s >> r.name >> r.band >> r.l2Ms >> r.l3Ms >> r.l4Ms;
            if (s.status() != QDataStream::Ok) return false;
            out.push_back(r);
        }
        return true;
    }

    // v2 -> v3 migración (tenía l2Ms y l3Ms)
    if (ver == 2) {
        for (int i = 0; i < count; ++i) {
            PlayerRecord r;
            s >> r.name >> r.band >> r.l2Ms >> r.l3Ms;
            if (s.status() != QDataStream::Ok) return false;
            r.l4Ms = -1.0; // desconocido
            out.push_back(r);
        }
        return saveAllV3(out);
    }

    // v1 -> v3 migración (tenía avg y runs)
    if (ver == 1) {
        for (int i = 0; i < count; ++i) {
            QString name; qint32 band; double avg; qint32 runs;
            s >> name >> band >> avg >> runs;
            if (s.status() != QDataStream::Ok) return false;

            PlayerRecord r;
            r.name = name;
            r.band = band;
            // No sabemos los niveles; para no perder ranking, coloca avg en L2/L3 y -1 en L4.
            r.l2Ms = avg;
            r.l3Ms = avg;
            r.l4Ms = -1.0;
            out.push_back(r);
        }
        return saveAllV3(out);
    }

    return false; // versión desconocida
}

bool StatsFile::upsertRun(const QString& playerName, qint32 band, int level, double ms) {
    if (level != 2 && level != 3 && level != 4) return false;

    QVector<PlayerRecord> all;
    if (!loadAll(all)) return false;

    int idx = -1;
    for (int i = 0; i < all.size(); ++i) {
        if (all[i].name.compare(playerName, Qt::CaseInsensitive) == 0) { idx = i; break; }
    }

    if (idx < 0) {
        PlayerRecord r;
        r.name = playerName;
        r.band = band;
        r.l2Ms = -1.0;
        r.l3Ms = -1.0;
        r.l4Ms = -1.0;
        if (level == 2) r.l2Ms = ms;
        else if (level == 3) r.l3Ms = ms;
        else                 r.l4Ms = ms;
        all.push_back(r);
    } else {
        PlayerRecord& r = all[idx];
        r.band = band; // por si cambió
        if (level == 2)      r.l2Ms = ms;  // reemplaza por el más reciente
        else if (level == 3) r.l3Ms = ms;
        else                 r.l4Ms = ms;
    }

    return saveAllV3(all);
}
