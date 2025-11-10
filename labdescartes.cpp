#include "labdescartes.h"

#include <QGraphicsView>
#include <QGraphicsItemGroup>
#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QTimer>
#include <QtMath>
#include <QScrollBar>
#include <QDebug>


LabDescartes::LabDescartes(QObject* parent)
    : SceneBase(parent)
{
    loadScene();
}


void LabDescartes::loadScene()
{
    setSceneRect(0,0,1000,1000);

    QPixmap bg("://res/ruletaEscenario.png");
    if (bg.isNull()) {
        QPixmap solid(sceneRect().size().toSize());
        solid.fill(QColor(40,25,40));
        setBackground(solid);
    } else {
        setBackground(bg.scaled(sceneRect().size().toSize(),
                                Qt::KeepAspectRatioByExpanding,
                                Qt::SmoothTransformation));
    }

    colocarProps();
    setupCollisions();
    setupInteractions();
}

void LabDescartes::colocarProps()
{
    auto addProp = [&](const QString& path, QPointF pos, QSize size, qreal z){
        QPixmap px(path);
        if (px.isNull()) return (QGraphicsPixmapItem*)nullptr;
        auto* it = new QGraphicsPixmapItem(px.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        it->setPos(pos); it->setZValue(z);
        addItem(it);
        return it;
    };

    lamparaL = addProp("://res/Objects/lamp.png", QPointF(300,240), QSize(100,120), 6);
    lamparaR = addProp("://res/Objects/lamp.png", QPointF(670,240), QSize(100,120), 6);

    poster1 = addProp("://res/Objects/poster1.png", QPointF(700,300),QSize(166,166),6);
    poster2 = addProp("://res/Objects/poster2.png", QPointF(225,300), QSize(166,166),6);

    mesaEvidencia   = addProp("://res/Objects/desk.png", QPointF(250,500), QSize(120,85), 6);
    mesaAnalisis    = addProp("://res/Objects/desk.png", QPointF(700,500), QSize(120,85), 6);
    mesaSintesis    = addProp("://res/Objects/desk.png", QPointF(250,600), QSize(120,85), 6);
    mesaEnumeracion = addProp("://res/Objects/desk.png", QPointF(700,600), QSize(120,85), 6);

    batFrames.clear();
    auto load = [&](const QString& p){
        QPixmap px(p);
        if (!px.isNull()) batFrames << px.scaled(batTargetSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    };
    load("://res/Objects/murcielago1.png");
    load("://res/Objects/murcielago2.png");
    load("://res/Objects/murcielago3.png");

    if (batFrames.isEmpty()) {
        QPixmap ph(batTargetSize); ph.fill(Qt::magenta);
        batFrames << ph << ph << ph;
    }

    descartesItem = new QGraphicsPixmapItem(batFrames.first());
    descartesItem->setPos(batBasePos);
    descartesItem->setZValue(6);
    addItem(descartesItem);

    startBatAnimation();
}

void LabDescartes::setupCollisions()
{
    addCollisionArea(QRectF(40, 400, 920, -250));
    addCollisionArea(QRectF(40, 850, 920, 40));
    addCollisionArea(QRectF(30, 300, -30, 400));
    addCollisionArea(QRectF(970, 300, 30, 400));

    addCollisionArea(QRectF(250, 500, 100, 10));
    addCollisionArea(QRectF(700, 500, 100, 10));
    addCollisionArea(QRectF(250, 600, 100, 10));
    addCollisionArea(QRectF(700, 600, 100, 10));
}

void LabDescartes::setupInteractions()
{
    clearInteractiveAreas();

    if (phase == Phase::EVIDENCIA) {
        addInteractiveArea(QRectF(250,500,140,85), "Presiona E para analizar la mesa de Evidencia");
    } else if (phase == Phase::ANALISIS) {
        addInteractiveArea(QRectF(680,500,120,85), "Presiona E para analizar la mesa de Análisis");
    } else if (phase == Phase::SINTESIS) {
        addInteractiveArea(QRectF(250,600,140,85), "Presiona E para analizar la mesa de Síntesis");
    } else if (phase == Phase::ENUMERACION) {
        addInteractiveArea(QRectF(680,600,120,85), "Presiona E para analizar la mesa de Enumeración");
    }
    addInteractiveArea(QRectF(470,415,60,60), "Presiona E para hablar con Descartes");
}


void LabDescartes::posicionarHud() {
    if (!attachedView || !hudDialogGroup) return;
    const QRect vp = attachedView->viewport()->rect();
    const QPointF topLeft = attachedView->mapToScene(vp.topLeft());
    hudDialogGroup->setPos(topLeft + QPointF(10, vp.height() - 10 - 150));
}

void LabDescartes::initHud(QGraphicsView* view) {
    attachedView = view;
    if (!attachedView) return;

    if (!hudDialogGroup) {
        hudDialogGroup = new QGraphicsItemGroup();
        hudDialogGroup->setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
        hudDialogGroup->setZValue(2000);
        addItem(hudDialogGroup);
    }

    attachedView->installEventFilter(this);
    attachedView->viewport()->installEventFilter(this);

    posicionarHud();
    updateInventoryHud();
}

void LabDescartes::mostrarBarraDialogo() {
    if (!hudDialogGroup) return;
    if (!barraDialogo) {
        barraDialogo = new QGraphicsRectItem(0,0,780,150,hudDialogGroup);
        barraDialogo->setBrush(QColor(20,20,30,230));
        barraDialogo->setPen(QPen(Qt::white,3));
    }
    if (!textoDialogo) {
        textoDialogo = new QGraphicsTextItem(hudDialogGroup);
        textoDialogo->setDefaultTextColor(Qt::white);
        textoDialogo->setFont(QFont("Minecraft",12,QFont::Bold));
        textoDialogo->setTextWidth(700);
        textoDialogo->setPos(20,15);
    }
    if (!hintDialogo) {
        hintDialogo = new QGraphicsTextItem("Presiona ESPACIO para continuar", hudDialogGroup);
        hintDialogo->setDefaultTextColor(Qt::yellow);
        hintDialogo->setFont(QFont("Minecraft",10));
        hintDialogo->setPos(20,110);
    }

    posicionarHud();

    barraDialogo->setVisible(true);
    textoDialogo->setVisible(true);
    hintDialogo->setVisible(true);
}

void LabDescartes::recreatePanelFocus() {
    if (panelFocus) {
        removeItem(panelFocus);
        delete panelFocus;
        panelFocus = nullptr;
    }
    panelFocus = new QGraphicsRectItem(0,0,760,360);
    panelFocus->setBrush(QColor(20,20,30,230));
    panelFocus->setPen(QPen(Qt::white,3));
    panelFocus->setZValue(5000);
    addItem(panelFocus);

    if (attachedView) {
        QRect vp = attachedView->viewport()->rect();
        QPointF tl = attachedView->mapToScene(vp.topLeft());
        panelFocus->setPos(tl + QPointF((vp.width()-760)/2, (vp.height()-360)/2));
    } else {
        panelFocus->setPos(120,320);
    }

    focusOpciones.clear();
    analysisPic = nullptr;
    enumPic     = nullptr;
    txtFocus    = nullptr;
}


void LabDescartes::ocultarBarraDialogo()
{
    if (barraDialogo) barraDialogo->setVisible(false);
    if (textoDialogo) textoDialogo->setVisible(false);
    if (hintDialogo)  hintDialogo->setVisible(false);
}

void LabDescartes::refrescarLineaActual()
{
    if (!textoDialogo) return;
    textoDialogo->setPlainText(dialog_lineas.value(dialog_idx));
}

bool LabDescartes::eventFilter(QObject* obj, QEvent* ev) {
    if ((obj == attachedView || obj == attachedView->viewport())) {
        switch (ev->type()) {
        case QEvent::Resize:
        case QEvent::Wheel:
        case QEvent::Paint:
        case QEvent::Show:
        case QEvent::Hide:
            posicionarHud();
            updateInventoryHud();
            break;
        default: break;
        }
    }
    return SceneBase::eventFilter(obj, ev);
}


void LabDescartes::iniciarDialogoDescartes(const QStringList& lineas)
{
    if (lineas.isEmpty() || dialog_activo) return;
    dialog_lineas = lineas;
    dialog_idx = 0;
    dialog_activo = true;

    mostrarBarraDialogo();
    refrescarLineaActual();
}

bool LabDescartes::avanzarDialogoSiActivo()
{
    if (!dialog_activo) return false;
    dialog_idx++;
    if (dialog_idx >= dialog_lineas.size()) {
        ocultarBarraDialogo();
        dialog_activo = false;
        setupInteractions();
    } else {
        refrescarLineaActual();
    }
    return true;
}


void LabDescartes::marcarRespondidoExterno(const QString& id)
{
    if (finalMostrada) return;
    respondidosExternos.insert(id);
    qDebug() << "[DescartesLab] Mesa completada:" << id
             << "total respondidas:" << respondidosExternos.size();
    checarLanzarFinal();
}

void LabDescartes::checarLanzarFinal()
{
    if (finalMostrada) return;
    if (respondidosExternos.size() >= 4) {
        finalMostrada = true;
        mostrarPreguntaFinal();
    }
}

void LabDescartes::mostrarPreguntaFinal()
{
    if (!panelPregunta) {
        panelPregunta = new QGraphicsRectItem(0,0,700,260);
        panelPregunta->setBrush(QColor(20,20,30,230));
        panelPregunta->setPen(QPen(Qt::white,3));
        panelPregunta->setZValue(1500);
        addItem(panelPregunta);
    }
    if (attachedView) {
        QRect vp = attachedView->viewport()->rect();
        QPointF tl = attachedView->mapToScene(vp.topLeft());
        panelPregunta->setPos(tl + QPointF((vp.width()-700)/2, (vp.height()-260)/2));
    } else panelPregunta->setPos(150,370);

    if (!txtPregunta) {
        txtPregunta = new QGraphicsTextItem(panelPregunta);
        txtPregunta->setDefaultTextColor(Qt::white);
        txtPregunta->setFont(QFont("Minecraft",12,QFont::Bold));
        txtPregunta->setTextWidth(660);
        txtPregunta->setPos(20,15);
    }

    txtPregunta->setPlainText(
        "Son los pasos o razones de la duda metódica:\n"
        );

    for (auto* t : txtOpciones) { if (t) t->deleteLater(); }
    txtOpciones.clear();

    QStringList ops = {
        "A) Los sentidos nos engañan",
        "B) La existencia del genio maligno",
        "C) Imposibilidad vigilia/sueño",
        "D) Todas son correctas"
    };
    int y = 120;
    for (auto& s : ops) {
        auto* t = new QGraphicsTextItem(s, panelPregunta);
        t->setDefaultTextColor(Qt::yellow);
        t->setFont(QFont("Minecraft",12,QFont::Bold));
        t->setPos(40,y);
        txtOpciones << t;
        y += 30;
    }
    panelPregunta->setVisible(true);
}

bool LabDescartes::manejarTecla(int key) {

    if (panelActivo) {
        if (key==Qt::Key_A || key==Qt::Key_B || key==Qt::Key_C || key==Qt::Key_D) {
            resolveAnswer(QChar(key));
            return true;
        }
        return false;
    }

    if (!finalMostrada || !panelPregunta) return false;

    if (key==Qt::Key_A || key==Qt::Key_B || key==Qt::Key_C || key==Qt::Key_D) {
        QChar letra = QChar(key).toUpper();
        bool correcta = (letra == 'D');
        if (txtPregunta) {
            txtPregunta->setPlainText(correcta
                                          ? "¡Felicidades! Has probado tu conocimiento.\nVolviendo a la terraza..."
                                          : "Te daré otra oportunidad…");
        }
        if (correcta) {
            QTimer::singleShot(1200, this, [this](){
                emit laboratorioResuelto();
                ocultarPregunta();
                phase = Phase::FINISHED;
            });
        }
        return true;
    }
    return false;
}

void LabDescartes::ocultarPregunta()
{
    if (panelPregunta) panelPregunta->setVisible(false);
}

void LabDescartes::startBatAnimation()
{
    batTimer.disconnect();
    connect(&batTimer, &QTimer::timeout, this, [this](){
        if (!descartesItem || batFrames.isEmpty()) return;

        batFrameIndex = (batFrameIndex + 1) % batFrames.size();
        descartesItem->setPixmap(batFrames[batFrameIndex]);

        bobTick = (bobTick + 1) % 360;
        const qreal amplitude = 6.0;
        const qreal radians   = bobTick * M_PI / 90.0;
        const qreal dy        = amplitude * std::sin(radians);
        const qreal dx        = 2.0 * std::sin(radians * 0.5);

        descartesItem->setPos(batBasePos + QPointF(dx, dy));
    });
    batTimer.start(100);
}

void LabDescartes::stopBatAnimation()
{
    batTimer.stop();
}


void LabDescartes::updateInventoryHud() {
    if (!attachedView) return;

    if (!hudInvGroup) {
        hudInvGroup = new QGraphicsItemGroup();
        hudInvGroup->setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
        hudInvGroup->setZValue(1900);
        addItem(hudInvGroup);
        hudInvGroup->setVisible(inventoryVisible);

        hudInvTitle = new QGraphicsTextItem("Inventario", hudInvGroup);
        hudInvTitle->setDefaultTextColor(Qt::white);
        hudInvTitle->setFont(QFont("Minecraft", 12, QFont::Bold));

        auto mkIcon = [&](QGraphicsPixmapItem*& ref, const QString& path, QPointF p){
            QPixmap px(path);
            if (px.isNull()) { QPixmap ph(28,28); ph.fill(Qt::gray); px = ph; }
            ref = new QGraphicsPixmapItem(
                px.scaled(QSize(28,28), Qt::KeepAspectRatio, Qt::SmoothTransformation),
                hudInvGroup
                );
            ref->setPos(p);
            ref->setOpacity(0.25);
        };
        mkIcon(iconEvid,    "://res/Objects/evidence.png", QPointF(0,20));
        mkIcon(iconEstatua, "://res/Objects/estatua.png",  QPointF(34,20));
        mkIcon(iconLazo,    "://res/Objects/lazo.png",     QPointF(68,20));
        mkIcon(iconOrden,   "://res/Objects/pensOrg.png",  QPointF(102,20));


    }

    QRect vp = attachedView->viewport()->rect();
    QPointF tr = attachedView->mapToScene(vp.topRight());

    const qreal hudW = 160;
    const qreal margin = 10;

    hudInvGroup->setPos(tr + QPointF(-hudW - margin, margin));
    hudInvTitle->setPos(0, 0);

    if (iconEvid)    iconEvid->setOpacity(haveEvidencia?1.0:0.25);
    if (iconEstatua) iconEstatua->setOpacity(haveEstatua?1.0:0.25);
    if (iconLazo)    iconLazo->setOpacity(haveLazo?1.0:0.25);
    if (iconOrden)   iconOrden->setOpacity(haveOrden?1.0:0.25);

}
void LabDescartes::showEvidenceGrid()
{
    if (panelActivo) return;
    panelActivo = true;
    respuestaCorrecta = QChar('A');

    recreatePanelFocus();
    panelFocus->setVisible(true);

    if (!txtFocus) {
        txtFocus = new QGraphicsTextItem(panelFocus);
        txtFocus->setDefaultTextColor(Qt::white);
        txtFocus->setFont(QFont("Minecraft",12,QFont::Bold));
        txtFocus->setTextWidth(720);
        txtFocus->setPos(20,15);
    }
    txtFocus->setPlainText(
        "Solo debemos aceptar como verdadero lo EVIDENTE, CLARO y DISTINTO:\n"
        "Elige con A/B/C/D."
        );

    for (auto* t : focusOpciones) { if (t) t->deleteLater(); }
    focusOpciones.clear();

    QPixmap base("://res/Objects/evidence.png");
    if (base.isNull()) {
        QPixmap ph(100,100); ph.fill(Qt::darkGray);
        base = ph;
    }
    QPixmap scaled = base.scaled(QSize(120,120), Qt::KeepAspectRatio, Qt::SmoothTransformation);

    struct Card { QString label; QString text; };
    QVector<Card> cards = {
        {"A)", "René Descartes"},
        {"B)", "David Hume"},
        {"C)", "George Berkeley"},
        {"D)", "Aristóteles"}
    };

    const int startX = 40;
    const int startY = 80;
    const int colW   = 170;

    for (int i=0; i<cards.size(); ++i) {
        int x = startX + i*colW;

        auto* pic = new QGraphicsPixmapItem(scaled, panelFocus);
        pic->setPos(x, startY);
        pic->setZValue(panelFocus->zValue()+1);

        auto* lab = new QGraphicsTextItem(cards[i].label, panelFocus);
        lab->setDefaultTextColor(QColor(255,255,0));
        lab->setFont(QFont("Minecraft",12,QFont::Bold));
        lab->setPos(x, startY + 125);

        auto* name = new QGraphicsTextItem(cards[i].text, panelFocus);
        name->setDefaultTextColor(Qt::white);
        name->setFont(QFont("Minecraft",11));
        name->setPos(x + 28, startY + 125);

        focusOpciones << lab;
        focusOpciones << name;
    }
}


void LabDescartes::showQuestion(const QString& enunciado,
                                const QStringList& opciones,
                                QChar correcta,
                                const QList<QPixmap>& extras)
{
    if (panelActivo) return;
    panelActivo = true;
    respuestaCorrecta = correcta.toUpper();

    if (!panelFocus) {
        panelFocus = new QGraphicsRectItem(0,0,760,360);
        panelFocus->setBrush(QColor(20,20,30,230));
        panelFocus->setPen(QPen(Qt::white,3));
        panelFocus->setZValue(1600);
        panelFocus->setVisible(true);
        addItem(panelFocus);
    }
    if (attachedView) {
        QRect vp = attachedView->viewport()->rect();
        QPointF tl = attachedView->mapToScene(vp.topLeft());
        panelFocus->setPos(tl + QPointF((vp.width()-760)/2, (vp.height()-360)/2));
    } else panelFocus->setPos(120,320);

    if (!txtFocus) {
        txtFocus = new QGraphicsTextItem(panelFocus);
        txtFocus->setDefaultTextColor(Qt::white);
        txtFocus->setFont(QFont("Minecraft",12,QFont::Bold));
        txtFocus->setTextWidth(720);
        txtFocus->setPos(20,15);
    }
    txtFocus->setPlainText(enunciado);

    for (auto* t : focusOpciones) { if (t) t->deleteLater(); }
    focusOpciones.clear();

    int y = 150;
    for (const auto& s : opciones) {
        auto* t = new QGraphicsTextItem(s, panelFocus);
        t->setDefaultTextColor(Qt::yellow);
        t->setFont(QFont("Minecraft",12,QFont::Bold));
        t->setPos(40, y);
        focusOpciones << t;
        y += 28;
    }

    int xExtra = 40; int yExtra = 80;
    for (const auto& px : extras) {
        auto* extra = new QGraphicsPixmapItem(px.scaled(QSize(80,80), Qt::KeepAspectRatio, Qt::SmoothTransformation), panelFocus);
        extra->setPos(xExtra, yExtra);
        xExtra += 90;
    }
}

void LabDescartes::closeQuestion() {
    panelActivo = false;
    if (panelFocus) { removeItem(panelFocus); delete panelFocus; panelFocus = nullptr; }
    focusOpciones.clear();
    txtFocus = nullptr;
    analysisPic = nullptr;
    enumPic = nullptr;
}


void LabDescartes::resolveAnswer(QChar letter) {
    if (!panelActivo || resolvingAnswer) return;
    letter = letter.toUpper();
    const bool ok = (letter == respuestaCorrecta);

    if (txtFocus) {
        txtFocus->setDefaultTextColor(ok ? QColor(120,255,120) : QColor(255,150,120));
        txtFocus->setPlainText(ok
                                   ? "¡Correcto!"
                                   : "Respuesta incorrecta. Intenta de nuevo (A/B/C/D).");
    }

    if (!ok) {
        QTimer::singleShot(800, this, [this](){
            panelActivo = false;

            switch (phase) {
            case Phase::EVIDENCIA:   showEvidenceGrid();      break;
            case Phase::ANALISIS:    showAnalysisQuestion();  break;
            case Phase::SINTESIS:    showSynthesisQuestion(); break;
            case Phase::ENUMERACION: showEnumerationQuestion(); break;
            default: break;
            }
        });
        return;
    }

    resolvingAnswer = true;

    if      (phase == Phase::EVIDENCIA)   haveEvidencia = true;
    else if (phase == Phase::ANALISIS)    haveEstatua   = true;
    else if (phase == Phase::SINTESIS)    haveLazo      = true;
    else if (phase == Phase::ENUMERACION) {
        haveOrden = true;
        if (enumPic) {
            QPixmap pxOrg("://res/Objects/pensOrg.png");
            if (!pxOrg.isNull()) {
                enumPic->setPixmap(pxOrg.scaled(enumPic->pixmap().size(),
                                                Qt::KeepAspectRatio,
                                                Qt::SmoothTransformation));
            }
        }
    }

    updateInventoryHud();

    QTimer::singleShot(700, this, [this](){
        closeQuestion();
        resolvingAnswer = false;
        advancePhase();
    });
}


void LabDescartes::advancePhase() {
    if      (phase == Phase::EVIDENCIA)     phase = Phase::ANALISIS;
    else if (phase == Phase::ANALISIS)      phase = Phase::SINTESIS;
    else if (phase == Phase::SINTESIS)      phase = Phase::ENUMERACION;
    else if (phase == Phase::ENUMERACION)   phase = Phase::FINAL_READY;

    setupInteractions();
    maybePromptTalkToDescartes();
}

void LabDescartes::maybePromptTalkToDescartes() {
    if (phase == Phase::FINAL_READY && haveEvidencia && haveEstatua && haveLazo && haveOrden) {
        iniciarDialogoDescartes(QStringList{
            "Descartes: Has reunido toda la evidencia del método.",
            "Descartes: Ven a hablar conmigo para tu última pregunta."
        });
    }
}


bool LabDescartes::tryHandleInteraction(const QString& msg) {
    if (msg.contains("hablar con Descartes", Qt::CaseInsensitive)) {
        if (phase == Phase::FINAL_READY) {
            finalMostrada = true;
            mostrarPreguntaFinal();
        } else {
            iniciarDialogoDescartes(QStringList{
                "Descartes: Bienvenido al laboratorio.",
                "Descartes: Avanza por Evidencia, Análisis, Síntesis y Enumeración y prueba tu conocimiento",
                "Cuando termines ven y te haré la prueba final"
            });
        }
        return true;
    }

    if (panelActivo) return true;

    if (phase == Phase::EVIDENCIA &&
        msg.contains("mesa de Evidencia", Qt::CaseInsensitive)) {
        showEvidenceGrid();
        return true;
    }

    if (phase == Phase::ANALISIS &&
        (msg.contains("mesa de Análisis", Qt::CaseInsensitive) ||
         msg.contains("mesa de Analisis", Qt::CaseInsensitive))) {
        showAnalysisQuestion();
        return true;
    }

    if (phase == Phase::SINTESIS &&
        (msg.contains("mesa de Síntesis", Qt::CaseInsensitive) ||
         msg.contains("mesa de Sintesis", Qt::CaseInsensitive))) {
        showSynthesisQuestion();
        return true;
    }

    if (phase == Phase::ENUMERACION &&
        (msg.contains("mesa de Enumeración", Qt::CaseInsensitive) ||
         msg.contains("mesa de Enumeracion", Qt::CaseInsensitive))) {
        showEnumerationQuestion();
        return true;
    }

    return false;
}

void LabDescartes::showAnalysisQuestion()
{
    if (panelActivo) return;
    panelActivo = true;
    respuestaCorrecta = QChar('A');

    recreatePanelFocus();
    panelFocus->setVisible(true);

    if (!txtFocus) {
        txtFocus = new QGraphicsTextItem(panelFocus);
        txtFocus->setDefaultTextColor(Qt::white);
        txtFocus->setFont(QFont("Minecraft",12,QFont::Bold));
        txtFocus->setTextWidth(720);
        txtFocus->setPos(20,15);
    }
    txtFocus->setPlainText(
        "En cuanto a la certeza del conocimiento, René Descartes afirma lo siguiente:\n"
        "A) Es preciso dudar.\nB) Debemos confiar ciegamente.\nC) Nada es importante.\nD) Todo es posible.\n"
        "Elige con A/B/C/D."
        );

    for (auto* t : focusOpciones) { if (t) t->deleteLater(); }
    focusOpciones.clear();

    QPixmap pista;
    pista.load("://res/Objects/estatua.png");
    if (pista.isNull()) { QPixmap ph(100,100); ph.fill(Qt::darkGray); pista = ph; }

    QPixmap scaled = pista.scaled(QSize(140,140), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    analysisPic = new QGraphicsPixmapItem(scaled, panelFocus);
    analysisPic->setPos(310, 160);
    analysisPic->setZValue(panelFocus->zValue()+1);
}

void LabDescartes::showSynthesisQuestion()
{
    if (panelActivo) return;
    panelActivo = true;
    respuestaCorrecta = QChar('B');

    recreatePanelFocus();
    panelFocus->setVisible(true);

    if (!txtFocus) {
        txtFocus = new QGraphicsTextItem(panelFocus);
        txtFocus->setDefaultTextColor(Qt::white);
        txtFocus->setFont(QFont("Minecraft",12,QFont::Bold));
        txtFocus->setTextWidth(720);
        txtFocus->setPos(20,15);
    }
    txtFocus->setPlainText(
        "En relación con los datos experimentales que otorgan los sentidos en el conocimiento, Descartes define:\n"
        "A) Los sentidos son una fuente confiable para conocer.\n"
        "B) Desconfianza de lo que los sentidos nos proporcionan.\n"
        "C) Los sentidos son complementarios.\n"
        "D) Los sentidos son determinantes.\n"
        "Elige con A/B/C/D."
        );

    for (auto* t : focusOpciones) { if (t) t->deleteLater(); }
    focusOpciones.clear();

    auto loadOrPh = [](const QStringList& paths, const QSize& sz) {
        QPixmap px;
        for (const auto& p: paths) { if (px.load(p)) break; }
        if (px.isNull()) { QPixmap ph(sz); ph.fill(Qt::darkGray); px = ph; }
        return px.scaled(sz, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    };

    QPixmap pxEvid = loadOrPh({"://res/Objects/evidence.png", ":/res/Objects/evidence.png"}, QSize(110,110));
    QPixmap pxEst  = loadOrPh({"://res/Objects/estatua.png",  ":/res/Objects/estatua.png"},  QSize(110,110));

    auto* imgE = new QGraphicsPixmapItem(pxEvid, panelFocus);
    imgE->setPos(230, 180);
    auto* imgS = new QGraphicsPixmapItem(pxEst,  panelFocus);
    imgS->setPos(420, 180);

    auto mkLbl = [&](const QString& s, qreal x){ auto* t=new QGraphicsTextItem(s, panelFocus);
        t->setDefaultTextColor(QColor(220,220,220)); t->setFont(QFont("Minecraft",10)); t->setPos(x, 295); return t; };
    mkLbl("Evidencia", 240);
    mkLbl("Estatua",   440);
}

void LabDescartes::showEnumerationQuestion()
{
    if (panelActivo) return;
    panelActivo = true;
    respuestaCorrecta = QChar('C');

    recreatePanelFocus();
    panelFocus->setVisible(true);

    if (!txtFocus) {
        txtFocus = new QGraphicsTextItem(panelFocus);
        txtFocus->setDefaultTextColor(Qt::white);
        txtFocus->setFont(QFont("Minecraft",12,QFont::Bold));
        txtFocus->setTextWidth(720);
        txtFocus->setPos(20,15);
    }
    txtFocus->setPlainText(
        "Una de las siguientes es considerada de las cuatro reglas del método en Descartes:\n"
        "A) Hipótesis.\nB) Deducción.\nC) Evidencia.\nD) Inducción.\n"
        "Elige con A/B/C/D."
        );

    for (auto* t : focusOpciones) { if (t) t->deleteLater(); }
    focusOpciones.clear();

    auto loadOrPh = [](const QStringList& paths, const QSize& sz) {
        QPixmap px;
        for (const auto& p: paths) { if (px.load(p)) break; }
        if (px.isNull()) { QPixmap ph(sz); ph.fill(Qt::darkGray); px = ph; }
        return px.scaled(sz, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    };

    QPixmap pxDis;
    if (!pxDis.load("://res/Objects/pensDis.png")) {
        QPixmap ph(200,160); ph.fill(Qt::darkGray);
        pxDis = ph;
    }
    pxDis = pxDis.scaled(QSize(200,160), Qt::KeepAspectRatio, Qt::SmoothTransformation);

    enumPic = new QGraphicsPixmapItem(pxDis, panelFocus);
    enumPic->setPos(280, 170);
    enumPic->setZValue(panelFocus->zValue()+1);

}

void LabDescartes::clearPanelFocusChildren() {
    if (!panelFocus) return;

    const auto childs = panelFocus->childItems();
    for (QGraphicsItem* it : childs) {
        it->setParentItem(nullptr);
        delete it;
    }

    focusOpciones.clear();
    analysisPic = nullptr;
    enumPic     = nullptr;
    txtFocus    = nullptr;
}
