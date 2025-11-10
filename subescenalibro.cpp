#include "subescenalibro.h"
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QScrollBar>
#include <QFont>
#include <QBrush>
#include <QPen>
#include <QTimer>

SubEscenaLibro::SubEscenaLibro(QObject* parent)
    : SceneBase(parent)
{
    fondoPath = "://res/LibreroFondo.png";
    loadScene();
}

void SubEscenaLibro::setBackgroundImage(const QString& resPath)
{
    fondoPath = resPath;
    QPixmap bg(fondoPath);
    if (bg.isNull()) {
        QPixmap solid(sceneRect().size().toSize());
        solid.fill(QColor(25,25,25));
        setBackground(solid);
    } else {
        setBackground(bg.scaled(800,600,
                                Qt::KeepAspectRatioByExpanding,
                                Qt::SmoothTransformation));
    }
}

void SubEscenaLibro::setPreguntas(const LibroPregunta& p1, const LibroPregunta& p2)
{
    libQ1 = p1;
    libQ2 = p2;
    preguntasCargadas = true;
}

void SubEscenaLibro::loadScene()
{
    setSceneRect(0, 0, 800, 600);
    setBackgroundImage(fondoPath);

    hotspotLibro1 = QRectF(305, 150, 90, 120);
    hotspotLibro2 = QRectF(480, 310, 40, 120);

    ensureHoverHint();
    setupInteractions();
}

void SubEscenaLibro::setupInteractions()
{
    clearInteractiveAreas();

    addInteractiveArea(hotspotLibro1, "Click para inspeccionar");
    addInteractiveArea(hotspotLibro2, "Click para inspeccionar");
}

void SubEscenaLibro::ensureHoverHint()
{
    if (!hoverHint) {
        hoverHint = new QGraphicsTextItem();
        hoverHint->setDefaultTextColor(Qt::white);
        hoverHint->setFont(QFont("Minecraft", 12, QFont::Bold));
        hoverHint->setZValue(2000);
        hoverHint->setPlainText("Click para inspeccionar");
        hoverHint->setVisible(false);
        addItem(hoverHint);
    }
}

void SubEscenaLibro::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    const QPointF p = event->scenePos();
    const bool sobreLibro = hotspotLibro1.contains(p) || hotspotLibro2.contains(p);

    if (sobreLibro) {
        ensureHoverHint();
        hoverHint->setPos(p + QPointF(12, -24));
        hoverHint->setVisible(true);
    } else if (hoverHint) {
        hoverHint->setVisible(false);
    }

    SceneBase::mouseMoveEvent(event);
}

void SubEscenaLibro::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    const QPointF p = event->scenePos();

    if (preguntaActiva) { event->accept(); return; }

    if (hotspotLibro1.contains(p) && !librosResueltos.contains(1)) {
        mostrarPreguntaDeLibro(1);
        event->accept();
        return;
    }
    if (hotspotLibro2.contains(p) && !librosResueltos.contains(2)) {
        mostrarPreguntaDeLibro(2);
        event->accept();
        return;
    }

    SceneBase::mousePressEvent(event);
}


void SubEscenaLibro::mostrarPreguntaDeLibro(int cual)
{
    if (!preguntasCargadas) return;
    preguntaActiva = true;
    libroActual = cual;

    if (!panelPregunta) {
        panelPregunta = new QGraphicsRectItem(0,0,720,320);
        panelPregunta->setBrush(QColor(20,20,30,230));
        panelPregunta->setPen(QPen(Qt::white,3));
        panelPregunta->setZValue(1500);
        addItem(panelPregunta);
    }

    if (views().size() > 0) {
        auto* v = views().first();
        QRect vp = v->viewport()->rect();
        QPointF tl = v->mapToScene(vp.topLeft());
        panelPregunta->setPos(tl + QPointF((vp.width()-720)/2, (vp.height()-320)/2));
    } else {
        panelPregunta->setPos(140,340);
    }

    if (!txtEnunciado) {
        txtEnunciado = new QGraphicsTextItem(panelPregunta);
        txtEnunciado->setDefaultTextColor(Qt::white);
        txtEnunciado->setFont(QFont("Minecraft", 12, QFont::Bold));
        txtEnunciado->setTextWidth(680);
        txtEnunciado->setPos(20, 15);
    }

    const LibroPregunta& P = (cual==1 ? libQ1 : libQ2);
    txtEnunciado->setPlainText(P.enunciado);

    for (auto* t : txtOpciones) { if (t) t->deleteLater(); }
    txtOpciones.clear();

    int y = 110;
    for (int i=0;i<4;i++) {
        QString pref = QString("%1) ").arg(QChar('A'+i));
        auto* t = new QGraphicsTextItem(pref + P.opciones[i], panelPregunta);
        t->setDefaultTextColor(Qt::yellow);
        t->setFont(QFont("Minecraft", 12, QFont::Bold));
        t->setPos(40, y);
        txtOpciones << t;
        y += 34;
    }

    panelPregunta->setVisible(true);
}

void SubEscenaLibro::ocultarPregunta()
{
    if (panelPregunta) panelPregunta->setVisible(false);
    preguntaActiva = false;
    libroActual = 0;
}

bool SubEscenaLibro::manejarTecla(int key)
{
    if (!preguntaActiva) return false;

    if (key==Qt::Key_A || key==Qt::Key_B || key==Qt::Key_C || key==Qt::Key_D) {
        QChar letra = QChar(key).toUpper();
        const LibroPregunta& P = (libroActual==1 ? libQ1 : libQ2);
        const bool ok = (letra == P.correcta);
        resolver(ok, P.pista);
        return true;
    }

    if (key==Qt::Key_W || key==Qt::Key_A || key==Qt::Key_S || key==Qt::Key_D) {
        return true;
    }

    return false;
}

void SubEscenaLibro::resolver(bool correcta, const QString& pista)
{
    librosResueltos.insert(libroActual);

    if (correcta && !librosConPista.contains(libroActual)) {
        if (txtEnunciado) txtEnunciado->setPlainText("¡Correcto! Pista obtenida:\n• " + pista);
        emit pistaDesbloqueada(pista);
        librosConPista.insert(libroActual);
    } else {
        if (txtEnunciado && !correcta) {
            txtEnunciado->setPlainText("Incorrecto. Observa mejor el librero y vuelve a intentar.");
        }
    }

    QTimer::singleShot(1200, this, [this](){
        ocultarPregunta();
        if (librosResueltos.size() >= 2) {
            emit subescenaTerminada();
        }
    });
}


void SubEscenaLibro::toggleDebugAreas()
{
    debugOn = !debugOn;
    if (debugOn) drawDebugAreas();
    else         clearDebugAreas();
}

void SubEscenaLibro::drawDebugAreas()
{
    clearDebugAreas();
    auto mk = [&](const QRectF& r, QColor c){
        auto* it = new QGraphicsRectItem(r);
        it->setBrush(QColor(c.red(), c.green(), c.blue(), 80));
        it->setPen(QPen(c, 2));
        it->setZValue(1400);
        addItem(it);
        debugRects << it;
    };
    mk(hotspotLibro1, Qt::red);
    mk(hotspotLibro2, Qt::green);
}

void SubEscenaLibro::clearDebugAreas()
{
    for (auto* r : debugRects) { if (r) { removeItem(r); delete r; } }
    debugRects.clear();
}
