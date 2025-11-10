#include "game.h"
#include <QApplication>
#include <QDebug>
#include "scenebatalla.h"
#include <QInputDialog>
#include <QMessageBox>

Game::Game(QWidget *parent)
    : QMainWindow(parent)
    , player(new Player(this))
    , currentScene(new StartScene(this))
    , view(nullptr)
    , movementTimer(new QTimer(this))
    , playerX(400), playerY(500)
    , movingUp(false), movingDown(false), movingLeft(false), movingRight(false)
    , interactionText(new QGraphicsTextItem())
    , isInteracting(false)
    , mapa(nullptr)
    , movimientoHabilitado(false)
    ,combateActual(nullptr)
{

    if(currentScene->name=="StartScene" || currentScene->name== "JuegoArte" || currentScene->name ==
        "juegoHistoria"){
            currentScene->setSceneRect(0, 0, 800, 600);
        }
    else{
        currentScene->setSceneRect(0, 0, 1000, 1000);

        }



    const QString sceneName = currentScene->getSceneName();
    if (sceneName != "StartScene" && sceneName != "Mapa" && sceneName != "VideoScene") {
        if (!currentScene->items().contains(player)) {
            player->setPos(playerX, playerY);
            player->setZValue(1000);
            player->setVisible(true);
            currentScene->addItem(player);
        }
    }

    setupUI(); // ahora interactionText existe

    // View
    view = new QGraphicsView(currentScene, this);
    view->setMouseTracking(true);
    view->viewport()->setAttribute(Qt::WA_AcceptTouchEvents, false);
    view->setFixedSize(800, 600);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setAlignment(Qt::AlignCenter);
    setCentralWidget(view);


    // --- StartScene → pedir nombre y saltar a MAPA ---
    if (auto* start = qobject_cast<StartScene*>(currentScene)) {
        connect(start, &StartScene::pedirNombreUsuario, this, [this](){
            bool ok = false;
            const QString nombre = QInputDialog::getText(
                this, "Usuario", "Ingresa tu nombre:", QLineEdit::Normal, "", &ok);
            if (ok && !nombre.trimmed().isEmpty()) {
                nombreUsuario = nombre.trimmed();
                qDebug() << "Usuario:" << nombreUsuario;
            }
        });

        connect(start, &StartScene::pedirTipoPartida, this, [this](){
            // 1) ir a mapa (persistente)
            mapa = new Mapa(this);
            cambiarEscena(mapa);
            movimientoHabilitado = false;         // En mapa no usamos WASD

            // 2) desbloquear L1
            mapa->setUnlocked(QSet<QString>() << "L1");

            // 3) clicks del mapa
            connect(mapa, &Mapa::levelChosen, this, [this](const QString& node){
                if (node == "L1") {
                    auto* video = new VideoScene(this);
                    cambiarEscena(video);
                    connect(video, &VideoScene::videoTerminado, this, [this](){
                        cambiarEscena(mapa);
                        mapa->unlock("L1");
                        mapa->unlockVecinos("L1"); // habilita L2
                        movimientoHabilitado = false;
                    });
                }
                else if (node == "L2") {
                    auto* ruleta = new RuletaScene(this);
                    cambiarEscena(ruleta);
                    movimientoHabilitado = true; // si dentro de ruleta quieres moverte
                    // La conexión Ruleta -> Mapa se hace dentro de cambiarEscena(...)
                }
                else if (node == "L3") {
                   iniciarBatallaL3(vidaParaL3);
                }
                else if (node == "L4") {
                    auto* camino = new CaminoUni(this);
                    // ⏱️ INICIO cronómetro L4
                    startLevelTimer();

                    cambiarEscena(camino, QPointF(530,200));
                    movimientoHabilitado = true;

                    // (El connect a CaminoUni::caminoTerminado se elimina)
                }


            });
        });
    }

    // centrado inicial (si procede)
    if (sceneName != "StartScene" && sceneName != "Mapa" && sceneName != "VideoScene")
        view->centerOn(player);
    else
        view->centerOn(currentScene->sceneRect().center());

    // Movimiento
    connect(movementTimer, &QTimer::timeout, this, &Game::updateMovement);
    movementTimer->start(50);

    // Si por alguna razón la escena inicial fuera Ruleta, conecta Ruleta → Mapa
    if (auto* ruletaScene = qobject_cast<RuletaScene*>(currentScene)) {
        QObject::disconnect(ruletaScene, nullptr, this, nullptr);
        connect(ruletaScene, &RuletaScene::irAlMapaTrasBando, this,
                [this](bool /*esEmpirista*/, int /*vidaInicial*/){
                    if (mapa) cambiarEscena(mapa);
                    movimientoHabilitado = false;
                });
    }
}


Game::~Game()
{
}

void Game::setupUI()
{
    if (!interactionText) interactionText = new QGraphicsTextItem();
    interactionText->setDefaultTextColor(Qt::white);
    interactionText->setFont(QFont("Minecraft", 14, QFont::Bold));
    interactionText->setZValue(1000);
    interactionText->setVisible(false);
    if (!currentScene->items().contains(interactionText))
        currentScene->addItem(interactionText);
}

void Game::iniciarBatallaL3(int vidaInicial)
{
    if (combateActual) {
        combateActual->disconnect(this);
        if (currentScene && currentScene->items().contains(player))
            currentScene->removeItem(player);
        combateActual->deleteLater();
        combateActual = nullptr;
    }

    // Reinicio visual del jugador
    player->resetAnimation();
    player->setVisible(true);
    player->setOpacity(1.0);
    player->setZValue(1000);

    if (currentScene && currentScene->items().contains(player))
        currentScene->removeItem(player);

    // Crear batalla nueva
    Player::Bando bando = player->bandoActual;

    // ⏱️ INICIO cronómetro L3
    startLevelTimer();

    combateActual = new SceneBatalla(player, bando, vidaInicial, this);
    cambiarEscena(combateActual);
    movimientoHabilitado = false;

    // Conexión única
    connect(combateActual, &SceneBatalla::juegoTerminado, this,
            [this](bool gano) {
                // ⏱️ FIN cronómetro L3
                stopLevelTimerAndStore();

                const qint64 ms = tiemposMs.isEmpty() ? -1 : tiemposMs.back();
                if (ms >= 0) {
                    StatsFile::upsertRun(
                        nombreUsuario.isEmpty() ? QString("NN") : nombreUsuario,
                        (player->bandoActual == Player::Bando::EMPIRISTA) ? 0 : 1,
                        /*level*/ 3,
                        double(ms)
                        );
                }

                SceneBatalla* finalizada = combateActual;
                combateActual = nullptr;

                // Quitar SIEMPRE al player de la escena que se va a destruir (evita crashes)
                if (finalizada && finalizada->items().contains(player)) {
                    finalizada->removeItem(player);
                }

                // Destruir la escena de batalla
                if (finalizada) finalizada->deleteLater();

                // Volver al MAPA SIEMPRE (ganes o pierdas)
                if (mapa) {
                    cambiarEscena(mapa);
                    movimientoHabilitado = false; // en mapa no usamos WASD

                    // SOLO si gana, desbloquear L4
                    if (gano) {
                        mapa->unlock("L4");
                        // mapa->unlockVecinos("L3");
                    }
                }
            });
}


void Game::updateMovement()
{

    if(!movimientoHabilitado)return;

    Player::Direction currentDirection = Player::Direction::IDLE;

    if (movingUp) currentDirection = Player::Direction::UP;
    else if (movingDown) currentDirection = Player::Direction::DOWN;
    else if (movingLeft) currentDirection = Player::Direction::LEFT;
    else if (movingRight) currentDirection = Player::Direction::RIGHT;

    if(!movingUp && !movingDown && !movingLeft && !movingRight) {
        player->setAnimation(false, currentDirection);
        checkInteractions();
        return;
    }

    player->setAnimation(true, currentDirection);

    double step = 15;
    double newX = playerX;
    double newY = playerY;

    if (movingUp) newY -= step;
    if (movingDown) newY += step;
    if (movingLeft) newX -= step;
    if (movingRight) newX += step;

    newX = qMax(30.0, qMin(newX, 970.0));
    newY = qMax(30.0, qMin(newY, 970.0));

    double originalX = playerX;
    double originalY = playerY;

    playerX = newX;
    playerY = newY;

    checkCollisions();

    QSize playerSize = player->getCurrentSize();
    QRectF playerRect(playerX,playerY,playerSize.width(),playerSize.height());

    for (const QRectF &obstacle : currentScene->getCollisionAreas()) {
        if (playerRect.intersects(obstacle)) {
            playerX = originalX;
            playerY = originalY;
            break;
        }
    }

    player->setPos(playerX, playerY);

    checkInteractions();

    if(isInteracting){
        double textX = playerX - (interactionText->boundingRect().width()/2);
        double textY = playerY - 30;
        interactionText->setPos(textX, textY);
    }

    QString sceneName = currentScene->getSceneName();
    if(sceneName != "JuegoPolitica" && sceneName != "JuegoHistoria"){
        view->centerOn(player);
    }

    static int frameCount = 0;
    frameCount++;

    if (frameCount % 20 == 0) { // Cada ~1 segundo

        QRectF viewportRect = view->mapToScene(view->viewport()->rect()).boundingRect();
        QRectF playerRect = player->mapToScene(player->boundingRect()).boundingRect();
    }

    JuegoHistoria* juegoHistoria = qobject_cast<JuegoHistoria*>(currentScene);
    if(juegoHistoria) {
        juegoHistoria->verificarLlavesCercanas();

        QMetaObject::invokeMethod(juegoHistoria,"verificarCambioEscena",Qt::QueuedConnection);
    }
}

void Game::checkCollisions()
{
    QRectF playerRect(playerX, playerY, player->getCurrentSize().width(), player->getCurrentSize().height());

    for(const QRectF &obstacle : currentScene->getCollisionAreas()){
        if(playerRect.intersects(obstacle)){
            break;
        }
    }
}

void Game::checkInteractions()
{
    QRectF playerRect(playerX, playerY, player->getCurrentSize().width(), player->getCurrentSize().height());
    bool foundInteraction = false;

    for (const QRectF &area : currentScene->getInteractiveAreas()) {
        if (playerRect.intersects(area)) {
            QString message = currentScene->getInteractionMessage(area);
            showInteractionText(message);
            currentInteraction = message;

            currentInteractiveArea = area;
            foundInteraction = true;
            break;
        }
    }

    if(!foundInteraction && isInteracting){
        hideInteractionText();
        currentInteractiveArea = QRectF();
    }
}

void Game::showInteractionText(const QString &text)
{
    if(!isInteracting){
        interactionText->setPlainText(text);
        interactionText->setTextWidth(110);

        double textX = playerX - (interactionText->boundingRect().width()/2);
        double textY = playerY - 30;

        interactionText->setPos(textX, textY);
        interactionText->setVisible(true);
        isInteracting = true;
    }
}

void Game::hideInteractionText()
{
    interactionText->setVisible(false);
    isInteracting = false;
    currentInteraction.clear();
}

void Game::keyPressEvent(QKeyEvent *event)
{

    if (event->isAutoRepeat()) return;

    if (auto* subP = qobject_cast<SubEscenaPizarra*>(currentScene)) {
        if (subP->manejarTecla(event->key())) return; // <- consumida aquí
    }

    // (si la tienes) SubEscenaLibro
    if (auto* subL = qobject_cast<SubEscenaLibro*>(currentScene)) {
        if (subL->manejarTecla(event->key())) return;
    }

    // (si la tienes) AulaKant: A/B/C/D durante preguntas
    if (auto* aula = qobject_cast<AulaKantScene*>(currentScene)) {
        if (aula->manejarTecla(event->key())) return;
    }

    // (si la tienes) Laboratorio de Descartes: A/B/C/D durante preguntas
    if (auto* lab = qobject_cast<LabDescartes*>(currentScene)) {
        if (lab->manejarTecla(event->key())) return;
    }


  switch (event->key()) {

  case Qt::Key_W: case Qt::Key_A: case Qt::Key_S: case Qt::Key_D:
      if (qobject_cast<SceneBatalla*>(currentScene)) {
          // En batalla no se mueve con WASD
          return;  // <- bloquea movimiento en L3
      }
      // Si no estás en batalla, aplica la lógica normal:
      if (event->key() == Qt::Key_W) movingUp = true;
      if (event->key() == Qt::Key_S) movingDown = true;
      if (event->key() == Qt::Key_A) movingLeft = true;
      if (event->key() == Qt::Key_D) movingRight = true;
      break;

    // case Qt::Key_C:
    case Qt::Key_M:
        mostrarCollisionsDebug();
        if (auto* sub = qobject_cast<SubEscenaLibro*>(currentScene)) {
            sub->toggleDebugAreas();
        }
        return;

    case Qt::Key_K: {
        if (auto* sb = qobject_cast<SceneBatalla*>(currentScene)) {
            sb->debugGanarBatalla();
        }
        return;
    }

    case Qt::Key_G: {
        if (auto* rs = qobject_cast<RuletaScene*>(currentScene)) {
            rs->debugGanarTodoYElegirBando();
        }
        return;
    }

    case Qt::Key_P: {
        if (auto* rs = qobject_cast<RuletaScene*>(currentScene)) {
            rs->debugPerderTodoYReiniciar();
        }
        return;
    }

    case Qt::Key_Space:
        if (auto* aula = qobject_cast<AulaKantScene*>(currentScene)) {
            if (aula->avanzarDialogoSiActivo()) return;
        }
        if (auto* lab = qobject_cast<LabDescartes*>(currentScene)) {
            if (lab->avanzarDialogoSiActivo()) return;
        }
        qDebug() << "posicion x:" << playerX << "y:" << playerY;
        return;


    case Qt::Key_V:
        if (auto* aula = qobject_cast<AulaKantScene*>(currentScene)) {
            if (aula->togglePistasIfHud('V')) return;
        }
        break;


    case Qt::Key_E:
        if (!isInteracting) return;

        if (auto* lab = qobject_cast<LabDescartes*>(currentScene)) {
            if (lab->tryHandleInteraction(currentInteraction)) {
                hideInteractionText();
                return; // no sigas evaluando otros else if
            }
        }

        if (currentInteraction.contains("girar")) {
            if (auto* ruletaScene = qobject_cast<RuletaScene*>(currentScene)) {
                ruletaScene->girarRuleta();
            }
            return;
        }
        else if (currentInteraction.contains("Arte")) {
            SceneBase* oldScene = currentScene;

            // NEW: avisa a la ruleta qué sección vas a jugar (Arte = 0)
            if (auto* rs = qobject_cast<RuletaScene*>(oldScene)) {
                rs->setSeccionEnJuego(0);
            }

            currentScene->removeItem(player);

            auto* juegoArte = new JuegoArte(this);
            currentScene = juegoArte;
            view->setScene(juegoArte);

            connect(juegoArte, &JuegoArte::juegoTerminado, [this, oldScene](bool ganado){
                if (ganado) ganados++;
                currentScene = oldScene;
                currentScene->addItem(player);

                playerX = 400; playerY = 500;
                player->setPos(playerX, playerY);
                player->setVisible(true);
                player->setZValue(1000);

                view->setScene(currentScene);
                view->centerOn(player);

                if (auto* rs = qobject_cast<RuletaScene*>(currentScene)) {
                    rs->initHud(view);
                    if (ganado) rs->agregarJuegoGanado();
                    else        rs->agregarJuegoPerdido();
                }
            });
            hideInteractionText();
            return;
        }
        else if (currentInteraction.contains("Politica")) {
            SceneBase* oldScene = currentScene;

            if (auto* rs = qobject_cast<RuletaScene*>(oldScene)) {
                rs->setSeccionEnJuego(3); // Política = 3
            }

            currentScene->removeItem(player);
            currentScene->removeItem(interactionText);

            auto* juegoPolitica = new JuegoPolitica(this);
            playerX = 400; playerY = 395;
            player->setPos(playerX, playerY);

            juegoPolitica->addItem(player);
            currentScene = juegoPolitica;

            view->setScene(juegoPolitica);
            view->centerOn(500, 500);

            connect(juegoPolitica, &JuegoPolitica::juegoTerminado, [this, oldScene](bool ganado){
                if (ganado) ganados++;

                currentScene->removeItem(player);
                currentScene = oldScene;

                playerX = 400; playerY = 500;
                player->setPos(playerX, playerY);
                player->setVisible(true);
                player->setZValue(1000);
                player->resetAnimation();

                currentScene->addItem(player);
                currentScene->addItem(interactionText);

                view->setScene(currentScene);
                view->centerOn(player);

                if (auto* rs = qobject_cast<RuletaScene*>(currentScene)) {
                    rs->initHud(view);
                    if (ganado) rs->agregarJuegoGanado();
                    else        rs->agregarJuegoPerdido();
                }
            });
            hideInteractionText();
            return;
        }

        else if (currentInteraction.contains("Historia")) {
            SceneBase* oldScene = currentScene;

            if (auto* rs = qobject_cast<RuletaScene*>(oldScene)) {
                rs->setSeccionEnJuego(2); // Historia = 2
            }

            currentScene->removeItem(player);
            currentScene->removeItem(interactionText);

            player->setPlayerSize(55, 75);

            auto* juegoHistoria = new JuegoHistoria(this);
            playerX = 10; playerY = 20;
            player->setPos(playerX, playerY);

            juegoHistoria->addItem(player);
            currentScene = juegoHistoria;

            view->setScene(juegoHistoria);
            view->centerOn(500, 500);

            connect(juegoHistoria, &JuegoHistoria::juegoTerminado, [this, oldScene, juegoHistoria](bool ganado){
                if (ganado) ganados++;

                currentScene->removeItem(player);
                player->resetPlayerSize();

                int llavesRecolectadas = juegoHistoria->getLlavesRecolectadas();
                Q_UNUSED(llavesRecolectadas);

                auto* puertaScene = new PuertaJuegoHistoria(llavesRecolectadas, this);
                currentScene = puertaScene;
                view->setScene(puertaScene);

                connect(puertaScene, &PuertaJuegoHistoria::juegoTerminado, [this, oldScene](bool ganoPuerta){
                    currentScene = oldScene;
                    currentScene->addItem(player);
                    currentScene->addItem(interactionText);

                    playerX = 400; playerY = 500;
                    player->setPos(playerX, playerY);
                    player->setVisible(true);
                    player->setZValue(1000);
                    player->resetAnimation();

                    view->setScene(currentScene);
                    view->centerOn(player);

                    if (auto* rs = qobject_cast<RuletaScene*>(currentScene)) {
                        rs->initHud(view);
                        if (ganoPuerta) rs->agregarJuegoGanado();
                        else            rs->agregarJuegoPerdido();
                    }
                });
            });
            hideInteractionText();
            return;
        }

        else if (currentInteraction.contains("Ciencia")) {
            SceneBase* oldScene = currentScene;

            if (auto* rs = qobject_cast<RuletaScene*>(oldScene)) {
                rs->setSeccionEnJuego(1); // Ciencia = 1
            }

            currentScene->removeItem(player);
            currentScene->removeItem(interactionText);

            auto* juegoCiencia = new JuegoCiencia(this);
            player->setPlayerSize(55, 75);
            playerX = 50; playerY = 300;
            player->setPos(playerX, playerY);

            juegoCiencia->addItem(player);
            currentScene = juegoCiencia;

            view->setScene(juegoCiencia);
            view->centerOn(player);

            connect(juegoCiencia, &JuegoCiencia::juegoTerminado, [this, oldScene](bool ganado){
                if (ganado) ganados++;

                player->resetPlayerSize();
                currentScene->removeItem(player);

                currentScene = oldScene;

                playerX = 400; playerY = 500;
                player->setPos(playerX, playerY);
                player->setVisible(true);
                player->setZValue(1000);
                player->resetAnimation();

                currentScene->addItem(player);
                currentScene->addItem(interactionText);

                view->setScene(currentScene);
                view->centerOn(player);

                if (auto* rs = qobject_cast<RuletaScene*>(currentScene)) {
                    rs->initHud(view);
                    if (ganado) rs->agregarJuegoGanado();
                    else        rs->agregarJuegoPerdido();
                }
            });
            hideInteractionText();
            return;
        }

        else if (currentInteraction.contains("entrar a la universidad")) {
            currentScene->removeItem(player);
            currentScene->removeItem(interactionText);

            auto* terraza = new TerraceScene(this);
            terraza->setProgreso(kantCompletado,descartesCompletado);

            connect(terraza, &TerraceScene::volverAlInicio, this, [this]() {
                if (currentScene && currentScene->items().contains(player))
                    currentScene->removeItem(player);
                tiemposMs.clear();
                kantCompletado = false;
                descartesCompletado = false;
                auto* start = new StartScene(this);
                cambiarEscena(start);
                movimientoHabilitado = false;
                if (view) view->centerOn(start->sceneRect().center());
            });

            playerX = 300; playerY = 420;
            player->setPos(playerX, playerY);

            terraza->addItem(player);
            terraza->addItem(interactionText);
            interactionText->setZValue(1001);
            interactionText->setVisible(false);

            currentScene = terraza;
            view->setScene(terraza);
            view->centerOn(player);

            hideInteractionText();
            return;
        }

        else if (currentInteraction.contains("Emmanuel Kant")) {
            currentScene->removeItem(player);
            currentScene->removeItem(interactionText);

            auto* aula = new AulaKantScene(this);
            playerX = 320; playerY = 500;
            player->setPos(playerX, playerY);

            aula->addItem(player);
            aula->addItem(interactionText);
            interactionText->setZValue(1001);
            interactionText->setVisible(false);

            startLevelTimer();
            currentScene = aula;
            view->setScene(aula);
            view->centerOn(player);
            aula->initHud(view);

            connect(aula, &AulaKantScene::aulaResuelta, this, [this]() {
                kantCompletado = true;

                auto* terraza = new TerraceScene(this);
                terraza->setProgreso(kantCompletado,descartesCompletado);

                connect(terraza, &TerraceScene::volverAlInicio, this, [this]() {
                    if (currentScene && currentScene->items().contains(player))
                        currentScene->removeItem(player);
                    tiemposMs.clear();
                    kantCompletado = false;
                    descartesCompletado = false;
                    auto* start = new StartScene(this);
                    cambiarEscena(start);
                    movimientoHabilitado = false;
                    if (view) view->centerOn(start->sceneRect().center());
                });

                cambiarEscena(terraza, QPointF(300,450));
            });

            hideInteractionText();
            return;
        }

        if (auto* aula = qobject_cast<AulaKantScene*>(currentScene)) {

            if (currentInteraction.contains("hablar con Kant")) {
                movimientoHabilitado = false;
                connect(aula, &AulaKantScene::dialogoTerminado, this, [this](){
                    movimientoHabilitado = true;
                });

                QStringList guion{
                    "Kant: Bienvenido a mi aula, veo que ya elegiste tu bando...",
                    "Kant: Interesante eleccion...",
                    "Kant: En otro tema, has escuchado sobre el misterio en esta universidad?",
                    "Kant: El otro dia se robaron mi pluma favorita para escribir mis pensamientos, me ayudas a encontrar quien fue?",
                    "Kant: Inspecciona mi aula, talvez hay pistas por algun lado"
                };
                aula->iniciarDialogoKant(guion);
                hideInteractionText();
                return;
            }

            if (!currentInteraction.contains("pizarra", Qt::CaseInsensitive) &&
                !currentInteraction.contains("librero", Qt::CaseInsensitive)) {
                aula->mostrarPreguntaDesdeMensaje(currentInteraction);
                hideInteractionText();
                return;
            }
        }


        if (currentInteraction.contains("pizarra", Qt::CaseInsensitive)) {
            auto* aula = qobject_cast<AulaKantScene*>(currentScene);
            if (!aula) return;

            currentScene->removeItem(player);
            currentScene->removeItem(interactionText);

            const double oldX = playerX;
            const double oldY = playerY;

            auto* sub = new SubEscenaPizarra(this);

            currentScene = sub;
            view->setScene(sub);
            view->centerOn(400, 300);

            connect(sub, &SubEscenaPizarra::pistaDesbloqueada, this, [this, aula](const QString& pista){
                aula->addPistaDesdeFuera(pista);
            });

            connect(sub, &SubEscenaPizarra::subescenaTerminada, this, [this, aula, oldX, oldY](){
                currentScene = aula;
                view->setScene(aula);

                aula->marcarRespondidoExterno("pizarra");

                if (!aula->items().contains(player))         aula->addItem(player);
                if (!aula->items().contains(interactionText)) aula->addItem(interactionText);

                interactionText->setZValue(1001);
                interactionText->setVisible(false);

                playerX = oldX;
                playerY = oldY;
                player->setPos(playerX, playerY);
                player->setVisible(true);
                player->setZValue(1000);

                aula->initHud(view);
                view->centerOn(player);
            });

            hideInteractionText();
            return;
        }

        else if (currentInteraction.contains("inspeccionar el librero")) {
            // Guardar referencia al aula actual y la posición del jugador
            auto* aula = qobject_cast<AulaKantScene*>(currentScene);
            if (!aula) return;

            // Quitar player/interactionText del aula (no se usan en la subescena)
            currentScene->removeItem(player);
            currentScene->removeItem(interactionText);

            const double oldX = playerX;
            const double oldY = playerY;
            // (no tocamos playerX/Y para preservar)

            // Crear subescena
            auto* sub = new SubEscenaLibro(this);

            // Cargar dos preguntas de las 6 (ejemplo)
            LibroPregunta p1{
                "1) Seleccione el mandato cuya obligación viene del miedo al castigo o la búsqueda de un premio:",
                {"Imperativo Hipotético.","Imperativo categórico.","Ambos.","Ninguno"},
                'A',
                "Al culpable le encanta leer."
            };
            LibroPregunta p2{
                "4) Kant cambia el centro del conocimiento del objeto al sujeto. A esto se le llama:",
                {"Subjetivismo.","Prejuicio.","Giro copernicano.","Suerte"},
                'C',
                "El culpable pasa mucho tiempo con libros"
            };
            sub->setPreguntas(p1, p2);

            // Pasar escena a la view
            currentScene = sub;
            view->setScene(sub);
            view->centerOn(500, 500);

            // Delegar teclado (ABCD y bloquear WASD con panel)
            // (Ya lo tienes en la cabecera de keyPressEvent, asegúrate de incluir:)
            // if (auto* sub = qobject_cast<SubEscenaLibro*>(currentScene)) { if (sub->manejarTecla(event->key())) return; }

            // Al ganar pista(s)
            connect(sub, &SubEscenaLibro::pistaDesbloqueada, this, [this, aula](const QString& pista){
                aula->addPistaDesdeFuera(pista);
            });

            // Al terminar, volver al MISMO aula y misma posición del player
            connect(sub, &SubEscenaLibro::subescenaTerminada, this, [this, aula, oldX, oldY](){
                // Volver a poner la view con el aula original
                currentScene = aula;
                view->setScene(aula);

                aula->marcarRespondidoExterno("libro1");
                aula->marcarRespondidoExterno("libro2");
                // Reinsertar player e interactionText en el aula
                if (!aula->items().contains(player)) aula->addItem(player);
                if (!aula->items().contains(interactionText)) aula->addItem(interactionText);
                interactionText->setZValue(1001);
                interactionText->setVisible(false);

                // <<< POSICIÓN EXACTA ANTERIOR >>>
                playerX = oldX;
                playerY = oldY;
                player->setPos(playerX, playerY);
                player->setVisible(true);
                player->setZValue(1000);

                // Importante: mantener el HUD del aula anclado
                aula->initHud(view);

                // Centrar cámara en el jugador (no cambia su posición)
                view->centerOn(player);
            });

            hideInteractionText();
            return;
        }
        else if (currentInteraction.contains("laboratorio", Qt::CaseInsensitive)) {

            currentScene->removeItem(player);
            currentScene->removeItem(interactionText);

            auto* lab = new LabDescartes(this);

            // posición inicial del jugador dentro del lab
            playerX = 520; playerY = 520;
            player->setPos(playerX, playerY);

            lab->addItem(player);
            lab->addItem(interactionText);
            interactionText->setZValue(1001);
            interactionText->setVisible(false);

            startLevelTimer();
            currentScene = lab;
            view->setScene(lab);
            view->centerOn(player);
            lab->initHud(view);

            // Cuando se resuelve el laboratorio, volver a la terraza
            connect(lab, &LabDescartes::laboratorioResuelto, this, [this]() {
                stopLevelTimerAndStore();
                descartesCompletado = true;

                auto* terraza = new TerraceScene(this);
                terraza->setProgreso(kantCompletado,descartesCompletado);

                connect(terraza, &TerraceScene::volverAlInicio, this, [this]() {
                    if (currentScene && currentScene->items().contains(player))
                        currentScene->removeItem(player);
                    tiemposMs.clear();
                    kantCompletado = false;
                    descartesCompletado = false;
                    auto* start = new StartScene(this);
                    cambiarEscena(start);
                    movimientoHabilitado = false;
                    if (view) view->centerOn(start->sceneRect().center());
                });

                trySaveRankingIfComplete();
                cambiarEscena(terraza, QPointF(300,450));
            });

            hideInteractionText();
            return;
        }
        // ---- Si YA estás en el Laboratorio de Descartes ----


    default:
        break;
    }

    QMainWindow::keyPressEvent(event);
}


void Game::keyReleaseEvent(QKeyEvent *event){
    if(event->isAutoRepeat()) return;

    switch(event->key()){
    case Qt::Key_W: movingUp = false; break;
    case Qt::Key_S: movingDown = false; break;
    case Qt::Key_A: movingLeft = false; break;
    case Qt::Key_D: movingRight = false; break;
    }
}

void Game::mostrarCollisionsDebug()
{
    // Primero limpiar cualquier visualización anterior

    // Obtener todas las áreas de colisión de la escena actual
    const QList<QRectF>& collisionAreas = currentScene->getCollisionAreas();


    for (int i = 0; i < collisionAreas.size(); ++i) {
        const QRectF& area = collisionAreas[i];

        // Crear un rectángulo rojo semi-transparente
        QGraphicsRectItem* debugRect = new QGraphicsRectItem(area);
        debugRect->setBrush(QBrush(QColor(255, 0, 0, 100))); // Rojo semi-transparente
        debugRect->setPen(QPen(Qt::red, 3)); // Borde rojo grueso
        debugRect->setZValue(999); // Casi al frente, pero debajo del player

        // Agregar a la escena
        currentScene->addItem(debugRect);
        debugCollisionRects.append(debugRect);

    }

    // También visualizar el rectángulo del player en azul
    QSize playerSize = player->getCurrentSize();
    QRectF playerRect(playerX, playerY, playerSize.width(), playerSize.height());
    QGraphicsRectItem* playerDebugRect = new QGraphicsRectItem(playerRect);
    playerDebugRect->setBrush(QBrush(QColor(0, 0, 255, 100)));
    playerDebugRect->setPen(QPen(Qt::blue, 3));
    playerDebugRect->setZValue(998);
    currentScene->addItem(playerDebugRect);
    debugCollisionRects.append(playerDebugRect);


}
void Game::cambiarEscena(SceneBase* nuevaEscena, const QPointF& spawn)
{
    if (currentScene) {
        if (currentScene->items().contains(player))          currentScene->removeItem(player);
        if (currentScene->items().contains(interactionText)) currentScene->removeItem(interactionText);
    }

    currentScene = nuevaEscena;

    const bool wantsPlayer = !( qobject_cast<Mapa*>(currentScene)
                               || qobject_cast<StartScene*>(currentScene)
                               || qobject_cast<VideoScene*>(currentScene) );

    if (wantsPlayer && !currentScene->items().contains(player)) {
        currentScene->addItem(player);
        playerX = spawn.x(); playerY = spawn.y();
        player->setPos(playerX, playerY);
        player->setVisible(true);
        player->setZValue(1000);
        player->resetAnimation();
    }

    if (!currentScene->items().contains(interactionText))
        currentScene->addItem(interactionText);
    interactionText->setZValue(1001);
    interactionText->setVisible(false);

    view->setScene(currentScene);
    if (wantsPlayer) view->centerOn(player);
    else             view->centerOn(currentScene->sceneRect().center());

    if (auto* rs = qobject_cast<RuletaScene*>(currentScene)) {
        rs->initHud(view);
        QObject::disconnect(rs, nullptr, this, nullptr);
        startLevelTimer();
        connect(rs, &RuletaScene::irAlMapaTrasBando, this, [this](bool esEmpirista, int vidaInicial){
            stopLevelTimerAndStore();
            const qint64 ms= tiemposMs.isEmpty()?-1:tiemposMs.back();
            if(ms>=0){
                StatsFile::upsertRun(
                    nombreUsuario.isEmpty()? QString("NN"):nombreUsuario,
                    esEmpirista?0:1, 2, double(ms));
            }
            player->cambiarBando(esEmpirista ? Player::Bando::EMPIRISTA : Player::Bando::RACIONALISTA);
            vidaParaL3 = vidaInicial;
            if (mapa) {
                cambiarEscena(mapa);
                mapa->unlock("L3");
            }
            movimientoHabilitado = false;
        });
    }

    if (auto* t = qobject_cast<TerraceScene*>(currentScene)) {
        conectarTerraza(t);
    }
    if (auto* s = qobject_cast<StartScene*>(currentScene)) {
        conectarStartScene(s);
    }
}

void Game::cambiarEscena(SceneBase* nuevaEscena)
{
    if (currentScene) {
        if (currentScene->items().contains(player))          currentScene->removeItem(player);
        if (currentScene->items().contains(interactionText)) currentScene->removeItem(interactionText);
    }

    currentScene = nuevaEscena;

    const bool wantsPlayer = !( qobject_cast<Mapa*>(currentScene)
                               || qobject_cast<StartScene*>(currentScene)
                               || qobject_cast<VideoScene*>(currentScene) );

    if (wantsPlayer && !currentScene->items().contains(player)) {
        currentScene->addItem(player);
        QPointF pos(530, 200);
        if (qobject_cast<RuletaScene*>(currentScene)) pos = QPointF(400, 500);
        playerX = pos.x(); playerY = pos.y();
        player->setPos(playerX, playerY);
        player->setVisible(true);
        player->setZValue(1000);
        player->resetAnimation();
    }

    if (!currentScene->items().contains(interactionText))
        currentScene->addItem(interactionText);
    interactionText->setZValue(1001);
    interactionText->setVisible(false);

    view->setScene(currentScene);
    if (wantsPlayer) view->centerOn(player);
    else             view->centerOn(currentScene->sceneRect().center());

    if (auto* rs = qobject_cast<RuletaScene*>(currentScene)) {
        rs->initHud(view);
        QObject::disconnect(rs, nullptr, this, nullptr);
        startLevelTimer();
        connect(rs, &RuletaScene::irAlMapaTrasBando, this, [this](bool esEmpirista, int vidaInicial){
            stopLevelTimerAndStore();
            const qint64 ms= tiemposMs.isEmpty()?-1:tiemposMs.back();
            if(ms>=0){
                StatsFile::upsertRun(
                    nombreUsuario.isEmpty()? QString("NN"):nombreUsuario,
                    esEmpirista?0:1, 2, double(ms));
            }
            player->cambiarBando(esEmpirista ? Player::Bando::EMPIRISTA : Player::Bando::RACIONALISTA);
            vidaParaL3 = vidaInicial;
            if (mapa) {
                cambiarEscena(mapa);
                mapa->unlock("L3");
            }
            movimientoHabilitado = false;
        });
    }

    if (auto* t = qobject_cast<TerraceScene*>(currentScene)) {
        conectarTerraza(t);
    }
    if (auto* s = qobject_cast<StartScene*>(currentScene)) {
        conectarStartScene(s);
    }
}

void Game::trySaveRankingIfComplete() {

}

void Game::conectarStartScene(StartScene* start)
{
    if (!start) return;

    QObject::disconnect(start, nullptr, this, nullptr);

    connect(start, &StartScene::pedirNombreUsuario, this, [this](){
        bool ok = false;
        const QString nombre = QInputDialog::getText(this, "Usuario", "Ingresa tu nombre:", QLineEdit::Normal, "", &ok);
        if (ok && !nombre.trimmed().isEmpty()) {
            nombreUsuario = nombre.trimmed();
        }
    });

    connect(start, &StartScene::pedirTipoPartida, this, [this](){
        mapa = new Mapa(this);
        cambiarEscena(mapa);
        movimientoHabilitado = false;
        mapa->setUnlocked(QSet<QString>() << "L1");
        connect(mapa, &Mapa::levelChosen, this, [this](const QString& node){
            if (node == "L1") {
                auto* video = new VideoScene(this);
                cambiarEscena(video);
                connect(video, &VideoScene::videoTerminado, this, [this](){
                    cambiarEscena(mapa);
                    mapa->unlock("L1");
                    mapa->unlockVecinos("L1");
                    movimientoHabilitado = false;
                });
            } else if (node == "L2") {
                auto* ruleta = new RuletaScene(this);
                cambiarEscena(ruleta);
                movimientoHabilitado = true;
            } else if (node == "L3") {
                iniciarBatallaL3(vidaParaL3);
            } else if (node == "L4") {
                auto* camino = new CaminoUni(this);
                startLevelTimer();
                cambiarEscena(camino, QPointF(530,200));
                movimientoHabilitado = true;
                connect(camino, &CaminoUni::caminoTerminado, this, [this](bool){
                    stopLevelTimerAndStore();
                    const qint64 ms = tiemposMs.isEmpty() ? -1 : tiemposMs.back();
                    if (ms >= 0) {
                        StatsFile::upsertRun(
                            nombreUsuario.isEmpty() ? QString("NN") : nombreUsuario,
                            (player->bandoActual == Player::Bando::EMPIRISTA) ? 0 : 1,
                            4, double(ms));
                    }
                });
            }
        });
    });
}

void Game::conectarTerraza(TerraceScene* terraza)
{
    if (!terraza) return;
    QObject::disconnect(terraza, nullptr, this, nullptr);
    connect(terraza, &TerraceScene::volverAlInicio, this, [this](){
        volverAlInicioDesdeTerraza();
    });
}

void Game::volverAlInicioDesdeTerraza()
{
    tiemposMs.clear();
    kantCompletado = false;
    descartesCompletado = false;
    ganados = 0;
    vidaParaL3 = 0;
    movimientoHabilitado = false;

    if (combateActual) {
        combateActual->disconnect(this);
        if (currentScene && currentScene->items().contains(player))
            currentScene->removeItem(player);
        combateActual->deleteLater();
        combateActual = nullptr;
    }

    auto* start = new StartScene(this);
    cambiarEscena(start);
    conectarStartScene(start);
    view->centerOn(currentScene->sceneRect().center());
}

