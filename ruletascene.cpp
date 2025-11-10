#include "ruletascene.h"

#include <QDebug>
#include <QKeyEvent>
#include <QRandomGenerator>
#include <QMessageBox>
#include <QGraphicsTextItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsScene>
#include <QGraphicsPolygonItem>
#include <QPen>
#include <QBrush>
#include <QGraphicsItemGroup>
#include <QGraphicsLineItem>
#include <QPainterPath>
#include <QGraphicsSceneMouseEvent>
#include <QtMath>
#include <algorithm>

RuletaScene::RuletaScene(QObject *parent)
    : SceneBase(parent)
    , animacionTimer(new QTimer(this))
{
    seccionesUsadas = QVector<bool>(4, false);
    textosRuleta.resize(4);
    tachadosItems.resize(4);

    puertaAbiertaParaJugar  = QVector<bool>(4, false);
    seccionCompletada       = QVector<bool>(4, false);

    seccionSeleccionada = -1;
    seccionEnJuego = -1;

    loadScene();
}


void RuletaScene::loadScene()
{
    QPixmap bgPixmap("://res/ruletaEscenario.png");
    setBackground(bgPixmap.scaled(1000, 1000, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));

    setupCollisions();
    setupInteractiveObjects();
    setupInteractions();

    connect(animacionTimer, &QTimer::timeout, this, &RuletaScene::actualizarAnimacionRuleta);
}

void RuletaScene::setSeccionEnJuego(int idx)
{
    seccionEnJuego = (idx >= 0 && idx < 4) ? idx : -1;
}

void RuletaScene::setupCollisions()
{
    addCollisionArea(QRectF(30, 425, 885, -425));
    addCollisionArea(QRectF(760,530,100,-130));
    addCollisionArea(QRectF(0,700,1000,10));
}

void RuletaScene::setupInteractions()
{
    clearInteractiveAreas();

    addInteractiveArea(QRectF(745, 380, 160, 170), "Presiona E para girar");

    int x = 30;
    int y = 310;

    QStringList mensajesPuertas ={
        "Presiona E para entrar a la seccion de Arte",
        "Presiona E para entrar a la seccion de Ciencia",
        "Presiona E para entrar a la seccion de Historia",
        "Presiona E para entrar a la seccion de Politica",
    };

    for (int i = 0; i < 4; ++i) {
        if (puertaAbiertaParaJugar[i]) {
            addInteractiveArea(QRectF(x, y, 130, 170), mensajesPuertas[i]);
        }
        x += 180;
    }
}

void RuletaScene::agregarJuegoGanado()   { registrarResultadoJuego(true); }
void RuletaScene::agregarJuegoPerdido()  { registrarResultadoJuego(false); }

void RuletaScene::setupInteractiveObjects()
{
    int x = 30;
    int y = 310;

    for(int i = 0; i < 4; i++) {
        QPixmap closedDoorPixmap("://res/Objects/closedDoorColored.png");
        QGraphicsPixmapItem *closedDoor = new QGraphicsPixmapItem(closedDoorPixmap.scaled(130, 200));
        closedDoor->setPos(x, y);
        addObject(closedDoor);
        puertasItems.append(closedDoor);

        QPixmap openDoorPixmap("://res/Objects/openDoorColored.png");
        QGraphicsPixmapItem *openDoor = new QGraphicsPixmapItem(openDoorPixmap.scaled(130, 200));
        openDoor->setPos(x, y);
        openDoor->setVisible(false);
        openDoor->setZValue(5);
        addObject(openDoor);
        puertasAbiertasItems.append(openDoor);
        x += 180;
    }

    if (!hudHeartsGroup) {
        hudHeartsGroup = new QGraphicsItemGroup();
        hudHeartsGroup->setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
        hudHeartsGroup->setZValue(9999);
        addObject(hudHeartsGroup);
    }

    for (int i = 0; i < totalJuegos; ++i) {
        auto* emptyHeart = new QGraphicsPixmapItem(QPixmap("://res/Objects/emptyHeart.png").scaled(30,30));
        emptyHeart->setPos(0 + i*40, 500);
        emptyHeart->setParentItem(hudHeartsGroup);
        corazonesVacios.append(emptyHeart);

        auto* heart = new QGraphicsPixmapItem(QPixmap("://res/Objects/fullHeart.png").scaled(30,30));
        heart->setPos(0 + i*40, 500);
        heart->setVisible(false);
        heart->setZValue(10000);
        heart->setParentItem(hudHeartsGroup);
        corazones.append(heart);
    }

    QString secciones[] = {"Arte","Ciencia","Historia","Politica"};
    int xDoor = 65;
    int yDoor = 290;
    for(int i = 0; i < 4; i++){
        QGraphicsSimpleTextItem* doorText = new QGraphicsSimpleTextItem(secciones[i]);
        QFont font("Minecraft", 18, QFont::Bold);
        doorText->setFont(font);
        doorText->setBrush(Qt::white);
        doorText->setPos(xDoor,yDoor);
        doorText->setZValue(20);
        addObject(doorText);
        textosRuleta[i] = doorText;
        xDoor += 170;
    }

    ruletaGroup = new QGraphicsItemGroup();

    QPainterPath ruletaPath;
    ruletaPath.addEllipse(0, 0, 160, 160);
    ruleta = new QGraphicsPathItem(ruletaPath);
    ruleta->setBrush(QBrush(Qt::white));
    ruleta->setPen(QPen(Qt::black, 3));
    ruletaGroup->addToGroup(ruleta);

    QStringList colores = {"#6C0018", "#B45D07", "#6E2B1E", "#211A4E"};
    QStringList nombres = {"ARTE", "CIENCIA", "HISTORIA", "POLÍTICA"};

    for(int i = 0; i < 4; i++) {
        QPainterPath sectorPath;
        sectorPath.moveTo(80, 80);

        double startAngle = i * 90.0;
        double sweepLength = 90.0;

        sectorPath.arcTo(0, 0, 160, 160, startAngle, sweepLength);
        sectorPath.lineTo(80, 80);
        sectorPath.closeSubpath();

        QGraphicsPathItem *seccion = new QGraphicsPathItem(sectorPath);
        seccion->setBrush(QBrush(QColor(colores[i])));
        seccion->setPen(QPen(Qt::black, 1));
        ruletaGroup->addToGroup(seccion);

        QGraphicsSimpleTextItem *texto = new QGraphicsSimpleTextItem(nombres[i]);
        QFont font("Minecraft", 8, QFont::Bold);
        texto->setFont(font);
        texto->setBrush(QBrush(Qt::white));

        double anguloMedio = (i * 90 + 45) * M_PI / 180.0;
        double radio = 55;
        double textoX = 80 + radio * qCos(anguloMedio);
        double textoY = 80 + radio * qSin(anguloMedio);

        QRectF bounds = texto->boundingRect();
        texto->setPos(textoX - bounds.width()/2, textoY - bounds.height()/2);
        texto->setTransformOriginPoint(bounds.width()/2, bounds.height()/2);
        texto->setRotation(i * 90 + 45);
        texto->setZValue(10);

        ruletaGroup->addToGroup(texto);

        QGraphicsLineItem *linea = new QGraphicsLineItem(textoX - 25, textoY, textoX + 25, textoY);
        linea->setPen(QPen(Qt::black, 3, Qt::SolidLine, Qt::RoundCap));
        linea->setVisible(false);
        linea->setZValue(11);
        tachadosItems[i] = linea;
        ruletaGroup->addToGroup(linea);
    }

    ruletaGroup->setTransformOriginPoint(80, 80);
    addObject(ruletaGroup);
    ruletaGroup->setPos(770, 380);
    ruletaGroup->setZValue(100);

    QPolygonF flechaPolygon;
    flechaPolygon << QPointF(20, 0) << QPointF(0, 10) << QPointF(20, 20);
    ruletaArrow = new QGraphicsPolygonItem(flechaPolygon);
    ruletaArrow->setPos(770 + 160, 380 + 70);
    ruletaArrow->setBrush(QBrush(QColor(175, 0, 24)));
    ruletaArrow->setPen(QPen(Qt::black, 2));
    ruletaArrow->setZValue(101);
    addObject(ruletaArrow);
}

void RuletaScene::girarRuleta()
{
    if(ruletaGirando) return;

    bool todasUsadas = std::all_of(seccionesUsadas.begin(), seccionesUsadas.end(),
                                   [](bool usada){ return usada; });
    if(todasUsadas) {
        QMessageBox::information(nullptr, "Ruleta", "¡Todas las secciones ya se han utilizado!");
        return;
    }

    ruletaGirando = true;

    QList<int> seccionesDisponibles;
    for(int i = 0; i < 4; i++) {
        if(!seccionesUsadas[i]) {
            seccionesDisponibles.append(i);
        }
    }

    int indiceAleatorio = QRandomGenerator::global()->bounded(seccionesDisponibles.size());
    seccionSeleccionada = seccionesDisponibles[indiceAleatorio];

    double anguloObjetivo = -(seccionSeleccionada * 90.0 + 45.0);

    int vueltas = QRandomGenerator::global()->bounded(2, 5);
    anguloFinal = anguloObjetivo - (360.0 * vueltas);

    animacionTimer->start(30);
}

void RuletaScene::actualizarAnimacionRuleta()
{
    double diff = anguloFinal - anguloRotacion;

    double vel = diff * 0.08;
    if(qFabs(vel) < 0.3) vel = (vel >= 0 ? 0.3 : -0.3);

    anguloRotacion += vel;
    ruletaGroup->setRotation(anguloRotacion);

    if(qFabs(diff) < 0.5) {
        ruletaGroup->setRotation(anguloFinal);
        animacionTimer->stop();
        ruletaGirando = false;
        procesarSeleccion(seccionSeleccionada);
    }
}

void RuletaScene::detenerRuleta()
{
    animacionTimer->stop();
    ruletaGirando = false;

    QList<int> seccionesDisponibles;
    for(int i = 0; i < 4; i++) {
        if(!seccionesUsadas[i]) seccionesDisponibles.append(i);
    }
    if(seccionesDisponibles.isEmpty()) {
        QMessageBox::information(nullptr, "Ruleta", "¡Todas las secciones ya se han utilizado!");
        return;
    }

    int indiceAleatorio = QRandomGenerator::global()->bounded(seccionesDisponibles.size());
    seccionSeleccionada = seccionesDisponibles[indiceAleatorio];

    double anguloPorSeccion = 90.0;
    double anguloDeseado = -(seccionSeleccionada * anguloPorSeccion + anguloPorSeccion/2);
    ruletaGroup->setRotation(anguloDeseado);

    procesarSeleccion(seccionSeleccionada);
}

void RuletaScene::procesarSeleccion(int seccion)
{
    if (seccion < 0 || seccion >= 4) return;

    marcarSeccionUsada(seccion);

    abrirPuerta(seccion);

    setupInteractions();

    static const QStringList nombres = {"ARTE", "CIENCIA", "HISTORIA", "POLÍTICA"};
    QMessageBox::information(nullptr, "Selección", "Seleccionada: " + nombres[seccion]);
}

void RuletaScene::marcarSeccionUsada(int seccion)
{
    if(seccion < 0 || seccion >= 4) return;

    seccionesUsadas[seccion] = true;

    if(seccion < tachadosItems.size() && tachadosItems[seccion]) {
        tachadosItems[seccion]->setVisible(true);
    }

    if(seccion < textosRuleta.size() && textosRuleta[seccion]) {
        textosRuleta[seccion]->setBrush(QBrush(QColor(100, 100, 100)));
        textosRuleta[seccion]->setOpacity(0.5);
    }
}

void RuletaScene::abrirPuerta(int seccion)
{
    if (seccion < 0 || seccion >= puertasItems.size() || seccion >= puertasAbiertasItems.size())
        return;

    puertaAbiertaParaJugar[seccion] = true;

    puertasItems[seccion]->setVisible(false);
    puertasAbiertasItems[seccion]->setVisible(true);
}

void RuletaScene::cerrarPuerta(int seccion)
{
    if (seccion < 0 || seccion >= puertasItems.size() || seccion >= puertasAbiertasItems.size())
        return;

    puertaAbiertaParaJugar[seccion] = false;
    seccionCompletada[seccion] = true;

    puertasAbiertasItems[seccion]->setVisible(false);
    puertasItems[seccion]->setVisible(true);

    if (seccion < tachadosItems.size() && tachadosItems[seccion]) {
        tachadosItems[seccion]->setVisible(true);
    }
    if (seccion < textosRuleta.size() && textosRuleta[seccion]) {
        textosRuleta[seccion]->setBrush(QBrush(QColor(100, 100, 100)));
        textosRuleta[seccion]->setOpacity(0.5);
    }
}

void RuletaScene::actualizarCorazones()
{
    for(int i = 0; i < totalJuegos; i++){
        if(i < juegosGanados){
            corazones[i]->setVisible(true);
            corazonesVacios[i]->setVisible(false);
        } else {
            corazones[i]->setVisible(false);
            corazonesVacios[i]->setVisible(true);
        }
    }
}

void RuletaScene::cambiarCorazonLleno(int seccion)
{
    if(seccion >= 0 && seccion < corazones.size() && seccion < corazonesVacios.size()){
        corazonesVacios[seccion]->setVisible(false);
        corazones[seccion]->setVisible(true);
    }
}

void RuletaScene::mostrarSeleccionBando()
{
    mostrarBandos = true;

    QGraphicsRectItem* fondoBando = new QGraphicsRectItem(0, 0, 500, 400);
    fondoBando->setBrush(QBrush(QColor(70, 23, 180, 230)));
    fondoBando->setPen(QPen(Qt::white, 3));
    fondoBando->setPos(200, 350);
    fondoBando->setZValue(1099);
    addObject(fondoBando);

    textoSeleccionBando = new QGraphicsTextItem("Elige tu bando filosófico:");
    textoSeleccionBando->setDefaultTextColor(Qt::white);
    textoSeleccionBando->setFont(QFont("Minecraft", 16, QFont::Bold));
    textoSeleccionBando->setPos(300, 400);
    textoSeleccionBando->setZValue(1100);
    addObject(textoSeleccionBando);

    QPixmap empiristaPixmap("://res/Objects/badgeRed.png");
    badgeEmpirista = new QGraphicsPixmapItem(empiristaPixmap.scaled(120, 160));
    badgeEmpirista->setPos(300, 450);
    badgeEmpirista->setZValue(1101);
    badgeEmpirista->setData(0, "empirista");
    addObject(badgeEmpirista);

    QGraphicsTextItem* textoEmpirista = new QGraphicsTextItem("Empirista");
    textoEmpirista->setDefaultTextColor(Qt::yellow);
    textoEmpirista->setFont(QFont("Minecraft", 12, QFont::Bold));
    textoEmpirista->setPos(310, 670);
    textoEmpirista->setZValue(1101);
    addObject(textoEmpirista);

    QPixmap racionalistaPixmap("://res/Objects/whiteBadge.png");
    badgeRacionalista = new QGraphicsPixmapItem(racionalistaPixmap.scaled(120, 160));
    badgeRacionalista->setPos(480, 450);
    badgeRacionalista->setZValue(1101);
    badgeRacionalista->setData(0, "racionalista");
    addObject(badgeRacionalista);

    QGraphicsTextItem* textoRacionalista = new QGraphicsTextItem("Racionalista");
    textoRacionalista->setDefaultTextColor(Qt::cyan);
    textoRacionalista->setFont(QFont("Minecraft", 12, QFont::Bold));
    textoRacionalista->setPos(475, 670);
    textoRacionalista->setZValue(1101);
    addObject(textoRacionalista);

    QGraphicsTextItem* instruccion = new QGraphicsTextItem("Haz click en el badge de tu elección");
    instruccion->setDefaultTextColor(Qt::white);
    instruccion->setFont(QFont("Minecraft", 12, QFont::Bold));
    instruccion->setPos(280, 630);
    instruccion->setZValue(1101);
    addObject(instruccion);
}

void RuletaScene::seleccionarBando(bool esEmpirista)
{
    if (!mostrarBandos) return;

    QString bando = esEmpirista ? "EMPIRISTA" : "RACIONALISTA";
    limpiarSeleccionBando();

    QGraphicsTextItem* confirmacion = new QGraphicsTextItem("¡Has elegido el bando " + bando + "!");
    confirmacion->setDefaultTextColor(Qt::white);
    confirmacion->setFont(QFont("Minecraft", 14, QFont::Bold));
    confirmacion->setZValue(1200);
    confirmacion->setPos(300, 330);
    addObject(confirmacion);

    QTimer::singleShot(2000, this, [this, esEmpirista, confirmacion]() {
        if (confirmacion) { removeItem(confirmacion); delete confirmacion; }
        int vidaInicial = vidaInicialCalculada();
        emit irAlMapaTrasBando(esEmpirista, vidaInicial);

    });
}

void RuletaScene::limpiarSeleccionBando()
{
    QList<QGraphicsItem*> itemsList = this->items();
    for (QGraphicsItem* item : itemsList) {
        if (item->zValue() == 150 || item->zValue() == 1099 || item->zValue() == 1100 || item->zValue() == 1101) {
            removeItem(item);
            delete item;
        }
    }

    mostrarBandos = false;
    badgeEmpirista = nullptr;
    badgeRacionalista = nullptr;
    textoSeleccionBando = nullptr;
}

void RuletaScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (!mostrarBandos) {
        QGraphicsScene::mousePressEvent(event);
        return;
    }

    QGraphicsItem* itemClicked = itemAt(event->scenePos(), QTransform());
    if (!itemClicked) {
        QGraphicsScene::mousePressEvent(event);
        return;
    }

    if (itemClicked == badgeEmpirista || itemClicked->parentItem() == badgeEmpirista) {
        seleccionarBando(true);
    }
    else if (itemClicked == badgeRacionalista || itemClicked->parentItem() == badgeRacionalista) {
        seleccionarBando(false);
    }

    QGraphicsScene::mousePressEvent(event);
}


void RuletaScene::confirmarBandoYVolverAlMapa()
{
    if (!puedeConfirmar) return;
    const bool esEmpirista = bandoElegidoEsEmpirista;
    const int vidaInicial = vidaInicialCalculada();
    emit irAlMapaTrasBando(esEmpirista, vidaInicial);

    puedeConfirmar = false;
}

void RuletaScene::keyPressEvent(QKeyEvent* event)
{
    if ((event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter || event->key() == Qt::Key_Space)
        && puedeConfirmar) {
        confirmarBandoYVolverAlMapa();
        return;
    }

    QGraphicsScene::keyPressEvent(event);
}
void RuletaScene::debugVisualRuleta()
{
    QList<QGraphicsItem*> hijos = ruletaGroup->childItems();
    for (auto *item : hijos) {
        if (item->data(0).toString() == "debugOverlay") {
            delete item;
        }
    }

    for (int i = 0; i < 4; ++i) {
        double angulo = i * 90.0;
        double rad = angulo * M_PI / 180.0;
        double x1 = 80 + 80 * qCos(rad);
        double y1 = 80 + 80 * qSin(rad);

        QGraphicsLineItem *linea = new QGraphicsLineItem(80, 80, x1, y1);
        linea->setPen(QPen(Qt::red, 2, Qt::DashLine));
        linea->setParentItem(ruletaGroup);
        linea->setData(0, "debugOverlay");
    }

    QGraphicsSimpleTextItem *texto = new QGraphicsSimpleTextItem();
    texto->setText(QString("Ángulo actual: %1°").arg(fmod(anguloRotacion, 360), 0, 'f', 1));
    texto->setBrush(Qt::blue);
    texto->setZValue(200);
    texto->setPos(770, 600);
    texto->setData(0, "debugOverlay");
    texto->setVisible(true);
    addObject(texto);
}

void RuletaScene::debugAbrirTodasLasPuertas()
{
    for(int i = 0; i < 4; i++) {
        if(i < puertasItems.size() && i < puertasAbiertasItems.size()) {
            puertasItems[i]->setVisible(false);
            puertasAbiertasItems[i]->setVisible(true);
        }
        seccionesUsadas[i] = true;
        if (i < tachadosItems.size() && tachadosItems[i]) tachadosItems[i]->setVisible(true);
        if (i < textosRuleta.size() && textosRuleta[i]) {
            textosRuleta[i]->setBrush(QBrush(QColor(100, 100, 100)));
            textosRuleta[i]->setOpacity(0.5);
        }
    }
    setupInteractions();
}

void RuletaScene::registrarResultadoJuego(bool gano)
{
    ++juegosJugados;
    if (gano) {
        ++juegosGanados;
        actualizarCorazones();
    }

    int paraCerrar = seccionEnJuego;
    if (paraCerrar < 0 || paraCerrar > 3) {
        paraCerrar = seccionSeleccionada;
    }
    if (paraCerrar >= 0 && paraCerrar < 4) {
        cerrarPuerta(paraCerrar);
    }
    seccionEnJuego = -1;

    setupInteractions();

    if (juegosJugados >= totalJuegos) {
        if (juegosGanados >= 1) {
            QTimer::singleShot(500, this, &RuletaScene::mostrarSeleccionBando);
        } else {
            reiniciarRuletaYPuertas();
            juegosJugados = 0;
            juegosGanados = 0;
            actualizarCorazones();
        }
    }
}


void RuletaScene::initHud(QGraphicsView* view)
{
    attachedView = view;
    if (!attachedView) return;

    connect(attachedView->horizontalScrollBar(), &QScrollBar::valueChanged,
            this, &RuletaScene::updateHudPos);
    connect(attachedView->verticalScrollBar(), &QScrollBar::valueChanged,
            this, &RuletaScene::updateHudPos);

    updateHudPos();
}

void RuletaScene::updateHudPos()
{
    if (!attachedView || !hudHeartsGroup) return;

    const QRect vp = attachedView->viewport()->rect();
    const QPointF topLeftScene = attachedView->mapToScene(vp.topLeft());
    hudHeartsGroup->setPos(topLeftScene + QPointF(80, 20));
}

void RuletaScene::reiniciarRuletaYPuertas()
{
    for (int i = 0; i < 4; ++i) {
        seccionesUsadas[i]         = false;
        puertaAbiertaParaJugar[i]  = false;
        seccionCompletada[i]       = false;
    }

    for (int i = 0; i < 4; ++i) {
        if (i < puertasItems.size() && i < puertasAbiertasItems.size()) {
            puertasAbiertasItems[i]->setVisible(false);
            puertasItems[i]->setVisible(true);
        }
        if (i < tachadosItems.size() && tachadosItems[i]) {
            tachadosItems[i]->setVisible(false);
        }
        if (i < textosRuleta.size() && textosRuleta[i]) {
            textosRuleta[i]->setBrush(QBrush(Qt::white));
            textosRuleta[i]->setOpacity(1.0);
        }
    }

    seccionSeleccionada = -1;
    ruletaGirando = false;
    anguloRotacion = 0;
    anguloFinal = 0;
    if (ruletaGroup) ruletaGroup->setRotation(0);

    setupInteractions();
}


void RuletaScene::debugGanarTodoYElegirBando()
{
    for (int i = 0; i < 4; ++i) {
        seccionesUsadas[i] = true;
        puertaAbiertaParaJugar[i] = false;
        seccionCompletada[i] = true;

        if (i < puertasItems.size() && i < puertasAbiertasItems.size()) {
            puertasAbiertasItems[i]->setVisible(false);
            puertasItems[i]->setVisible(true);
        }
        if (i < tachadosItems.size() && tachadosItems[i]) tachadosItems[i]->setVisible(true);
        if (i < textosRuleta.size() && textosRuleta[i]) {
            textosRuleta[i]->setBrush(QBrush(QColor(100,100,100)));
            textosRuleta[i]->setOpacity(0.5);
        }
    }

    juegosJugados = totalJuegos;
    juegosGanados = totalJuegos;
    actualizarCorazones();

    QTimer::singleShot(0, this, &RuletaScene::mostrarSeleccionBando);
}

void RuletaScene::debugPerderTodoYReiniciar()
{
    juegosJugados = totalJuegos;
    juegosGanados = 0;
    actualizarCorazones();

    reiniciarRuletaYPuertas();

    juegosJugados = 0;
    juegosGanados = 0;
    actualizarCorazones();
}
