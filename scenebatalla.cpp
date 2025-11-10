#include "scenebatalla.h"
#include <QKeyEvent>
#include <QDebug>
#include <QFont>
#include <QBrush>
#include <QPen>
#include <QGraphicsPixmapItem>
#include <QRandomGenerator>
#include <QGraphicsOpacityEffect>
#include <QMediaPlayer>


SceneBatalla::SceneBatalla(Player* existingPlayer, Player::Bando bandoPlayer,int vidaInicialPlayer, QObject *parent)
    : SceneBase(parent)
    , player(existingPlayer)
    , bandoPlayer(bandoPlayer)
    , bandoNPC(bandoPlayer == Player::Bando::EMPIRISTA ? Player::Bando::RACIONALISTA : Player::Bando::EMPIRISTA)
    , vidaNPC(100)
    , vidaPlayer(vidaInicialPlayer)
    , vidaMaxima(100)
    , turnoPlayer(true)
    , esperandoRespuesta(false)
    , batallaTerminada(false)
    , preguntasRespondidasPlayer(0)
    , preguntasRespondidasNPC(0)
    , barraVidaPlayer(nullptr)
    , barraVidaNPC(nullptr)
    , textoVidaPlayer(nullptr)
    , textoVidaNPC(nullptr)
    , textoTurno(nullptr)
    , textoContador(nullptr)
    , barraPregunta(nullptr)
    , textPregunta(nullptr)
    , batallaTimer(new QTimer(this))
    , turnoTimer(new QTimer(this))
    , npc(nullptr)
    , verificandoEnProceso(false)
{
    setupPreguntas();
    loadBloodFrames();
    loadWaveFrames();
    loadScene();

    winPlayer = new QMediaPlayer(this);
    winAudio = new QAudioOutput(this);
    winPlayer->setAudioOutput(winAudio);
    winAudio->setVolume(0.8);
}
void SceneBatalla::loadScene()
{
    setSceneRect(0, 0, 800, 600);

    QPixmap bgPixmap("://res/batalla.png");
    if (bgPixmap.isNull()) {
        QPixmap bg(800, 600); bg.fill(QColor(30,30,60)); setBackground(bg);
    } else {
        setBackground(bgPixmap.scaled(800, 600));
    }

    npc = new NpcBatalla(bandoNPC, this);
    addItem(npc);
    const qreal yRef = npc->pos().y();        // Y del NPC para alinear al player

    player->cambiarBando(bandoPlayer);
    player->setZValue(10);
    player->setPos(580, yRef);                // ← MISMA ALTURA QUE EL NPC
    if (!items().contains(player)) addItem(player);

    connect(npc,    &NpcBatalla::attackCompleted, this, &SceneBatalla::onAtaqueNPCCompletado);
    connect(npc,    &NpcBatalla::bloodSplatter,   this, &SceneBatalla::onSangreLanzada);

    setupUI();

    batallaTimer->stop();
    turnoTimer->stop();
    connect(batallaTimer, &QTimer::timeout, this, &SceneBatalla::actualizarBatalla);
    connect(turnoTimer,   &QTimer::timeout, this, &SceneBatalla::procesarTurno);

    batallaTerminada = false;
    esperandoRespuesta = false;
    preguntasRespondidasPlayer = preguntasRespondidasNPC = 0;
    turnoPlayer = true;
    mensajeFinalMostrado = false;
    terminandoEnCurso = false;

    batallaTimer->start(100);
    QTimer::singleShot(1000, this, &SceneBatalla::mostrarPregunta);
}

void SceneBatalla::setupUI()
{
    QGraphicsRectItem* fondoBarraPlayer = new QGraphicsRectItem(500, 50, 200, 20);
    fondoBarraPlayer->setBrush(QBrush(QColor(100, 100, 100)));
    fondoBarraPlayer->setPen(QPen(Qt::white, 2));
    fondoBarraPlayer->setZValue(100);
    addItem(fondoBarraPlayer);

    QGraphicsRectItem* fondoBarraNPC = new QGraphicsRectItem(100, 50, 200, 20);
    fondoBarraNPC->setBrush(QBrush(QColor(100, 100, 100)));
    fondoBarraNPC->setPen(QPen(Qt::white, 2));
    fondoBarraNPC->setZValue(100);
    addItem(fondoBarraNPC);

    barraVidaPlayer = new QGraphicsRectItem(500, 50, 200, 20);
    barraVidaPlayer->setBrush(QBrush(QColor(0, 200, 0)));
    barraVidaPlayer->setPen(Qt::NoPen);
    barraVidaPlayer->setZValue(101);
    addItem(barraVidaPlayer);

    barraVidaNPC = new QGraphicsRectItem(100, 50, 200, 20);
    barraVidaNPC->setBrush(QBrush(QColor(200, 0, 0)));
    barraVidaNPC->setPen(Qt::NoPen);
    barraVidaNPC->setZValue(101);
    addItem(barraVidaNPC);

    textoVidaPlayer = new QGraphicsTextItem("Jugador: 100/100");
    textoVidaPlayer->setDefaultTextColor(Qt::white);
    textoVidaPlayer->setFont(QFont("Minecraft", 10, QFont::Bold));
    textoVidaPlayer->setPos(500, 25);
    textoVidaPlayer->setZValue(102);
    addItem(textoVidaPlayer);

    QString npcNombre = (bandoNPC == Player::Bando::EMPIRISTA) ? "Empirista" : "Racionalista";
    textoVidaNPC = new QGraphicsTextItem(npcNombre + ": 100/100");
    textoVidaNPC->setDefaultTextColor(Qt::white);
    textoVidaNPC->setFont(QFont("Minecraft", 10, QFont::Bold));
    textoVidaNPC->setPos(100, 25);
    textoVidaNPC->setZValue(102);
    addItem(textoVidaNPC);

    textoTurno = new QGraphicsTextItem("TU TURNO");
    textoTurno->setDefaultTextColor(Qt::yellow);
    textoTurno->setFont(QFont("Minecraft", 14, QFont::Bold));
    textoTurno->setPos(250, 80);
    textoTurno->setZValue(102);
    addItem(textoTurno);

    textoContador = new QGraphicsTextItem("Preguntas: Jugador 0/4 - NPC 0/4");
    textoContador->setDefaultTextColor(Qt::white);
    textoContador->setFont(QFont("Minecraft", 10, QFont::Bold));
    textoContador->setPos(250, 110);
    textoContador->setZValue(102);
    addItem(textoContador);
}

void SceneBatalla::loadBloodFrames()
{
    bloodFrames.clear();

    for (int i = 1; i <= 3; i++) {
        QString path = QString("://res/Objects/blood%1.png").arg(i);
        QPixmap frame(path);
        if (!frame.isNull()) {
            bloodFrames << frame.scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            qDebug() << "Blood frame" << i << "cargado correctamente";
        } else {
            qDebug() << "ERROR: No se pudo cargar blood frame" << i << "desde:" << path;
            QPixmap placeholder(80, 80);
            placeholder.fill(QColor(200, 0, 0, 180));
            bloodFrames << placeholder;
        }
    }

    qDebug() << "Total blood frames cargados:" << bloodFrames.size();
}

void SceneBatalla::setupPreguntas()
{
    preguntas.enqueue("Para algunos de los siguientes filósofos, el criterio de verdad es la evidencia sensible: ");
    respuestasCorrectas.enqueue("A");
    opciones.enqueue(QStringList{"A) Empiristas", "B) Criticistas", "C) Racionalistas", "D) Dogmáticos"});

    preguntas.enqueue("De las siguientes, una de ellas es la corriente filosófica que en general tiende a negar la "
                      "posibilidad de la metafísica y a sostener que hay conocimiento únicamente de los fenómenos");
    respuestasCorrectas.enqueue("B");
    opciones.enqueue(QStringList{"A) Racionalistas", "B) Empiristas", "C) Escolásticos", "D) Escépticos"});

    preguntas.enqueue("Para unos de los siguientes filósofos, la experiencia como única fuente del conocimiento");
    respuestasCorrectas.enqueue("C");
    opciones.enqueue(QStringList{"A) Epistemólogos", "B) Racionalistas", "C) Empiristas", "D) Escépticos"});

    preguntas.enqueue("Filósofos para quienes la única fuente del conocimiento es la razón");
    respuestasCorrectas.enqueue("B");
    opciones.enqueue(QStringList{"A) Epistemólogos", "B) Racionalistas", "C) Racionalistas", "D) Escépticos"});

    preguntas.enqueue("Filósofos que postulan las ideas innatas en el sujeto.");
    respuestasCorrectas.enqueue("C");
    opciones.enqueue(QStringList{"A) Empiristas", "B) Idealistas", "C) Racionalistas", "D) Innatistas"});

    preguntas.enqueue("De los siguientes filósofos selecciones el que no se considera Racionalista:");
    respuestasCorrectas.enqueue("C");
    opciones.enqueue(QStringList{"A) David Hume", "B) John Locke", "C) Nicolas Malebranch", "D) Francis Baco"});

    preguntas.enqueue("Es la doctrina que establece que todos nuestros conocimientos provienen de la razón. ");
    respuestasCorrectas.enqueue("C");
    opciones.enqueue(QStringList{"A) Empirismo", "B) Criticismo", "C) Racionalismo", "D) Epistemología"});

    preguntas.enqueue("Uno de los siguientes filósofos, postula las ideas innatas en el sujeto:");
    respuestasCorrectas.enqueue("C");
    opciones.enqueue(QStringList{"A) George Berkeley", "B) David Hume", "C) Leibniz", "D) Hipatía"});
}

void SceneBatalla::playWinMusicForBand(Player::Bando bando) {
    if (!winPlayer) return;

    QString path;
    if (bando == Player::Bando::EMPIRISTA) {
        path = "C:/Users/chung/OneDrive/Documents/Estru2Parcial1Qt/iron.mp3";
    } else if (bando == Player::Bando::RACIONALISTA) {
        path = "C:/Users/chung/OneDrive/Documents/Estru2Parcial1Qt/carmina.mp3";
    }

    if (!path.isEmpty()) {
        winPlayer->stop();
        winPlayer->setSource(QUrl::fromLocalFile(path));
        winPlayer->play();
        QTimer::singleShot(20000, this, [this](){ if (winPlayer) winPlayer->stop(); });
    }
}

void SceneBatalla::actualizarBatalla()
{
    if (batallaTerminada) return;

    double progresoPlayer = (vidaPlayer / (double)vidaMaxima) * 200;
    double progresoNPC = (vidaNPC / (double)vidaMaxima) * 200;

    barraVidaPlayer->setRect(500, 50, qMax(0.0, progresoPlayer), 20);
    barraVidaNPC->setRect(100, 50, qMax(0.0, progresoNPC), 20);

    textoVidaPlayer->setPlainText(QString("Jugador: %1/%2").arg(qMax(0, vidaPlayer)).arg(vidaMaxima));
    QString npcNombre = (bandoNPC == Player::Bando::EMPIRISTA) ? "Empirista" : "Racionalista";
    textoVidaNPC->setPlainText(QString("%1: %2/%3").arg(npcNombre).arg(qMax(0, vidaNPC)).arg(vidaMaxima));
    if (turnoPlayer) {
        textoTurno->setPlainText("TU TURNO - Responde con A/B/C/D");
        textoTurno->setDefaultTextColor(Qt::yellow);
    } else {
        textoTurno->setPlainText("TURNO DEL NPC - Pensando...");
        textoTurno->setDefaultTextColor(Qt::red);
    }

    textoContador->setPlainText(QString("Preguntas: Jugador %1/4 - NPC %2/4")
                                    .arg(preguntasRespondidasPlayer)
                                    .arg(preguntasRespondidasNPC));
}

void SceneBatalla::mostrarPregunta()
{
    if (esperandoRespuesta || batallaTerminada) {
        return;
    }

    alternarTurnoFijo();

    esperandoRespuesta = true;

    QString textoPregunta = preguntas.head();
    QStringList opcionesActuales = opciones.head();
    respuestaCorrectaActual = respuestasCorrectas.head();

    barraPregunta = new QGraphicsRectItem(0, 0, 800, 200);
    barraPregunta->setBrush(QBrush(QColor(70, 23, 180, 230)));
    barraPregunta->setPen(QPen(Qt::white, 3));
    barraPregunta->setPos(0, 400);
    barraPregunta->setZValue(1100);
    addItem(barraPregunta);

    textPregunta = new QGraphicsTextItem(textoPregunta);
    textPregunta->setDefaultTextColor(Qt::white);
    textPregunta->setFont(QFont("Minecraft", 10, QFont::Bold));
    textPregunta->setTextWidth(750);
    textPregunta->setPos(25, 420);
    textPregunta->setZValue(1110);
    addItem(textPregunta);

    int y = 470;
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

    if (turnoPlayer) {
        turnoTimer->start(TIEMPO_TURNO);
    } else {
        QTimer::singleShot(2000, this, &SceneBatalla::procesarRespuestaNPC);
    }
}

void SceneBatalla::ocultarPregunta()
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

    turnoTimer->stop();
    esperandoRespuesta = false;

}

void SceneBatalla::procesarRespuestaJugador(const QString& respuesta)
{
    if (!esperandoRespuesta || !turnoPlayer || batallaTerminada) {
        return;
    }

    bool esCorrecta = (respuesta == respuestaCorrectaActual);

    if (textPregunta) {
        if (esCorrecta) {
            textPregunta->setPlainText("¡Correcto! Preparando ataque...");
            textPregunta->setDefaultTextColor(Qt::green);
        } else {
            textPregunta->setPlainText("Incorrecto. El NPC atacará...");
            textPregunta->setDefaultTextColor(Qt::red);
        }
    }

    preguntas.dequeue();
    opciones.dequeue();
    respuestasCorrectas.dequeue();

    preguntasRespondidasPlayer++;

    QTimer::singleShot(1500, this, [this, esCorrecta]() {
        ocultarPregunta();

        if (!batallaTerminada) {
            if (esCorrecta) {
                qDebug() << "Jugador ataca al NPC";
                atacarJugador();
            } else {
                qDebug() << "NPC ataca al jugador";
                npc->startAttack();
            }
        }
    });
}

void SceneBatalla::procesarRespuestaNPC()
{
    if (!esperandoRespuesta || turnoPlayer || batallaTerminada) {
        return;
    }

    const int chanceAcierto = QRandomGenerator::global()->bounded(40, 91); // [40..90]
    const bool npcAcierta = QRandomGenerator::global()->bounded(100) < chanceAcierto;

    if (textPregunta) {
        if (npcAcierta) {
            textPregunta->setPlainText(QString("NPC responde: %1 - ¡CORRECTO! Atacará...").arg(respuestaCorrectaActual));
            textPregunta->setDefaultTextColor(Qt::red);
        } else {
            textPregunta->setPlainText("NPC responde: INCORRECTO. Tu turno continúa.");
            textPregunta->setDefaultTextColor(Qt::green);
        }
    }

    preguntas.dequeue();
    opciones.dequeue();
    respuestasCorrectas.dequeue();

    preguntasRespondidasNPC++;

    QTimer::singleShot(2000, this, [this, npcAcierta]() {
        ocultarPregunta();

        if (!batallaTerminada) {
            if (npcAcierta) {
                npc->startAttack();
            } else {
                mostrarPregunta();
            }
        }
    });
}

void SceneBatalla::atacarJugador()
{
    qDebug() << "Iniciando ataque del jugador contra NPC";
    player->playAttackAnimation();

    QObject::connect(player, &Player::animationFinished, this,
                     [this](){ this->onAtaquePlayerCompletado(); },
                     Qt::SingleShotConnection);
    QTimer::singleShot(250, this, [this]() {
        QPointF start = player->pos()
                        + QPointF(-20, player->boundingRect().height() * -0.1); // hacia la izquierda y centro vertical
        crearEfectoOnda(start);
    });
}


void SceneBatalla::atacarNPC()
{
    vidaNPC -= DANIO_ATAQUE;

    if (vidaNPC <= 0) {
        vidaNPC = 0;
        qDebug() << "NPC derrotado! Vida restante:" << vidaNPC;

        npc->playDeathAnimation();

        QTimer::singleShot(800, this, [this]() {
            terminarBatalla(true); // Jugador gana
        });
    } else {
        qDebug() << "NPC recibe daño. Vida restante:" << vidaNPC;
        npc->takeDamage();
    }

    actualizarBatalla();
}

void SceneBatalla::terminarBatalla(bool ganoPlayer)
{
    if (terminandoEnCurso || batallaTerminada) {
        return;
    }

    terminandoEnCurso = true;
    batallaTerminada = true;

    batallaTimer->stop();
    turnoTimer->stop();
    ocultarPregunta();

    disconnect(player, &Player::animationFinished, this, nullptr);
    if (npc) {
        disconnect(npc, &NpcBatalla::deathCompleted, this, nullptr);
        disconnect(npc, &NpcBatalla::attackCompleted, this, nullptr);
    }

    if (ganoPlayer) {
        player->resetAnimation();

        playWinMusicForBand(bandoPlayer);
        QTimer::singleShot(1000, this, [this]() {
            mostrarMensajeFinal(true);
        });
    } else {
        vidaPlayer = 0;
        actualizarBatalla();
        player->playDisintegrateAnimation();
        playWinMusicForBand(bandoNPC);

        connect(player, &Player::animationFinished, this, [this]() {
            mostrarMensajeFinal(false);
        }, Qt::SingleShotConnection);

        QTimer::singleShot(2000, this, [this]() {
            mostrarMensajeFinal(false);
        });
    }


    terminandoEnCurso = false;
}

void SceneBatalla::mostrarMensajeFinal(bool ganoPlayer)
{
    if (mensajeFinalMostrado) return;
    mensajeFinalMostrado = true;

    ocultarPregunta();

    QGraphicsRectItem* fondoMensaje = new QGraphicsRectItem(0, 0, 800, 150);
    fondoMensaje->setBrush(QBrush(QColor(0, 0, 0, 200)));
    fondoMensaje->setPos(0, 225);
    fondoMensaje->setZValue(1150);
    addItem(fondoMensaje);

    QGraphicsTextItem* mensaje = new QGraphicsTextItem();
    mensaje->setDefaultTextColor(ganoPlayer ? Qt::green : Qt::red);
    mensaje->setFont(QFont("Minecraft", 20, QFont::Bold));
    mensaje->setTextWidth(700);
    mensaje->setZValue(1160);
    mensaje->setPos(50, 250);
    addItem(mensaje);

    if (ganoPlayer) {
        mensaje->setPlainText("Victoria!\nHas demostrado la superioridad de tu bando.");
    } else {
        mensaje->setPlainText("Derrota...\nTu oponente ha refutado tus argumentos.");
    }

    QTimer::singleShot(20000, this, [this, ganoPlayer]() {
        if(winPlayer) winPlayer->stop();
        emit juegoTerminado(ganoPlayer);
    });
}

void SceneBatalla::onAtaqueNPCCompletado()
{
    vidaPlayer -= DANIO_ATAQUE;

    if (vidaPlayer <= 0) {
        vidaPlayer = 0;
        actualizarBatalla();
    }

    player->playHurtAnimation();
    verificarGanador();

    if (!batallaTerminada) {
        QTimer::singleShot(1500, this, [this]() {
            if (batallaTerminada) return;

            const bool quedanCupos = (preguntasRespondidasPlayer < MAX_PREGUNTAS_POR_JUGADOR ||
                                      preguntasRespondidasNPC   < MAX_PREGUNTAS_POR_JUGADOR);
            if (!preguntas.isEmpty() && quedanCupos) {
                mostrarPregunta();
            } else {
                verificarGanador();
            }
        });
}
}
void SceneBatalla::onAtaquePlayerCompletado()
{
    atacarNPC();

    if (vidaNPC > 0 && !batallaTerminada) {
        QTimer::singleShot(1500, this, [this]() {
            if (batallaTerminada) return;

            const bool quedanCupos = (preguntasRespondidasPlayer < MAX_PREGUNTAS_POR_JUGADOR ||
                                      preguntasRespondidasNPC   < MAX_PREGUNTAS_POR_JUGADOR);
            if (!preguntas.isEmpty() && quedanCupos) {
                mostrarPregunta();
            } else {
                verificarGanador();
            }
        });
    }
}

void SceneBatalla::procesarTurno()
{
    if (turnoPlayer && esperandoRespuesta) {
        procesarRespuestaJugador("X");
    }
}

void SceneBatalla::onSangreLanzada(QPointF posicion)
{
    crearEfectoSangre(posicion);
}

void SceneBatalla::crearEfectoSangre(QPointF posicion)
{
    if (bloodFrames.isEmpty()) return;

    auto* sangre = new QGraphicsPixmapItem(bloodFrames[0]);
    sangre->setPos(posicion.x(),posicion.y()-120);
    sangre->setZValue(2000);
    addItem(sangre);

    auto* sangreTimer = new QTimer(this);

    connect(sangreTimer, &QTimer::timeout, this,
            [this, sangre, sangreTimer, frameIndex = 0]() mutable {
                if (frameIndex < bloodFrames.size()) {
                    sangre->setPixmap(bloodFrames[frameIndex]);
                    ++frameIndex;
                } else {
                    sangreTimer->stop();
                    sangreTimer->deleteLater();
                    removeItem(sangre);
                    delete sangre;
                }
            });

    sangreTimer->start(100);
}


void SceneBatalla::verificarGanador()
{
    if (batallaTerminada) return;

    if (verificandoEnProceso) return;
    verificandoEnProceso = true;

    bool jugadorGana = false;
    bool npcGana = false;

    if (vidaPlayer <= 0) {
        vidaPlayer = 0;
        npcGana = true;
        qDebug() << "NPC gana por vida del jugador agotada";
    }
    else if (vidaNPC <= 0) {
        vidaNPC = 0;
        jugadorGana = true;
        qDebug() << "Jugador gana por vida del NPC agotada";
    }
    else if (preguntasRespondidasPlayer >= MAX_PREGUNTAS_POR_JUGADOR &&
             preguntasRespondidasNPC >= MAX_PREGUNTAS_POR_JUGADOR) {
        if (vidaPlayer > vidaNPC) {
            jugadorGana = true;
            qDebug() << "Jugador gana por mayor vida después de preguntas";
        } else if (vidaNPC > vidaPlayer) {
            npcGana = true;
            qDebug() << "NPC gana por mayor vida después de preguntas";
        } else {
            jugadorGana = true;
            qDebug() << "Empate - NPC gana por defecto";
        }
    }

    verificandoEnProceso = false;

    if (jugadorGana) {
        qDebug() << "Terminando batalla - Jugador gana";
        terminarBatalla(true);
    } else if (npcGana) {
        qDebug() << "Terminando batalla - NPC gana";
        terminarBatalla(false);
    }
}
void SceneBatalla::keyPressEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) return;

    if(batallaTerminada){
        event->accept();
        return;
    }

    if (esperandoRespuesta && turnoPlayer) {
        QString letra = "";
        switch(event->key()) {
        case Qt::Key_A: letra = "A"; break;
        case Qt::Key_B: letra = "B"; break;
        case Qt::Key_C: letra = "C"; break;
        case Qt::Key_D: letra = "D"; break;
        default: return;
        }

        procesarRespuestaJugador(letra);
        return;
    }

    QGraphicsScene::keyPressEvent(event);
}

void SceneBatalla::alternarTurnoFijo()
{
    int totalPreguntasRespondidas = preguntasRespondidasPlayer + preguntasRespondidasNPC;

    if (totalPreguntasRespondidas % 2 == 0) {
        turnoPlayer = true;
    } else {
        turnoPlayer = false;
    }
}

void SceneBatalla::loadWaveFrames()
{
    waveFrames.clear();
    for (int i = 1; i <= 3; ++i) {
        QString path = QString("://res/Objects/wave%1.png").arg(i); // ajusta nombres
        QPixmap frame(path);
        if (!frame.isNull()) {
            waveFrames << frame.scaled(90, 90, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        } else {
            QPixmap ph(90, 90); ph.fill(QColor(255,255,255,180));
            waveFrames << ph;
        }
    }
}

void SceneBatalla::crearEfectoOnda(QPointF posicion)
{
    if (waveFrames.isEmpty()) return;

    auto* onda = new QGraphicsPixmapItem(waveFrames[0]);
    onda->setPos(posicion);
    onda->setZValue(2000);
    addItem(onda);

    auto* ondaTimer = new QTimer(this);
    connect(ondaTimer, &QTimer::timeout, this,
            [this, onda, ondaTimer, frameIndex = 0]() mutable {
                if (frameIndex < waveFrames.size()) {
                    onda->setPixmap(waveFrames[frameIndex]);
                    ++frameIndex;
                    onda->moveBy(-15, -2);
                } else {
                    ondaTimer->stop();
                    ondaTimer->deleteLater();
                    removeItem(onda);
                    delete onda;
                }
            });
    ondaTimer->start(90);
}

void SceneBatalla::debugGanarBatalla()
{
    if (batallaTerminada) return;

    turnoTimer->stop();
    batallaTimer->stop();
    ocultarPregunta();

    preguntasRespondidasPlayer = qMax(preguntasRespondidasPlayer, MAX_PREGUNTAS_POR_JUGADOR);

    vidaNPC = 0;
    actualizarBatalla();

    if (npc) {
        npc->playDeathAnimation();
        QTimer::singleShot(500, this, [this]() {
            terminarBatalla(true);
        });
    } else {
        terminarBatalla(true);
    }
}
