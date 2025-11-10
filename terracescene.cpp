#include "terracescene.h"
#include <QGraphicsView>
#include <QApplication>
#include <QPixmap>
#include <QDebug>
#include <QTimer>

TerraceScene::TerraceScene(QObject *parent)
    : SceneBase(parent)
{
    loadScene();
}

void TerraceScene::loadScene()
{
    setSceneRect(0, 0, 1000, 1000);

    QPixmap bgPixmap("://res/terrace.jpg");
    if (!bgPixmap.isNull()) {
        setBackground(bgPixmap.scaled(1000, 1000, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
    }

    setupCollisions();
    setupInteractions();

    if (kantOk && descartesOk) {
        QTimer::singleShot(150, this, [this](){ mostrarFelicidades(); });
    }
}

void TerraceScene::setupCollisions()
{
    addCollisionArea(QRectF(35, 300, 1000, -250));
    addCollisionArea(QRectF(995, 360, 10, 225));
    addCollisionArea(QRectF(35, 380, -30, 220));
    addCollisionArea(QRectF(35,625,1000,40));
}

void TerraceScene::setupInteractions()
{
    addInteractiveArea(QRectF(930, 360, 50, 225),
                       "Presiona E para entrar a El aula de Emmanuel Kant");
    addInteractiveArea(QRectF(70,360,50,225),
                       "Presiona E para entrar a el laboratorio de la duda");
}

void TerraceScene::setProgreso(bool kant, bool descartes)
{
    kantOk = kant;
    descartesOk = descartes;

    if (kantOk && descartesOk) {
        QTimer::singleShot(150, this, [this](){ mostrarFelicidades(); });
    }
}

void TerraceScene::mostrarFelicidades()
{
    if (felicRect) { felicRect->setVisible(true); startOverlayFollow(); return; }

    felicRect = new QGraphicsRectItem(0, 0, 520, 120);
    felicRect->setBrush(QColor(20,20,30,230));
    felicRect->setPen(QPen(Qt::white, 3));
    felicRect->setZValue(1200);
    addItem(felicRect);

    felicText = new QGraphicsTextItem("¡Felicidades! Ganaste gamificación", felicRect);
    felicText->setDefaultTextColor(Qt::yellow);
    felicText->setFont(QFont("Minecraft", 16, QFont::Bold));
    felicText->setTextWidth(480);
    felicText->setPos(20, 40);

    const QPointF c = viewCenter();
    felicRect->setPos(c.x() - 260.0, c.y() - 60.0);
    startOverlayFollow();

    QTimer::singleShot(3000, this, [this](){
        if (felicRect) felicRect->setVisible(false);
        QTimer::singleShot(2000, this, [this](){ mostrarRanking(); });
    });
}

void TerraceScene::mostrarRanking()
{
    QVector<PlayerRecord> all;
    if (!StatsFile::loadAll(all)) { qDebug() << "No pude cargar ranking"; return; }

    for (int i=1;i<all.size();++i) {
        PlayerRecord key = all[i];
        int j = i-1;
        while (j >= 0 && all[j].avgMs() > key.avgMs()) { all[j+1] = all[j]; --j; }
        all[j+1] = key;
    }

    QString out = "🏆 Ranking por tiempo promedio (Niveles 2-4)\n\n";
    const int N = qMin(10, all.size());
    for (int i=0;i<N;++i) {
        const auto& r = all[i];
        const QString bando = (r.band==0) ? "Empirista" : "Racionalista";
        out += QString("%1) %2  —  %3  —  %4\n")
                   .arg(i+1, 2)
                   .arg(r.name.left(24))
                   .arg(bando, -12)
                   .arg(formatMs(r.avgMs()));
    }
    if (all.isEmpty()) out += "(sin registros aún)";

    if (!rankingRect) {
        rankingRect = new QGraphicsRectItem(0,0, 640, 320);
        rankingRect->setBrush(QColor(10,10,18,220));
        rankingRect->setPen(QPen(Qt::white, 2));
        rankingRect->setZValue(1210);
        addItem(rankingRect);

        rankingText = new QGraphicsTextItem(rankingRect);
        rankingText->setDefaultTextColor(Qt::white);
        rankingText->setFont(QFont("Minecraft", 12, QFont::Bold));
        rankingText->setTextWidth(600);
        rankingText->setPos(20, 20);
    }
    rankingText->setPlainText(out);

    const QPointF c = viewCenter();
    rankingRect->setPos(c.x() - 320.0, c.y() - 160.0);
    rankingRect->setVisible(true);
    startOverlayFollow();

    QTimer::singleShot(20000, this, [this](){ mostrarBotonesFin(); });
}

void TerraceScene::startOverlayFollow()
{
    if (!overlayTimer) {
        overlayTimer = new QTimer(this);
        overlayTimer->setInterval(100);
        connect(overlayTimer, &QTimer::timeout, this, [this](){
            const QPointF c = viewCenter();
            if (felicRect && felicRect->isVisible())
                felicRect->setPos(c.x() - 260.0, c.y() - 60.0);
            if (rankingRect && rankingRect->isVisible())
                rankingRect->setPos(c.x() - 320.0, c.y() - 160.0);
            if (rankingRect && rankingRect->isVisible()) {
                const qreal bx = rankingRect->x() + 640/2.0 - 200;
                const qreal by = rankingRect->y() + 320 + 12;
                if (btnInicioProxy) btnInicioProxy->setPos(bx - 110, by);
                if (btnSalirProxy)  btnSalirProxy ->setPos(bx +  10, by);
            }
        });
    }
    if (!overlayTimer->isActive()) overlayTimer->start();
}

void TerraceScene::mostrarBotonesFin()
{
    if (!btnInicioProxy) {
        btnInicio = new QPushButton("Volver al inicio");
        btnInicio->setMinimumSize(180, 36);
        btnInicioProxy = addWidget(btnInicio);
        btnInicioProxy->setZValue(1220);
        connect(btnInicio, &QPushButton::clicked, this, [this](){
            if (felicRect) felicRect->setVisible(false);
            if (rankingRect) rankingRect->setVisible(false);
            emit volverAlInicio();
        });
    }

    if (!btnSalirProxy) {
        btnSalir = new QPushButton("Salir");
        btnSalir->setMinimumSize(180, 36);
        btnSalirProxy = addWidget(btnSalir);
        btnSalirProxy->setZValue(1220);
        connect(btnSalir, &QPushButton::clicked, this, [](){
            qApp->quit();
        });
    }

    const QPointF c = viewCenter();
    const qreal rx = c.x() - 320.0;
    const qreal ry = c.y() + 160.0;

    if (btnInicioProxy) btnInicioProxy->setPos(rx + 640/2.0 - 200 - 110, ry + 12);
    if (btnSalirProxy)  btnSalirProxy ->setPos(rx + 640/2.0 - 200 +  70, ry + 12);
}

QPointF TerraceScene::viewCenter() const
{
    if (!views().isEmpty()) {
        auto* v = views().first();
        QRect vp = v->viewport()->rect();
        QPoint c = vp.center();
        return v->mapToScene(c);
    }
    return sceneRect().center();
}
