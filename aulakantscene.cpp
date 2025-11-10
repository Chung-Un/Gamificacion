#include "aulakantscene.h"
#include <QPixmap>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsView>
#include <QScrollBar>
#include <QTimer>
#include <QKeyEvent>

AulaKantScene::AulaKantScene(QObject* parent)
    : SceneBase(parent)
{
    loadScene();
}

void AulaKantScene::loadScene()
{
    setSceneRect(0, 0, 1000, 1000);

    {
        QPixmap bg("://res/ruletaEscenario.png");
        if (bg.isNull()) {
            QPixmap solid(sceneRect().size().toSize());
            solid.fill(QColor(35,25,35));
            setBackground(solid);
        } else {
            setBackground(bg.scaled(sceneRect().size().toSize(),
                                    Qt::KeepAspectRatioByExpanding,
                                    Qt::SmoothTransformation));
        }
    }

    colocarProps();
    setupCollisions();
    setupInteractions();
    construirBancoPreguntas();
    actualizarHudPistas();
    finalCorrecta   = 'A';
    finalEnunciado  =
        "¿Quién robó la pluma de Kant?\n";
    finalOpciones = {
        "A) El bibliotecario",
        "B) Un racionalista",
        "C) El escéptico",
        "D) El empirista"
    };


}



QGraphicsPixmapItem* AulaKantScene::addProp(const QString& resPath, const QPointF& pos,
                                            const QSize& targetSize, qreal z)
{
    QPixmap px(resPath);
    if (px.isNull()) {
        QPixmap ph(targetSize);
        ph.fill(QColor(120,120,120));
        auto* it = new QGraphicsPixmapItem(ph);
        it->setPos(pos);
        it->setZValue(z);
        addItem(it);
        return it;
    }
    auto scaled = px.scaled(targetSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    auto* it = new QGraphicsPixmapItem(scaled);
    it->setPos(pos);
    it->setZValue(z);
    addItem(it);
    return it;
}

void AulaKantScene::colocarProps()
{
    libreroItem = addProp("://res/Objects/librero.png", QPointF(50, 380), QSize(140, 200), 5);
    pizarraItem = addProp("://res/Objects/pizarra.jpg", QPointF(355, 280), QSize(300, 180), 5);
    lamparaL = addProp("://res/Objects/lamp.png", QPointF(270, 250), QSize(100, 120), 6);
    lamparaR = addProp("://res/Objects/lamp.png", QPointF(690, 250), QSize(100, 120), 6);
    posterItem = addProp("://res/Objects/solar.png", QPointF(785, 300), QSize(150, 180), 5);
    plantaL = addProp("://res/Objects/planta.png", QPointF(300, 410), QSize(70, 90), 6);
    plantaR = addProp("://res/Objects/planta.png", QPointF(700, 410), QSize(70, 90), 6);
    pupitre0 = addProp("://res/Objects/desk.png", QPointF(430, 600), QSize(120, 85), 6);
    pupitre1 = addProp("://res/Objects/desk.png", QPointF(560, 600), QSize(120, 85), 6);
    pupitre2 = addProp("://res/Objects/desk.png", QPointF(690, 600), QSize(120, 85), 6);
    kant = addProp("://res/Objects/kant.png", QPointF(160, 450), QSize(150, 140), 6);
}

void AulaKantScene::setupCollisions()
{
    addCollisionArea(QRectF(40, 300, 920, -250));
    addCollisionArea(QRectF(40, 850, 920, 40));
    addCollisionArea(QRectF(30, 300, -30, 400));
    addCollisionArea(QRectF(970, 300, 30, 400));

    addCollisionArea(QRectF(50, 380, 150, 50));
    addCollisionArea(QRectF(350, 430, 310, -80));
    addCollisionArea(QRectF(780, 300, 160, 70));
    addCollisionArea(QRectF(430, 600, 120, 10));
    addCollisionArea(QRectF(560, 600, 120, 10));
    addCollisionArea(QRectF(690, 600, 120, 10));
}

void AulaKantScene::setupInteractions()
{
    clearInteractiveAreas();

    addInteractiveArea(QRectF(430, 300, 150, 150),
                       "Presiona E para inspeccionar a la pizarra de Kant");

    addInteractiveArea(QRectF(50, 380, 135, 185),
                       "Presiona E para inspeccionar el librero");

    addInteractiveArea(QRectF(830, 300, 145, 170),
                       "Presiona E para ver el sistema solar");

    addInteractiveArea(QRectF(300, 410, 20, 90),
                       "Presiona E para inspeccionar la planta 1");
    addInteractiveArea(QRectF(700, 410, 20, 90),
                       "Presiona E para inspeccionar la planta 2");

    addInteractiveArea(QRectF(160, 430, 140, 120),
                       "Presiona E para hablar con Kant");
}


void AulaKantScene::iniciarDialogoKant(const QStringList& lineas)
{
    if (lineas.isEmpty()) return;
    if (dialog_activo) return;

    dialog_lineas = lineas;
    dialog_idx = 0;
    dialog_activo = true;

    clearInteractiveAreas();

    mostrarBarraDialogo();
    refrescarLineaActual();

    emit dialogoIniciado();
}

bool AulaKantScene::avanzarDialogoSiActivo()
{
    if (!dialog_activo) return false;

    dialog_idx++;
    if (dialog_idx >= dialog_lineas.size()) {
        ocultarBarraDialogo();
        dialog_activo = false;

        setupInteractions();
        emit dialogoTerminado();
    } else {
        refrescarLineaActual();
    }
    return true;
}

void AulaKantScene::mostrarBarraDialogo()
{
    if (!hudDialogGroup) return;

    if (!barraDialogo) {
        barraDialogo = new QGraphicsRectItem(0, 0, 780, 150, hudDialogGroup);
        barraDialogo->setBrush(QColor(20, 20, 30, 230));
        barraDialogo->setPen(QPen(Qt::white, 3));
    }

    if (!textoDialogo) {
        textoDialogo = new QGraphicsTextItem(hudDialogGroup);
        textoDialogo->setDefaultTextColor(Qt::white);
        textoDialogo->setFont(QFont("Minecraft", 12, QFont::Bold));
        textoDialogo->setTextWidth(750);
        textoDialogo->setPos(20, 15);
    }

    if (!hintDialogo) {
        hintDialogo = new QGraphicsTextItem("Presiona ESPACIO para continuar", hudDialogGroup);
        hintDialogo->setDefaultTextColor(Qt::yellow);
        hintDialogo->setFont(QFont("Minecraft", 10));
        hintDialogo->setPos(20, 110);
    }

    barraDialogo->setVisible(true);
    textoDialogo->setVisible(true);
    hintDialogo->setVisible(true);

    updateHudPos();
}

void AulaKantScene::reconstruirOpcionesFinal(const QStringList& opciones)
{
    for (auto* t : txtOpciones) { if (t) t->deleteLater(); }
    txtOpciones.clear();

    int y = 120;
    for (const auto& s : opciones) {
        auto* t = new QGraphicsTextItem(s, panelPregunta);
        t->setDefaultTextColor(Qt::yellow);
        t->setFont(QFont("Minecraft",12,QFont::Bold));
        t->setPos(40,y);
        txtOpciones << t;
        y += 30;
    }
}

void AulaKantScene::ocultarBarraDialogo()
{
    if (barraDialogo) barraDialogo->setVisible(false);
    if (textoDialogo) textoDialogo->setVisible(false);
    if (hintDialogo)  hintDialogo->setVisible(false);
}

void AulaKantScene::refrescarLineaActual()
{
    if (!textoDialogo) return;
    const QString& line = dialog_lineas.value(dialog_idx);
    textoDialogo->setPlainText(line);
}

void AulaKantScene::initHud(QGraphicsView* view)
{
    attachedView = view;
    if (!attachedView) return;

    if (!hudDialogGroup) {
        hudDialogGroup = new QGraphicsItemGroup();
        hudDialogGroup->setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
        hudDialogGroup->setZValue(2000);
        addItem(hudDialogGroup);
    }

    connect(attachedView->horizontalScrollBar(), &QScrollBar::valueChanged,
            this, &AulaKantScene::updateHudPos);
    connect(attachedView->verticalScrollBar(), &QScrollBar::valueChanged,
            this, &AulaKantScene::updateHudPos);

    if (!hudPistasGroup) {
        hudPistasGroup = new QGraphicsItemGroup();
        hudPistasGroup->setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
        hudPistasGroup->setZValue(2000);
        addItem(hudPistasGroup);

        auto box = new QGraphicsRectItem(0,0,220,60, hudPistasGroup);
        box->setBrush(QColor(30,30,40,230));
        box->setPen(QPen(Qt::white,2));

        textoPistasHud = new QGraphicsTextItem("Pistas: 0/6\nV: ver", hudPistasGroup);
        textoPistasHud->setDefaultTextColor(Qt::white);
        textoPistasHud->setFont(QFont("Minecraft", 10, QFont::Bold));
        textoPistasHud->setPos(10,8);
    }


    updateHudPos();
}

void AulaKantScene::updateHudPos()
{
    const QRect vp = attachedView->viewport()->rect();
    const QPointF topLeftScene = attachedView->mapToScene(vp.topLeft());

    const qreal margin = 20;
    if (hudDialogGroup) {
        QPointF bottomLeft = topLeftScene + QPointF(margin, vp.height()-150-margin);
        hudDialogGroup->setPos(bottomLeft);
    }
    if (hudPistasGroup) {
        QPointF topRight = topLeftScene + QPointF(vp.width()-220-margin, margin);
        hudPistasGroup->setPos(topRight);
    }

}

void AulaKantScene::construirBancoPreguntas()
{
    preguntas.clear();
    preguntas << Pregunta{
        "1) Seleccione el mandato cuya obligación viene del miedo al castigo o la búsqueda de un premio:",
        {"A) Imperativo Hipotético.","B) Imperativo categórico.","C) Ambos.","D) Ninguno"}, 'A',
        "La moral por conveniencia no es moral: recuerda el **imperativo hipotético**."
    };
    preguntas << Pregunta{
        "2) Para Emanuel Kant, es posible conocer lo que las cosas nos permiten a través de nuestros sentidos:",
        {"A) Conocimiento Nouménico.","B) Conocimiento fenoménico.","C) Conocimiento empírico.","D) Conocimiento racional"}, 'B',
        "Solo accedemos al **fenómeno**, no al noúmeno."
    };
    preguntas << Pregunta{
        "3) Kant decía que el lema de la Ilustración era “Sapere aude”, que significa:",
        {"A) Sopesa tus acciones.","B) Atrévete a saber por ti mismo.","C) Saber a la fuerza.","D) Someterse al conocimiento"}, 'B',
        "La pista brilla en tiza: **Atrévete a saber por ti mismo**."
    };
    preguntas << Pregunta{
        "4) Kant cambia el centro del conocimiento del objeto al sujeto. A esto se le llama:",
        {"A) Subjetivismo.","B) Prejuicio.","C) Giro copernicano.","D) Suerte"}, 'C',
        "El **giro copernicano** de Kant mueve el foco al sujeto."
    };
    preguntas << Pregunta{
        "5) La postura conciliadora de Kant respecto a empiristas y racionalistas:",
        {"A) Racionalismo.","B) Empirismo.","C) Criticismo.","D) Escepticismo"}, 'C',
        "El **criticismo** articula cómo conoce el sujeto."
    };
    preguntas << Pregunta{
        "6) De estas obras, la que define su epistemología:",
        {"A) Crítica de la razón práctica.","B) Crítica de la razón pura.","C) Crítica del juicio.","D) Crítica fenomenológica"}, 'B',
        "La **Crítica de la razón pura** fija el marco del conocer."
    };

    mapaObjetoPregunta.clear();
    mapaObjetoPregunta["planta1"] = 1;
    mapaObjetoPregunta["planta2"] = 2;
    mapaObjetoPregunta["poster"]  = 4;
}

void AulaKantScene::actualizarHudPistas()
{
    if (textoPistasHud)
        textoPistasHud->setPlainText(QString("Pistas: %1/6\nV: ver").arg(pistasDesbloqueadas.size()));
}

void AulaKantScene::mostrarPreguntaDesdeMensaje(const QString& msg)
{
    if (preguntaActiva || dialog_activo) return;

    const auto f = Qt::CaseInsensitive;
    if      (msg.contains("planta 1", f)) objetoActual = "planta1";
    else if (msg.contains("planta 2", f)) objetoActual = "planta2";
    else if (msg.contains("sistema solar", f)) objetoActual = "poster";
    else return;

    if (objetosResueltos.contains(objetoActual)) return;

    int idx = mapaObjetoPregunta.value(objetoActual, -1);
    if (idx < 0) return;

    mostrarPregunta(idx);
}


void AulaKantScene::mostrarPregunta(int idx)
{
    preguntaActiva = true;

    if (!panelPregunta) {
        panelPregunta = new QGraphicsRectItem(0,0,700,260);
        panelPregunta->setBrush(QColor(70,23,180,230));
        panelPregunta->setPen(QPen(Qt::white,3));
        panelPregunta->setZValue(1500);
        addItem(panelPregunta);
    }
    if (attachedView) {
        QRect vp = attachedView->viewport()->rect();
        QPointF tl = attachedView->mapToScene(vp.topLeft());
        panelPregunta->setPos(tl + QPointF((vp.width()-700)/2, (vp.height()-260)/2));
    } else {
        panelPregunta->setPos(150, 370);
    }

    if (!txtPregunta) {
        txtPregunta = new QGraphicsTextItem(panelPregunta);
        txtPregunta->setDefaultTextColor(Qt::white);
        txtPregunta->setFont(QFont("Minecraft", 12, QFont::Bold));
        txtPregunta->setTextWidth(660);
        txtPregunta->setPos(20,15);
    }
    txtPregunta->setPlainText(preguntas[idx].enunciado);

    for (auto* t : txtOpciones) { if (t) t->deleteLater(); }
    txtOpciones.clear();

    int y = 100;
    for (const QString& opt : preguntas[idx].opciones) {
        auto* t = new QGraphicsTextItem(opt, panelPregunta);
        t->setDefaultTextColor(Qt::yellow);
        t->setFont(QFont("Minecraft", 12, QFont::Bold));
        t->setPos(40, y);
        txtOpciones << t;
        y += 30;
    }
    panelPregunta->setVisible(true);
}

void AulaKantScene::ocultarPregunta()
{
    if (panelPregunta) panelPregunta->setVisible(false);
    preguntaActiva = false;
    objetoActual.clear();
}

void AulaKantScene::resolverPregunta(bool correcta)
{
    const int idx = mapaObjetoPregunta.value(objetoActual, -1);

    objetosResueltos.insert(objetoActual);

    if (correcta) {
        if (!pistasDesbloqueadas.contains(preguntas[idx].pista))
            pistasDesbloqueadas << preguntas[idx].pista;
        if (txtPregunta) txtPregunta->setPlainText("¡Correcto! Pista obtenida:\n• " + preguntas[idx].pista);
    } else {
        if (txtPregunta) txtPregunta->setPlainText("Incorrecto. Explora más y vuelve a intentarlo.");
    }

    actualizarHudPistas();

    qDebug() << "[Aula] Resolver pregunta de" << objetoActual
             << "correcta?" << correcta
             << "objetosResueltos:" << objetosResueltos.size();

    QTimer::singleShot(1400, this, [this](){
        ocultarPregunta();
        checarLanzarFinal();
    });
}

void AulaKantScene::mostrarPreguntaFinal()
{
    preguntaActiva = true;
    objetoActual = "final";

    if (!panelPregunta) {
        panelPregunta = new QGraphicsRectItem(0,0,700,260);
        panelPregunta->setBrush(QColor(191, 48, 48));
        panelPregunta->setPen(QPen(Qt::white,3));
        panelPregunta->setZValue(1500);
        addItem(panelPregunta);
    }
    if (attachedView) {
        QRect vp = attachedView->viewport()->rect();
        QPointF tl = attachedView->mapToScene(vp.topLeft());
        panelPregunta->setPos(tl + QPointF((vp.width()-700)/2, (vp.height()-260)/2));
    } else {
        panelPregunta->setPos(150, 370);
    }

    if (!txtPregunta) {
        txtPregunta = new QGraphicsTextItem(panelPregunta);
        txtPregunta->setDefaultTextColor(Qt::white);
        txtPregunta->setFont(QFont("Minecraft", 12, QFont::Bold));
        txtPregunta->setTextWidth(660);
        txtPregunta->setPos(20,15);
    }

    txtPregunta->setPlainText(finalEnunciado);
    reconstruirOpcionesFinal(finalOpciones);
    panelPregunta->setVisible(true);
}



bool AulaKantScene::togglePistasIfHud(QChar tecla)
{
    if (tecla.toUpper() != 'V' || dialog_activo || preguntaActiva) return false;
    overlayPistasVisible ? ocultarOverlayPistas() : mostrarOverlayPistas();
    return true;
}

void AulaKantScene::mostrarOverlayPistas()
{
    if (overlayPistasVisible) return;
    overlayPistasVisible = true;

    if (!overlayPistas) {
        overlayPistas = new QGraphicsRectItem(0,0,720,360);
        overlayPistas->setBrush(QColor(0,0,0,200));
        overlayPistas->setPen(QPen(Qt::white,3));
        overlayPistas->setZValue(1800);
        addItem(overlayPistas);
    }
    if (attachedView) {
        QRect vp = attachedView->viewport()->rect();
        QPointF tl = attachedView->mapToScene(vp.topLeft());
        overlayPistas->setPos(tl + QPointF((vp.width()-720)/2, (vp.height()-360)/2));
    } else overlayPistas->setPos(140,320);

    for (auto* t : overlayLineas) { if (t) t->deleteLater(); }
    overlayLineas.clear();

    auto* title = new QGraphicsTextItem("Pistas desbloqueadas", overlayPistas);
    title->setDefaultTextColor(Qt::white);
    title->setFont(QFont("Minecraft", 16, QFont::Bold));
    title->setPos(20, 15);
    overlayLineas << title;

    int y = 60;
    int i = 1;
    for (const QString& p : pistasDesbloqueadas) {
        auto* t = new QGraphicsTextItem(QString("• %1) %2").arg(i++).arg(p), overlayPistas);
        t->setDefaultTextColor(Qt::yellow);
        t->setFont(QFont("Minecraft", 12));
        t->setTextWidth(680);
        t->setPos(20, y);
        overlayLineas << t;
        y += 28;
    }
    overlayPistas->setVisible(true);
}

void AulaKantScene::ocultarOverlayPistas()
{
    overlayPistasVisible = false;
    if (overlayPistas) overlayPistas->setVisible(false);
}

bool AulaKantScene::responderOpcionIfActivo(QChar letra)
{
    if (!preguntaActiva) return false;
    letra = letra.toUpper();

    if (objetoActual == "final") {
        const bool correcta = (letra == finalCorrecta);
        if (correcta) {
            if (txtPregunta) txtPregunta->setPlainText("¡Caso resuelto! Volviendo a la terraza...");
            QTimer::singleShot(1200, this, [this](){
                ocultarPregunta();
                emit aulaResuelta();
            });
        } else {
            if (txtPregunta) {
                txtPregunta->setDefaultTextColor(QColor(255, 220, 150));
                txtPregunta->setPlainText("No parece correcto. Intenta de nuevo (A/B/C/D).");
            }
            QTimer::singleShot(900, this, [this](){
                if (!panelPregunta) return;
                if (txtPregunta) {
                    txtPregunta->setDefaultTextColor(Qt::white);
                    txtPregunta->setPlainText(finalEnunciado);
                }
                reconstruirOpcionesFinal(finalOpciones);
            });
        }
        return true;
    }

    int idx = mapaObjetoPregunta.value(objetoActual, -1);
    if (idx < 0) { ocultarPregunta(); return true; }

    resolverPregunta(letra == preguntas[idx].correcta);
    return true;
}

bool AulaKantScene::manejarTecla(int key)
{
    if (!preguntaActiva) return false;

    if (key == Qt::Key_A || key == Qt::Key_B || key == Qt::Key_C || key == Qt::Key_D) {
        responderOpcionIfActivo(QChar(key));
        return true;
    }

    if (key == Qt::Key_W || key == Qt::Key_A || key == Qt::Key_S || key == Qt::Key_D) {
        return true;
    }

    return false;
}
void AulaKantScene::addPistaDesdeFuera(const QString& pista)
{
    if (pista.isEmpty()) return;
    if (!pistasDesbloqueadas.contains(pista)) {
        pistasDesbloqueadas << pista;
        actualizarHudPistas();
        checarLanzarFinal();
    }
}

void AulaKantScene::checarLanzarFinal()
{
    if (finalMostrada) {
        qDebug() << "[Aula] Final ya mostrada. No hago nada.";
        return;
    }

    const int totalRespondidas = objetosResueltos.size() + respondidosExternos.size();
    qDebug() << "[Aula] Checar final -> objetosResueltos:" << objetosResueltos.size()
             << "respondidosExternos:" << respondidosExternos.size()
             << "totalRespondidas:" << totalRespondidas;

    if (totalRespondidas >= TOTAL_RESPUESTAS_NECESARIAS) {
        finalMostrada = true;
        qDebug() << "[Aula] MOSTRAR PREGUNTA FINAL";
        mostrarPreguntaFinal();
    }
}


void AulaKantScene::marcarRespondidoExterno(const QString& id)
{
    if (finalMostrada) return;
    respondidosExternos.insert(id);
    qDebug() << "[Aula] Respondido externo:" << id
             << "respondidosExternos:" << respondidosExternos.size();
    checarLanzarFinal();
}

