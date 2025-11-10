#include "juegohistoria.h"
#include <QKeyEvent>
#include <QDebug>
#include <QFont>
#include <QBrush>
#include <QPen>

JuegoHistoria::JuegoHistoria(QObject *parent)
    : SceneBase(parent)
{
    cantCorrectas = 0;
    noPregunta = 1;
    interactionBlocked = false;
    esperandoRespuesta = false;
    time = 20;
    llavesConseguidas = 0;
    llaveActual = -1;
    textPregunta = nullptr;
    barraPregunta = nullptr;
    textTiempo = nullptr;
    textoInteraccion = nullptr;
    textoEstado = nullptr;
    textoSalida = nullptr;
    posicionCambioEscena = QPointF(670, 520);

    llavesRecogidas = QList<bool>({false, false, false, false, false});

    preguntas.enqueue("Después del feudalismo medieval acudimos al surgimiento de una nueva clase social conocida como la:");
    respuestasCorrectas.enqueue("C");
    opciones.enqueue(QStringList{"A) La monarquía", "B) El mercantilismo", "C) La burguesía", "D) El proletariado"});

    preguntas.enqueue("El renacimiento supone una época de absolutismo y nacionalismos, como el nacimiento de fuertes monarquías europeas centralizadas como:");
    respuestasCorrectas.enqueue("B");
    opciones.enqueue(QStringList{"A) Grecia", "B) Inglaterra", "C) Yugoslavia", "D) Egipto"});

    preguntas.enqueue("Antes de la consolidación del estado moderno, Italia estuvo divida en pequeñas ciudades-estado normalmente enfrentadas entre si, como es el caso de:");
    respuestasCorrectas.enqueue("A");
    opciones.enqueue(QStringList{"A) Florencia-Napoli", "B) Ámsterdam-Cracovia", "C) Reims-Colonia", "D) Milán-Lourdes"});

    preguntas.enqueue("La toma de Constantinopla supone un bloqueo comercial entre Europa y Asia (la ruta de la seda) y ocurrió en lo que hoy es actualmente:");
    respuestasCorrectas.enqueue("B");
    opciones.enqueue(QStringList{"A) Eslovaquia", "B) Estambul en Turquía", "C) Mesopotamia", "D) Jerusalén"});

    preguntas.enqueue("Resurge el interés por Grecia y Roma, junto al declive del sistema feudal, el crecimiento del comercio e innovaciones entre las que mencionamos:");
    respuestasCorrectas.enqueue("A");
    opciones.enqueue(QStringList{"A) La imprenta y la brújula", "B) La rueda y la escritura", "C) Las máquinas de vapor y la producción en masa", "D) La pólvora y La rueda"});

    timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, this, &JuegoHistoria::onTimeout);

    timerVisual = new QTimer(this);
    connect(timerVisual, &QTimer::timeout, this, &JuegoHistoria::actualizarTimer);

    loadScene();
}

void JuegoHistoria::loadScene()
{
    QPixmap bgPixmap("://res/laberinto.png");
    setBackground(bgPixmap.scaled(800, 600));

    setupInteractiveObjects();
    setupInteractions();
    setupCollisions();

#ifdef QT_DEBUG
    QGraphicsRectItem* salidaRect = new QGraphicsRectItem(posicionCambioEscena.x() - 20,
                                                          posicionCambioEscena.y() - 20,
                                                          40, 40);
    salidaRect->setBrush(QBrush(QColor(0,255,0,100)));
    salidaRect->setPen(QPen(Qt::green, 2));
    salidaRect->setZValue(700);
    addItem(salidaRect);
#endif
}


void JuegoHistoria::setupCollisions()
{
    addCollisionArea(QRectF(125, 195, 10, 80));
    addCollisionArea(QRectF(760, 30, 45, 530));
    addCollisionArea(QRectF(30, 280, 180, 10));
    addCollisionArea(QRectF(30, 530, 700, 10));
    addCollisionArea(QRectF(122, 440, 10, 80));
    addCollisionArea(QRectF(385, 270, 10, 250));
    addCollisionArea(QRectF(520, 350, 10, 160));
    addCollisionArea(QRectF(670, 260, 100, 20));
    addCollisionArea(QRectF(670, 410, 100, 20));
    addCollisionArea(QRectF(236, 415, 180, 10));
    addCollisionArea(QRectF(390, 260, 100, 3));
    addCollisionArea(QRectF(215, 170, 60, 20));
    addCollisionArea(QRectF(465, 150, 140, 20));
    addCollisionArea(QRectF(465, 150, 10, 90));
    addCollisionArea(QRectF(215, 90, 10, 70));
}

void JuegoHistoria::setupInteractions()
{
    clearInteractiveAreas();
}

void JuegoHistoria::setupInteractiveObjects()
{
    int x[] = {250, 500, 50, 360, 360};
    int y[] = {90, 190, 480, 480, 350};

    for(int i = 0; i < 5; i++) {
        QString ruta = QString("://res/Objects/key%1.png").arg(i + 1);
        QPixmap keyPixmap(ruta);

        QGraphicsPixmapItem* key = new QGraphicsPixmapItem(keyPixmap.scaled(24, 49));
        key->setPos(x[i], y[i]);
        key->setZValue(100);
        key->setData(0, i);
        addObject(key);
        llaves.append(key);
    }

    textoEstado = new QGraphicsTextItem();
    textoEstado->setPlainText(QString::number(llavesConseguidas) + "/5 llaves conseguidas");
    textoEstado->setDefaultTextColor(Qt::white);
    textoEstado->setFont(QFont("Minecraft", 12, QFont::Bold));
    textoEstado->setPos(550, 10);
    textoEstado->setZValue(500);
    addObject(textoEstado);
}

void JuegoHistoria::verificarLlavesCercanas()
{
    if (esperandoRespuesta || interactionBlocked) return;

    QGraphicsItem* playerItem = nullptr;
    for (QGraphicsItem* item : items()) {
        if (item->zValue() == 1000) {
            playerItem = item;
            break;
        }
    }
    if (!playerItem) return;

    QRectF playerRect = playerItem->sceneBoundingRect();

    bool llaveCercana = false;
    for (int i = 0; i < llaves.size(); i++) {
        if (llavesRecogidas[i]) continue;

        QRectF llaveRect = llaves[i]->sceneBoundingRect();
        llaveRect.adjust(-30, -30, 30, 30);

        if (playerRect.intersects(llaveRect)) {
            llaveActual = i;
            llaveCercana = true;

            if (!textoInteraccion) {
                textoInteraccion = new QGraphicsTextItem("Presiona E para interactuar");
                textoInteraccion->setDefaultTextColor(Qt::yellow);
                textoInteraccion->setFont(QFont("Minecraft", 12, QFont::Bold));
                textoInteraccion->setZValue(810);
                addObject(textoInteraccion);
            }

            QPointF playerPos = playerItem->scenePos();
            textoInteraccion->setPos(playerPos.x() - 80, playerPos.y() - 30);
            textoInteraccion->setVisible(true);

            break;
        }
    }

    if (!llaveCercana) {
        if (textoInteraccion) {
            textoInteraccion->setVisible(false);
        }
        llaveActual = -1;
    }
}

void JuegoHistoria::mostrarPregunta(int indiceLlave)
{
    if (indiceLlave < 0 || indiceLlave >= preguntas.size()) return;

    interactionBlocked = true;
    esperandoRespuesta = true;
    clearInteractiveAreas();

    QList<QString> preguntasList = preguntas.toList();
    QList<QStringList> opcionesList = opciones.toList();

    QString textoPregunta = preguntasList[indiceLlave];
    QStringList opcionesActuales = opcionesList[indiceLlave];

    barraPregunta = new QGraphicsRectItem(0, 0, 800, 250);
    barraPregunta->setBrush(QBrush(QColor(70, 23, 180, 255)));
    barraPregunta->setPen(QPen(Qt::white, 3));
    barraPregunta->setPos(0, 150);
    barraPregunta->setZValue(1100);
    addObject(barraPregunta);

    QString tituloTexto = QString("Pregunta");
    QGraphicsTextItem* titulo = new QGraphicsTextItem(tituloTexto);
    titulo->setDefaultTextColor(Qt::yellow);
    titulo->setFont(QFont("Minecraft", 14, QFont::Bold));
    titulo->setPos(25, 160);
    titulo->setZValue(1110);
    addObject(titulo);
    textosOpciones.append(titulo);

    textPregunta = new QGraphicsTextItem(textoPregunta);
    textPregunta->setDefaultTextColor(Qt::white);
    textPregunta->setFont(QFont("Minecraft", 11, QFont::Bold));
    textPregunta->setTextWidth(750);
    textPregunta->setPos(25, 190);
    textPregunta->setZValue(1110);
    addObject(textPregunta);

    QGraphicsLineItem* separador = new QGraphicsLineItem(25, 260, 775, 260);
    separador->setPen(QPen(Qt::yellow, 2));
    separador->setZValue(1105);
    addObject(separador);

    QGraphicsTextItem* dummySeparador = new QGraphicsTextItem();
    dummySeparador->setData(99, QVariant::fromValue(separador));
    textosOpciones.append(dummySeparador);

    int y = 275;
    for (const QString& opcion : opcionesActuales) {
        QGraphicsTextItem* textoOp = new QGraphicsTextItem(opcion);
        textoOp->setDefaultTextColor(Qt::yellow);
        textoOp->setFont(QFont("Minecraft", 11, QFont::Bold));
        textoOp->setPos(50, y);
        textoOp->setZValue(1110);
        addObject(textoOp);
        textosOpciones.append(textoOp);
        y += 25;
    }

    textTiempo = new QGraphicsTextItem();
    textTiempo->setPlainText("⏱ 20s");
    textTiempo->setDefaultTextColor(Qt::yellow);
    textTiempo->setFont(QFont("Minecraft", 14, QFont::Bold));
    textTiempo->setPos(700, 160);
    textTiempo->setZValue(1110);
    addObject(textTiempo);

    reiniciarTimer();
}

void JuegoHistoria::mostrarOpciones()
{
}

void JuegoHistoria::ocultarPreguntaYOpciones()
{
    if (barraPregunta) {
        removeItem(barraPregunta);
        delete barraPregunta;
        barraPregunta = nullptr;
    }

    if (textPregunta) {
        removeItem(textPregunta);
        delete textPregunta;
        textPregunta = nullptr;
    }

    if (textTiempo) {
        removeItem(textTiempo);
        delete textTiempo;
        textTiempo = nullptr;
    }

    for (QGraphicsTextItem* texto : textosOpciones) {
        if (texto) {
            QVariant lineData = texto->data(99);
            if (lineData.isValid()) {
                QGraphicsLineItem* line = qvariant_cast<QGraphicsLineItem*>(lineData);
                if (line) {
                    removeItem(line);
                    delete line;
                }
            }

            removeItem(texto);
            delete texto;
        }
    }
    textosOpciones.clear();
}

void JuegoHistoria::procesarRespuesta(const QString& respuesta)
{
    if (!esperandoRespuesta || llaveActual < 0) return;

    timer->stop();
    timerVisual->stop();

    QList<QString> respuestasCorrectasList = respuestasCorrectas.toList();
    QString correcta = respuestasCorrectasList[llaveActual];

    bool esCorrecta = (respuesta == correcta);

    QString resultado = esCorrecta ? "¡Respuesta correcta!" : "Respuesta incorrecta";

    if (textPregunta) {
        textPregunta->setPlainText(resultado);
        textPregunta->setDefaultTextColor(esCorrecta ? Qt::green : Qt::red);
    }

    if (esCorrecta) {
        recogerLlave(llaveActual);
        cantCorrectas++;

        if (llavesConseguidas >= 5) {
            if (textPregunta) {
                textPregunta->setPlainText("¡Felicidades! Todas las llaves conseguidas\nVe hacia la salida!");
                textPregunta->setDefaultTextColor(Qt::green);
            }
        }
    } else {
        llavesRecogidas[llaveActual] = true;
        llaves[llaveActual]->setVisible(false);
    }

    esperandoRespuesta = false;

    QTimer::singleShot(2000, this, [this]() {
        ocultarPreguntaYOpciones();
        interactionBlocked = false;
        llaveActual = -1;
    });
}

void JuegoHistoria::recogerLlave(int indice)
{
    if (indice >= 0 && indice < llaves.size()) {
        llavesRecogidas[indice] = true;
        llaves[indice]->setVisible(false);
        llavesConseguidas++;
        actualizarEstado();
    }
}

void JuegoHistoria::actualizarEstado()
{
    textoEstado->setPlainText(QString::number(llavesConseguidas) + "/5 llaves conseguidas");
}

void JuegoHistoria::actualizarTimer()
{
    if (time <= 0) {
        timerVisual->stop();
        return;
    }
    time--;
    if (textTiempo) {
        QColor color = (time <= 5) ? Qt::red : (time <= 10) ? QColor(255, 165, 0) : Qt::yellow;
        textTiempo->setDefaultTextColor(color);
        textTiempo->setPlainText(QString("⏱ %1s").arg(time));
    }
}

void JuegoHistoria::onTimeout()
{
    procesarRespuesta("TIMEOUT");
}

void JuegoHistoria::reiniciarTimer()
{
    time = 20;
    if (textTiempo) {
        textTiempo->setPlainText("⏱ 20s");
        textTiempo->setDefaultTextColor(Qt::yellow);
    }
    timer->stop();
    timerVisual->stop();

    timer->start(20000);
    timerVisual->start(1000);
}

void JuegoHistoria::terminarJuego()
{
    timer->stop();
    timerVisual->stop();
    ocultarPreguntaYOpciones();

    for (auto* llave : llaves) {
        llave->setVisible(false);
    }

    QString mensaje = "Ahora escapa! (si puedes...)";
    QGraphicsTextItem* resultado = new QGraphicsTextItem(mensaje);
    resultado->setDefaultTextColor(Qt::yellow);
    resultado->setFont(QFont("Minecraft", 24, QFont::Bold));
    resultado->setPos(250, 250);
    resultado->setZValue(813);
    addObject(resultado);
}

void JuegoHistoria::keyPressEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) return;

    if (esperandoRespuesta) {
        QString letra = "";
        switch(event->key()) {
        case Qt::Key_A: letra = "A"; break;
        case Qt::Key_B: letra = "B"; break;
        case Qt::Key_C: letra = "C"; break;
        case Qt::Key_D: letra = "D"; break;
        default: return;
        }

        procesarRespuesta(letra);
        return;
    }

    verificarLlavesCercanas();

    if (event->key() == Qt::Key_E) {
        if (llaveActual >= 0 && !llavesRecogidas[llaveActual]) {
            mostrarPregunta(llaveActual);
        }
        return;
    }

    QGraphicsScene::keyPressEvent(event);
}

void JuegoHistoria::verificarCambioEscena()
{

    if (esperandoRespuesta || interactionBlocked) return;

    QGraphicsItem* playerItem = nullptr;
    for (QGraphicsItem* item : items()) {
        if (item->zValue() == 1000) {
            playerItem = item;
            break;
        }
    }
    if (!playerItem) return;

    QPointF playerPos = playerItem->scenePos();

    QRectF areaCambio(posicionCambioEscena.x() - 60, posicionCambioEscena.y() - 100, 80, 40);

    if (areaCambio.contains(playerPos)) {
        bool todasRespondidas = true;
        for (bool respondida : llavesRecogidas) {
            if (!respondida) {
                todasRespondidas = false;
                break;
            }
        }

        if (todasRespondidas) {
            bool ganado = (llavesConseguidas == 5);


            emit juegoTerminado(ganado);
        } else {
            if (!textoSalida) {
                textoSalida = new QGraphicsTextItem();
                textoSalida->setDefaultTextColor(Qt::red);
                textoSalida->setFont(QFont("Minecraft", 12, QFont::Bold));
                textoSalida->setZValue(1120);
                addObject(textoSalida);
            }

            textoSalida->setPlainText("No has respondido las 5 preguntas");
            textoSalida->setPos(playerPos.x() - 120, playerPos.y() - 50);
            textoSalida->setVisible(true);

            QTimer::singleShot(2000, this, [this]() {
                if (textoSalida) {
                    textoSalida->setVisible(false);
                }
            });

        }
    }
}
