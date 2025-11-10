#include "subescenapizarra.h"
#include <QFont>
#include <QBrush>
#include <QPen>
#include <QTimer>
#include <QGraphicsView>

SubEscenaPizarra::SubEscenaPizarra(QObject* parent)
    : SceneBase(parent)
{
    loadScene();
}

void SubEscenaPizarra::loadScene()
{
    setSceneRect(0, 0, 800, 600);

    setBackgroundImage("://res/PizarraFondoReal.png");

    auto* hint = new QGraphicsTextItem("Responde con A / B / C / D");
    hint->setDefaultTextColor(Qt::white);
    hint->setFont(QFont("Minecraft", 12, QFont::Bold));
    hint->setZValue(1000);
    hint->setPos(20, 560);
    addItem(hint);
}

void SubEscenaPizarra::setBackgroundImage(const QString& resPath)
{
    fondoPath = resPath;

    QPixmap bg(fondoPath);
    if (!bgItem) {
        bgItem = new QGraphicsPixmapItem();
        bgItem->setZValue(-1000);
        addItem(bgItem);
    }

    if (bg.isNull()) {
        QPixmap solid(sceneRect().size().toSize());
        solid.fill(QColor(20,20,25));
        bgItem->setPixmap(solid);
        bgItem->setPos(0,0);
        return;
    }

    bgItem->setPixmap(bg);
    layoutBackground();
}

void SubEscenaPizarra::layoutBackground()
{
    if (!bgItem || bgItem->pixmap().isNull()) return;

    const QSizeF sceneSz = sceneRect().size();
    const QSize   imgSz  = bgItem->pixmap().size();

    const qreal sx = sceneSz.width()  / imgSz.width();
    const qreal sy = sceneSz.height() / imgSz.height();
    const qreal s  = qMin(sx, sy);

    bgItem->setTransform(QTransform::fromScale(s, s));

    const qreal w = imgSz.width()  * s;
    const qreal h = imgSz.height() * s;
    const qreal x = (sceneSz.width()  - w) / 2.0;
    const qreal y = (sceneSz.height() - h) / 2.0;
    bgItem->setPos(x, y);
}

bool SubEscenaPizarra::manejarTecla(int key)
{
    if (respondida) return true;

    if (key==Qt::Key_A || key==Qt::Key_B || key==Qt::Key_C || key==Qt::Key_D) {
        resolver(QChar(key).toUpper().toLatin1());
        return true;
    }
    if (key==Qt::Key_W || key==Qt::Key_A || key==Qt::Key_S || key==Qt::Key_D) {
        return true;
    }
    return false;
}

void SubEscenaPizarra::showFeedback(const QString& msg)
{
    if (!feedbackPanel) {
        feedbackPanel = new QGraphicsRectItem(0,0, 680, 140);
        feedbackPanel->setBrush(QColor(100, 40, 180, 230)); // morado
        feedbackPanel->setPen(QPen(Qt::white, 3));
        feedbackPanel->setZValue(1500);
        addItem(feedbackPanel);

        feedbackText = new QGraphicsTextItem(feedbackPanel);
        feedbackText->setDefaultTextColor(Qt::white);
        feedbackText->setFont(QFont("Minecraft", 12, QFont::Bold));
        feedbackText->setTextWidth(640);
        feedbackText->setPos(20, 15);
    }

    feedbackText->setPlainText(msg);

    if (!views().isEmpty()) {
        auto* v = views().first();
        const QRect vp = v->viewport()->rect();
        const QPointF tl = v->mapToScene(vp.topLeft());
        const QPointF pos = tl + QPointF((vp.width()-680)/2.0, (vp.height()-140)/2.0);
        feedbackPanel->setPos(pos);
    } else {
        feedbackPanel->setPos(60, 230);
    }

    feedbackPanel->setVisible(true);
}

void SubEscenaPizarra::resolver(char opcion)
{
    if (respondida) return;
    respondida = true;

    const bool correcta = (opcion == 'B');

    if (correcta && !pistaOtorgada) {
        emit pistaDesbloqueada("La **Crítica de la razón pura** fija el marco del conocer.");
        pistaOtorgada = true;
        showFeedback("¡Correcto! Pista obtenida:\n• La persona que le robo a Kant ama callar gente.");
    } else {
        showFeedback("Incorrecto. No obtuviste pista.");
    }

    QTimer::singleShot(1100, this, [this](){
        if (feedbackPanel) feedbackPanel->setVisible(false);
        emit subescenaTerminada();
    });
}
