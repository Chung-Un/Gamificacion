#include "juegociencia.h"
#include <QKeyEvent>
#include <QDebug>
#include <QFont>
#include <QBrush>
#include "Player.h"
#include <QPen>

const double JuegoCiencia::META_X = 740.0;

JuegoCiencia::JuegoCiencia(QObject *parent)
    : SceneBase(parent)
    , runningMan(nullptr)
    , playerPosX(50)
    , playerVelocidadBase(2.0)
    , playerVelocidadActual(2.0)
    , playerConBoost(false)
    , boostDuracion(0)
    , respuestasCorrectasCount(0)
    , preguntaActual(0)
    , esperandoRespuesta(false)
    , respuestasMalas(0)
    , barraPregunta(nullptr)
    , textPregunta(nullptr)
    , carreraTimer(new QTimer(this))
    , preguntaTimer(new QTimer(this))
    , juegoIniciado(false)
    , terminado(false)
    , textoDistanciaPlayer(nullptr)
    , textoDistanciaRival(nullptr)
    , barraProgresoPlayer(nullptr)
    , barraProgresoRival(nullptr)
{
    preguntas.enqueue("Entre los siguientes renacentistas seleccione, uno de los precursores filósofo-científico del heliocentrismo "
    "(teoría que afirma que el sol es el centro del universo): ");
    respuestasCorrectas.enqueue("B");
    opciones.enqueue(QStringList{"A) Tomas Moro", "B) Galileo ", "C) Platón", "D) Arquimedes"});

    preguntas.enqueue("El método científico se introduce por el interés de tres filósofos. Entre los siguientes uno de los "
    "mencionados no es precursor del método científico: ");
    respuestasCorrectas.enqueue("C");
    opciones.enqueue(QStringList{"A) Francis Bacon ", "B) Galileo Galilei", "C) Nicolas Maquiavelo", "D) René Descartes"});

    preguntas.enqueue("Es uno de los precursores del pensamiento Moderno: ");
    respuestasCorrectas.enqueue("B");
    opciones.enqueue(QStringList{"A) Isaac Newton", "B) René Descartes", "C) Erasmo de Roterdam", "D) Francis Bacon"});

    preguntas.enqueue("De los siguientes filósofos niega el geocentrismo (teoría que afirma que el centro de nuestro sistema solar"
                     " es la tierra):");
    respuestasCorrectas.enqueue("B");
    opciones.enqueue(QStringList{"A) Aristóteles", "B) Nicolás Copérnico", "C) Tomás de Aquin", "D) Isaac Newton"});

    preguntas.enqueue(" Uno de los inventos que suscitó un conocimiento ilimitado, fue el de Gutenberg:");
    respuestasCorrectas.enqueue("B");
    opciones.enqueue(QStringList{"A) El astrolabio", "B) La imprenta", "C) La Nao y la Carabela", "D)  El Telescopio"});

    loadScene();
}

void JuegoCiencia::loadScene()
{
    setSceneRect(0, 0, 800, 600);

    QPixmap bgPixmap("://res/pista.png");
    if (bgPixmap.isNull()) {
        QPixmap bg(800, 600);
        bg.fill(QColor(135, 206, 235));
        setBackground(bg);
    } else {
        setBackground(bgPixmap.scaled(800, 600));
    }

    runningMan = new RunningMan(this);
    runningMan->setPos(50, 400);
    runningMan->setVelocidad(2.2);
    addItem(runningMan);

    QGraphicsRectItem* fondoBarraPlayer = new QGraphicsRectItem(50, 50, 700, 20);
    fondoBarraPlayer->setBrush(QBrush(QColor(100, 100, 100)));
    fondoBarraPlayer->setPen(QPen(Qt::white, 2));
    fondoBarraPlayer->setZValue(100);
    addItem(fondoBarraPlayer);

    barraProgresoPlayer = new QGraphicsRectItem(50, 50, 0, 20);
    barraProgresoPlayer->setBrush(QBrush(QColor(0, 200, 0)));
    barraProgresoPlayer->setPen(Qt::NoPen);
    barraProgresoPlayer->setZValue(101);
    addItem(barraProgresoPlayer);

    textoDistanciaPlayer = new QGraphicsTextItem("Tú: 0m");
    textoDistanciaPlayer->setDefaultTextColor(Qt::white);
    textoDistanciaPlayer->setFont(QFont("Minecraft", 12, QFont::Bold));
    textoDistanciaPlayer->setPos(50, 25);
    textoDistanciaPlayer->setZValue(102);
    addItem(textoDistanciaPlayer);

    QGraphicsRectItem* fondoBarraRival = new QGraphicsRectItem(50, 90, 700, 20);
    fondoBarraRival->setBrush(QBrush(QColor(100, 100, 100)));
    fondoBarraRival->setPen(QPen(Qt::white, 2));
    fondoBarraRival->setZValue(100);
    addItem(fondoBarraRival);

    barraProgresoRival = new QGraphicsRectItem(50, 90, 0, 20);
    barraProgresoRival->setBrush(QBrush(QColor(200, 0, 0)));
    barraProgresoRival->setPen(Qt::NoPen);
    barraProgresoRival->setZValue(101);
    addItem(barraProgresoRival);

    textoDistanciaRival = new QGraphicsTextItem("Rival: 0m");
    textoDistanciaRival->setDefaultTextColor(Qt::white);
    textoDistanciaRival->setFont(QFont("Minecraft", 12, QFont::Bold));
    textoDistanciaRival->setPos(50, 65);
    textoDistanciaRival->setZValue(102);
    addItem(textoDistanciaRival);

    connect(carreraTimer, &QTimer::timeout, this, &JuegoCiencia::actualizarCarrera);
    connect(preguntaTimer, &QTimer::timeout, this, &JuegoCiencia::mostrarPregunta);

    iniciarCarrera();
}

void JuegoCiencia::iniciarCarrera()
{
    if (juegoIniciado){
        return;
    }

    juegoIniciado = true;
    runningMan->startRunning();

    carreraTimer->start(50);
    preguntaTimer->start(TIEMPO_PREGUNTA);

}

void JuegoCiencia::actualizarCarrera()
{
    if (terminado) return;

    Player* player = nullptr;
    for (QGraphicsItem* item : items()) {
        player = dynamic_cast<Player*>(item);
        if (player) {
            break;
        }
    }

    if (player) {
        playerPosX += playerVelocidadActual;

        player->setPos(playerPosX, 300);

        static int debugCounter = 0;
        if (debugCounter++ % 20 == 0) {

        }

        if (playerVelocidadActual > 0) {
            if (player->getCurrentAnimType() != Player::AnimationType::RUN) {
                player->startRunning();
            }
        } else {
            if (player->getCurrentAnimType() == Player::AnimationType::RUN) {
                player->stopRunning();
            }
        }

        if (playerConBoost) {
            boostDuracion--;
            if (boostDuracion <= 0) {
                playerConBoost = false;
                playerVelocidadActual = playerVelocidadBase;
            }
        }

        double progresoPlayer = qMin((playerPosX / META_X) * 700, 700.0);
        barraProgresoPlayer->setRect(50, 50, progresoPlayer, 20);
        textoDistanciaPlayer->setPlainText(QString("Tú: %1m").arg((int)(playerPosX)));
    } else {
        static int errorCounter = 0;
        if (errorCounter++ % 50 == 0) {
        }
    }

    runningMan->mover();
    double rivalPosX = runningMan->getPosicionX();

    double progresoRival = qMin((rivalPosX / META_X) * 700, 700.0);
    barraProgresoRival->setRect(50, 90, progresoRival, 20);
    textoDistanciaRival->setPlainText(QString("Rival: %1m").arg((int)(rivalPosX)));

    verificarGanador();
}
void JuegoCiencia::mostrarPregunta()
{
    if (esperandoRespuesta || terminado) return;
    if (preguntas.isEmpty()) {
        preguntaTimer->stop();
        return;
    }

    esperandoRespuesta = true;

    preguntaTimer->stop();

    carreraTimer->stop();
    runningMan->stopRunning();

    Player* player = nullptr;
    for (QGraphicsItem* item : items()) {
        player = dynamic_cast<Player*>(item);
        if (player) break;
    }

    if (player) {
        player->stopRunning();
    }

    QString textoPregunta = preguntas.head();
    QStringList opcionesActuales = opciones.head();
    QString correcta = respuestasCorrectas.head();

    barraPregunta = new QGraphicsRectItem(0, 0, 800, 250);
    barraPregunta->setBrush(QBrush(QColor(70, 23, 180, 230)));
    barraPregunta->setPen(QPen(Qt::white, 3));
    barraPregunta->setPos(0, 175);
    barraPregunta->setZValue(1100);
    addItem(barraPregunta);

    QGraphicsTextItem* titulo = new QGraphicsTextItem("¡Pregunta de Ciencia!");
    titulo->setDefaultTextColor(Qt::yellow);
    titulo->setFont(QFont("Minecraft", 16, QFont::Bold));
    titulo->setPos(250, 185);
    titulo->setZValue(1110);
    addItem(titulo);
    textosOpciones.append(titulo);

    textPregunta = new QGraphicsTextItem(textoPregunta);
    textPregunta->setDefaultTextColor(Qt::white);
    textPregunta->setFont(QFont("Minecraft", 12, QFont::Bold));
    textPregunta->setTextWidth(750);
    textPregunta->setPos(25, 225);
    textPregunta->setZValue(1110);
    addItem(textPregunta);

    int y = 280;
    for (const QString& opcion : opcionesActuales) {
        QGraphicsTextItem* textoOp = new QGraphicsTextItem(opcion);
        textoOp->setDefaultTextColor(Qt::yellow);
        textoOp->setFont(QFont("Minecraft", 11, QFont::Bold));
        textoOp->setPos(50, y);
        textoOp->setZValue(1110);
        addItem(textoOp);
        textosOpciones.append(textoOp);
        y += 30;
    }

    respuestaCorrectaActual = correcta;

}
void JuegoCiencia::procesarRespuesta(const QString& respuesta)
{
    if (!esperandoRespuesta) return;

    bool esCorrecta = (respuesta == respuestaCorrectaActual);

    if (textPregunta) {
        if (esCorrecta) {
            textPregunta->setPlainText("¡Correcto! +Velocidad");
            textPregunta->setDefaultTextColor(Qt::green);
            respuestasCorrectasCount++;
        } else {
            textPregunta->setPlainText("Incorrecto. -Velocidad");
            textPregunta->setDefaultTextColor(Qt::red);
            respuestasMalas++;

            playerVelocidadBase = qMax(0.5, playerVelocidadBase * 0.7);
            if (!playerConBoost) {
                playerVelocidadActual = playerVelocidadBase;
            }
        }
    }

    if (esCorrecta) {
        darBoostAlPlayer();
    }

    preguntas.dequeue();
    opciones.dequeue();
    respuestasCorrectas.dequeue();

    preguntaActual++;
    esperandoRespuesta = false;

    QTimer::singleShot(1500, this, [this]() {
        ocultarPregunta();

        if (!terminado) {
            carreraTimer->start(50);
            runningMan->startRunning();

            Player* player = nullptr;
            for (QGraphicsItem* item : items()) {
                player = dynamic_cast<Player*>(item);
                if (player) break;
            }

            if (player && playerVelocidadActual > 0) {
                player->startRunning();
            }

            if (!preguntas.isEmpty() && !terminado) {
                preguntaTimer->start(TIEMPO_PREGUNTA);
            }


            if (respuestasCorrectasCount >= 5 && !playerConBoost) {
                darBoostAlPlayer();
                playerVelocidadActual *= 1.5;
            }
        }
    });
}

void JuegoCiencia::darBoostAlPlayer()
{
    playerConBoost = true;
    playerVelocidadActual = playerVelocidadBase * 2.0;
    boostDuracion = 60;

}

void JuegoCiencia::verificarGanador()
{
    if (terminado) return;

    if (playerPosX >= META_X) {
        if (respuestasCorrectasCount >= 5) {
            terminarJuego(true);
        } else {
            terminarJuego(false);
        }
        return;
    }

    if (runningMan->getPosicionX() >= META_X) {
        terminarJuego(false);
        return;
    }
}

void JuegoCiencia::terminarJuego(bool gano)
{
    if (terminado) return;

    terminado = true;
    carreraTimer->stop();
    preguntaTimer->stop();
    runningMan->stopRunning();
    ocultarPregunta();

    for (QGraphicsItem* item : items()) {
        if (item->zValue() == 1000) {
            Player* player = dynamic_cast<Player*>(item);
            if (player) {
                player->stopRunning();
            }
            break;
        }
    }

    QGraphicsRectItem* fondoMensaje = new QGraphicsRectItem(0, 0, 800, 150);
    fondoMensaje->setBrush(QBrush(QColor(0, 0, 0, 200)));
    fondoMensaje->setPos(0, 225);
    fondoMensaje->setZValue(1150);
    addItem(fondoMensaje);

    QGraphicsTextItem* mensaje = new QGraphicsTextItem();
    mensaje->setDefaultTextColor(gano ? Qt::green : Qt::red);
    mensaje->setFont(QFont("Minecraft", 24, QFont::Bold));
    mensaje->setTextWidth(700);
    mensaje->setZValue(1160);
    mensaje->setPos(50, 260);
    addItem(mensaje);

    if (gano) {
        mensaje->setPlainText(QString("¡Ganaste la carrera!\nDominaste la ciencia renacentista.\nRespuestas correctas: %1/5").arg(respuestasCorrectasCount));
    } else {
        if (playerPosX >= META_X && respuestasCorrectasCount < 5) {
            mensaje->setPlainText(QString("Llegaste a la meta pero...\nNecesitabas 5 respuestas correctas.\nSolo tuviste: %1/5").arg(respuestasCorrectasCount));
        } else {
            mensaje->setPlainText(QString("Perdiste la carrera...\nRespuestas correctas: %1/5").arg(respuestasCorrectasCount));
        }
    }

    QTimer::singleShot(3000, this, [this, gano]() {
        emit juegoTerminado(gano);
    });
}

void JuegoCiencia::keyPressEvent(QKeyEvent *event)
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

    QGraphicsScene::keyPressEvent(event);
}

void JuegoCiencia::ocultarPregunta()
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

    for (QGraphicsTextItem* texto : textosOpciones) {
        if (texto) {
            removeItem(texto);
            delete texto;
        }
    }
    textosOpciones.clear();

}
