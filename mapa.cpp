#include "mapa.h"
#include <QPainter>
#include <QGraphicsTextItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsDropShadowEffect>
#include <QCursor>

// ---------------- LevelNode ----------------
LevelNode::LevelNode(const QString& name, const QString& label, const QPointF& pos,
                     const QString& iconRes, QGraphicsItem* parent)
    : QGraphicsObject(parent), id(name), label(label)
{
    icon.load(iconRes);
    setPos(pos);
    setAcceptedMouseButtons(Qt::LeftButton);
    setCursor(QCursor(Qt::PointingHandCursor));   // <-- FIX
    setZValue(20);

    auto* fx = new QGraphicsDropShadowEffect;
    fx->setBlurRadius(30);
    fx->setColor(QColor(0,0,0,180));
    fx->setOffset(0,4);
    setGraphicsEffect(fx);
}

void LevelNode::paint(QPainter* p, const QStyleOptionGraphicsItem*, QWidget*)
{
    p->setRenderHint(QPainter::Antialiasing, true);

    // glow exterior
    QRadialGradient g1(QPointF(0,0), 86);
    g1.setColorAt(0.0, QColor(255, 0, 120, locked?80:140));
    g1.setColorAt(1.0, QColor(120, 0, 60, 0));
    p->setBrush(g1);
    p->setPen(Qt::NoPen);
    p->drawEllipse(QPointF(0,0), 86, 86);

    // glow interior
    QRadialGradient g2(QPointF(0,0), 64);
    g2.setColorAt(0.0, QColor(255, 0, 120, locked?110:180));
    g2.setColorAt(1.0, QColor(120, 0, 60, 0));
    p->setBrush(g2);
    p->drawEllipse(QPointF(0,0), 64, 64);

    // base
    p->setBrush(QColor(15, 10, 18, 230));
    p->setPen(QPen(QColor(40,30,45), 3));
    p->drawEllipse(QPointF(0,0), 58, 58);

    // icono
    if (!icon.isNull()) {
        const int s = 58;
        QPixmap scaled = icon.scaled(QSize(s,s), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        p->drawPixmap(-scaled.width()/2, -scaled.height()/2, scaled);
    }

    // etiqueta
    QRectF r(-50, 52, 100, 26);
    p->setBrush(QColor(25, 20, 25, 220));
    p->setPen(QPen(QColor(30,30,35), 2));
    p->drawRoundedRect(r, 12, 12);

    QFont f("Minecraft", 11, QFont::Bold);
    p->setFont(f);
    p->setPen(QPen(Qt::white));
    p->drawText(r, Qt::AlignCenter, label);

    if (locked) {
        p->setBrush(QColor(0,0,0,120));
        p->setPen(Qt::NoPen);
        p->drawEllipse(QPointF(0,0), 58, 58);
        p->setPen(QPen(Qt::white, 3));
        p->drawRect(-12,-6,24,20);
        p->drawArc(-12,-22,24,24, 30*16, 120*16);
    }
}

void LevelNode::mousePressEvent(QGraphicsSceneMouseEvent* ev)
{
    if (!locked && ev->button()==Qt::LeftButton) emit clicked(id);
    QGraphicsObject::mousePressEvent(ev);
}

// ---------------- Mapa ----------------
Mapa::Mapa(QObject* parent)
    : SceneBase(parent), grafo(4)
{
    loadScene();
}

void Mapa::loadScene()
{
    // Tamaño fijo solicitado
    setSceneRect(0, 0, 800, 600);

    buildBackground();
    buildTitle();
    buildGraph();
    layoutNodes();
    drawEdges();
    refreshLocks();
}


void Mapa::buildBackground()
{
    QPixmap bg("://res/castle.jpg");  // ajusta ruta a tu recurso
    if (bg.isNull()){
        QPixmap solid(sceneRect().size().toSize());
        solid.fill(QColor(22,14,28));
        setBackground(solid);
    } else {
        setBackground(bg.scaled(sceneRect().size().toSize(),
                                Qt::KeepAspectRatioByExpanding,
                                Qt::SmoothTransformation));
    }
}

void Mapa::buildTitle()
{
    auto* t = new QGraphicsTextItem("MAPA");
    t->setDefaultTextColor(Qt::white);
    t->setFont(QFont("Minecraft", 54, QFont::Black));
    t->setZValue(5);
    addItem(t);

    // Centrado horizontal según el tamaño actual de la escena (800x600)
    const QRectF r = sceneRect();
    t->setPos(r.center().x() - t->boundingRect().width() / 2.0,
              r.top() + 40);
}


void Mapa::buildGraph()
{
    grafo.agregarVertice("L1");
    grafo.agregarVertice("L2");
    grafo.agregarVertice("L3");
    grafo.agregarVertice("L4");

    grafo.agregarArco("L1","L2",1);
    grafo.agregarArco("L2","L3",1);
    grafo.agregarArco("L3","L4",1);

    unlocked = QSet<QString>() << "L1";
}

void Mapa::layoutNodes()
{
    const QRectF r = sceneRect();
    const qreal W = r.width();    // 800
    const qreal H = r.height();   // 600
    const qreal offsetX = -110.0; // mover a la izquierda
    const qreal offsetY = -70.0;  // mover hacia arriba

    struct P { qreal x, y; };
    QMap<QString, P> pos = {
        {"L1", {0.275 * W, 0.550 * H}},
        {"L2", {0.488 * W, 0.867 * H}},
        {"L3", {0.725 * W, 0.867 * H}},
        {"L4", {0.950 * W, 0.550 * H}}
    };

    auto add = [&](const QString& id, const QString& label, const QString& icon){
        LevelNode* n = new LevelNode(
            id, label,
            QPointF(pos[id].x + offsetX, pos[id].y + offsetY),
            icon
            );
        addItem(n);
        nodes[id] = n;
        connect(n, &LevelNode::clicked, this, &Mapa::levelChosen);
    };

    add("L1","Nivel 1", "://res/Objects/casette.png");
    add("L2","Nivel 2", "://res/Objects/foco.png");
    add("L3","Nivel 3", "://res/Objects/skull.png");
    add("L4","Nivel 4", "://res/Objects/books.png");
}


void Mapa::drawEdges()
{
    QPen pen(QColor(255,0,120,90), 4, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);

    auto drawArrow = [&](const QString& a, const QString& b){
        if(!nodes.contains(a) || !nodes.contains(b)) return;
        QPointF A = nodes[a]->pos();
        QPointF B = nodes[b]->pos();
        auto* line = addLine(QLineF(A,B), pen);
        line->setZValue(10);
    };

    if (grafo.sonAdyacentes("L1","L2")) drawArrow("L1","L2");
    if (grafo.sonAdyacentes("L2","L3")) drawArrow("L2","L3");
    if (grafo.sonAdyacentes("L3","L4")) drawArrow("L3","L4");
}

void Mapa::refreshLocks()
{
    for (auto it = nodes.begin(); it != nodes.end(); ++it){
        it.value()->setLocked(!unlocked.contains(it.key()));
    }
}

void Mapa::setUnlocked(const QSet<QString>& set)
{
    unlocked = set;
    refreshLocks();
}

void Mapa::unlock(const QString& name)
{
    unlocked.insert(name);
    refreshLocks();
}

void Mapa::unlockVecinos(const QString& name)
{
    const auto& adys = grafo.adyacentesDe(name.toStdString());
    for (auto it = adys.begin(); it != adys.end(); ++it) {
        const Vertice& v = grafo.getVertice(it->first);
        unlocked.insert(QString::fromStdString(v.getNombre()));
    }
    refreshLocks();
}

