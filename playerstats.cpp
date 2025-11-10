#include "playerstats.h"

double PlayerRecord::avgMs() const {
    int n = 0;
    double sum = 0.0;
    if (l2Ms >= 0.0) { sum += l2Ms; ++n; }
    if (l3Ms >= 0.0) { sum += l3Ms; ++n; }
    if (l4Ms >= 0.0) { sum += l4Ms; ++n; }
    if (n == 0) return -1.0;
    return sum / double(n);
}

QString formatMs(double ms) {
    if (ms < 0.0) return "--:--.--";
    const qint64 total = static_cast<qint64>(ms);
    const qint64 s     = total / 1000;
    const qint64 m     = s / 60;
    const qint64 rems  = s % 60;
    const qint64 cs    = (total % 1000) / 10;
    return QString("%1:%2.%3")
        .arg(m,    2, 10, QLatin1Char('0'))
        .arg(rems, 2, 10, QLatin1Char('0'))
        .arg(cs,   2, 10, QLatin1Char('0'));
}
