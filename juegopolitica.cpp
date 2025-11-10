#include "juegopolitica.h"
#include "player.h"
#include <QGraphicsTextItem>
#include <QGraphicsRectItem>
#include <QRandomGenerator>
#include <QDebug>
#include <QTimer>
#include <QGraphicsPixmapItem>
#include <QFont>

JuegoPolitica::JuegoPolitica(QObject *parent)
    : SceneBase(parent)
{
    cantCorrectas = 0;
    errores = 0;
    noPregunta = 1;
    esperandoRespuesta = false;
    textPregunta = nullptr;
    barraPregunta = nullptr;
    fondoOpciones = nullptr;

    preguntas.enqueue("Durante el renacimiento, el modelo de gobierno es uno de los siguientes:");
    respuestasCorrectas.enqueue("A");
    opciones.enqueue(QStringList{
        "A. Monarquia absoluta",
        "B. Tiranía republicana",
        "C. Democracia participativa",
        "D. Liberalismo político"
    });

    preguntas.enqueue("De los siguientes acontecimientos, seleccione el que inicia el período moderno:");
    respuestasCorrectas.enqueue("B");
    opciones.enqueue(QStringList{
        "A. Toma de Constantinopla",
        "B. Tratado de paz de Westfalia",
        "C. Toma de la Bastilla",
        "D. La ruta de la seda"
    });

    preguntas.enqueue("Durante el siglo XV, la sociedad se estratifica en tres estamentos definidos:");
    respuestasCorrectas.enqueue("B");
    opciones.enqueue(QStringList{
        "A. Clase media, baja y alta",
        "B. Nobleza, clero y estado llano",
        "C. Artesanos, guardianes y gobernantes",
        "D. Siervos, nobles y reyes"
    });

    preguntas.enqueue("Aparece el realismo político, basado en un orden establecido y cómo gobernar:");
    respuestasCorrectas.enqueue("C");
    opciones.enqueue(QStringList{
        "A. Tomás Moro",
        "B. Jean Bodín",
        "C. Nicolas Maquiavelo",
        "D. Erasmo de Rotterdam"
    });

    preguntas.enqueue("Terminada la Edad Media, en el contexto político:");
    respuestasCorrectas.enqueue("B");
    opciones.enqueue(QStringList{
        "A. La Iglesia resalta su poder",
        "B. La Iglesia pierde el papel rector en la política",
        "C. La Iglesia evangélica se posiciona en la política",
        "D. La política desaparece"
    });
    loadScene();
}

void JuegoPolitica::loadScene()
{

    QPixmap bgPixmap("://res/cemetery.png");

    setBackground(bgPixmap.scaled(1000, 1000));

    setSceneRect(0, 0, 1000, 1000);

    setupCollisions();
    setupInteractiveObjects();
    setupInteractions();

    QTimer::singleShot(300, this, [this]() {
        mostrarPregunta();
        mostrarOpciones();
    });

    timerItems = new QTimer(this);
    connect(timerItems, &QTimer::timeout, this, &JuegoPolitica::generarItem);

    timerMovimiento = new QTimer(this);
    connect(timerMovimiento, &QTimer::timeout, this, &JuegoPolitica::moverItems);
    timerMovimiento->start(30);

}

void JuegoPolitica::mostrarPregunta()
{

    if (preguntas.isEmpty()) {
        return;
    }

    if (textPregunta) {
        removeItem(textPregunta);
        delete textPregunta;
        textPregunta = nullptr;
    }
    if (barraPregunta) {
        removeItem(barraPregunta);
        delete barraPregunta;
        barraPregunta = nullptr;
    }

    barraPregunta = new QGraphicsRectItem(0, 0, 1000, 150);
    barraPregunta->setBrush(QBrush(QColor(70, 23, 180, 255)));
    barraPregunta->setPen(QPen(Qt::white, 3));
    barraPregunta->setPos(0, 200);
    barraPregunta->setZValue(300);
    addObject(barraPregunta);

    QString texto = QString("Pregunta %1:\n%2").arg(noPregunta).arg(preguntas.first());
    textPregunta = new QGraphicsTextItem(texto);
    textPregunta->setDefaultTextColor(Qt::white);
    textPregunta->setFont(QFont("Minecraft", 20, QFont::Bold));
    textPregunta->setTextWidth(600);
    textPregunta->setPos(100, 220);
    textPregunta->setZValue(301);

    addObject(textPregunta);

    QTimer::singleShot(5000, this, [this]() {

        if (textPregunta) {
            textPregunta->setVisible(false);
        }
        if (barraPregunta) {
            barraPregunta->setVisible(false);
        }

        esperandoRespuesta = true;
        timerItems->start(1500);
    });
}

void JuegoPolitica::mostrarOpciones()
{
    for (auto* t : textosOpciones) {
        if (t) {
            removeItem(t);
            delete t;
        }
    }
    textosOpciones.clear();

    if (fondoOpciones) {
        removeItem(fondoOpciones);
        delete fondoOpciones;
        fondoOpciones = nullptr;
    }

    if (opciones.isEmpty()) {
        return;
    }

    QStringList actual = opciones.first();

    fondoOpciones = new QGraphicsRectItem(0, 600, 1000, 200);
    fondoOpciones->setBrush(QBrush(QColor(0, 0, 0, 230)));
    fondoOpciones->setPen(QPen(Qt::yellow, 3));
    fondoOpciones->setZValue(200);
    addObject(fondoOpciones);

    int y = 620;
    for (const QString& opcion : actual) {
        QGraphicsTextItem* textoOp = new QGraphicsTextItem(opcion);
        textoOp->setDefaultTextColor(Qt::yellow);
        textoOp->setFont(QFont("Minecraft", 16, QFont::Bold));
        textoOp->setPos(100, y);
        textoOp->setZValue(201);
        addObject(textoOp);
        textosOpciones.append(textoOp);
        y += 40;
    }
}


void JuegoPolitica::generarItem()
{
    if (opciones.isEmpty() || !esperandoRespuesta) return;

    QStringList actual = opciones.first();
    int indice = QRandomGenerator::global()->bounded(actual.size());
    QString letra = QString(QChar('A' + indice));

    QPixmap pix("://res/Objects/ataud.png");
    QGraphicsPixmapItem* item = new QGraphicsPixmapItem(pix.scaled(80, 80));
    item->setData(0, letra);
    int x = QRandomGenerator::global()->bounded(100, 850);
    item->setPos(x, -100);
    item->setZValue(10);
    addObject(item);

    QGraphicsTextItem* texto = new QGraphicsTextItem(letra);
    texto->setDefaultTextColor(Qt::yellow);
    texto->setFont(QFont("Arial", 24, QFont::Bold));
    texto->setZValue(11);
    addObject(texto);

    ItemConTexto itemCompleto;
    itemCompleto.pixmap = item;
    itemCompleto.texto = texto;
    itemCompleto.letra = letra;
    itemsCayendo.append(itemCompleto);
}


void JuegoPolitica::moverItems()
{
    QList<QGraphicsItem*> sceneItems = items();
    QGraphicsItem* player = nullptr;

    for (auto* it : sceneItems) {
        if (it->zValue() == 1000) {
            player = it;
            break;
        }
    }
    if (!player) return;

    for (int i = itemsCayendo.size() - 1; i >= 0; --i) {
        ItemConTexto& itemCompleto = itemsCayendo[i];
        QGraphicsPixmapItem* item = itemCompleto.pixmap;
        QGraphicsTextItem* texto = itemCompleto.texto;

        item->moveBy(0, 5);
        if (texto)
            texto->setPos(item->x() + 25, item->y() + 20);

        if (item->collidesWithItem(player) && esperandoRespuesta) {
            QString atrapada = itemCompleto.letra;
            QString correcta = respuestasCorrectas.first();
            QString resultado = "";

            if (atrapada == correcta) {
                cantCorrectas++;
                resultado = "Respuesta correcta!";
            } else {
                errores++;
                resultado = "Respuesta incorrecta!";
            }

            if (barraPregunta) {
                barraPregunta->setVisible(true);
            }
            if (textPregunta) {
                textPregunta->setVisible(true);
                textPregunta->setPlainText(resultado);
            }

            removeItem(item);
            removeItem(texto);
            delete item;
            delete texto;
            itemsCayendo.removeAt(i);

            timerItems->stop();
            esperandoRespuesta = false;

            limpiarItemsCayendo();

            QTimer::singleShot(2000, this, [this]() {
                if (barraPregunta) {
                    barraPregunta->setVisible(false);
                }
                if (textPregunta) {
                    textPregunta->setVisible(false);
                }

                QTimer::singleShot(300, this, &JuegoPolitica::siguientePregunta);
            });

            return;
        }

        if (item->y() > 1000) {
            removeItem(item);
            removeItem(texto);
            delete item;
            delete texto;
            itemsCayendo.removeAt(i);
        }
    }
}

void JuegoPolitica::limpiarItemsCayendo()
{
    for (auto& itemCompleto : itemsCayendo) {
        if (itemCompleto.pixmap) {
            removeItem(itemCompleto.pixmap);
            delete itemCompleto.pixmap;
        }
        if (itemCompleto.texto) {
            removeItem(itemCompleto.texto);
            delete itemCompleto.texto;
        }
    }
    itemsCayendo.clear();
}

void JuegoPolitica::siguientePregunta()
{
    if (!preguntas.isEmpty()) preguntas.dequeue();
    if (!respuestasCorrectas.isEmpty()) respuestasCorrectas.dequeue();
    if (!opciones.isEmpty()) opciones.dequeue();

    noPregunta++;

    if (preguntas.isEmpty()) {
        terminarJuego();
        return;
    }

    mostrarPregunta();
    mostrarOpciones();
}

void JuegoPolitica::setupInteractions() {}
void JuegoPolitica::setupInteractiveObjects() {}

void JuegoPolitica::setupCollisions()
{
    addCollisionArea(QRectF(0, 600, 1000, 20));
    addCollisionArea(QRectF(0, 290, 1000, 10));
    addCollisionArea(QRectF(880,0,1,1000));
    addCollisionArea(QRectF(80,0,1,1000));
}

void JuegoPolitica::terminarJuego()
{
    timerItems->stop();
    timerMovimiento->stop();
    limpiarItemsCayendo();

    if (errores >= 1) {
        QGraphicsItem* playerItem = nullptr;
        for (QGraphicsItem* item : items()) {
            if (item->zValue() == 1000) {
                playerItem = item;
                break;
            }
        }

        if (playerItem) {
            Player* player = dynamic_cast<Player*>(playerItem);
            if (player) {
                player->playDisintegrateAnimation();

                connect(player, &Player::animationFinished, this, [this]() {
                    ocultarOpcionesYFondo();
                    mostrarResultadoFinal();
                }, Qt::SingleShotConnection);

                return;
            }
        }
    }

    ocultarOpcionesYFondo();
    mostrarResultadoFinal();
}

void JuegoPolitica::mostrarResultadoFinal()
{
    QString mensaje;
    if (errores!=0) {
        mensaje = "Has perdido!\nCorrectas: %1\nIncorrectas: %2 ";
    } else if(errores==0){
        mensaje="Has ganado!";
    }

    QGraphicsTextItem* resultado = new QGraphicsTextItem(
        mensaje.arg(cantCorrectas).arg(errores)
        );
    resultado->setDefaultTextColor(errores >= 1 ? Qt::red : Qt::yellow);
    resultado->setFont(QFont("Minecraft", 24, QFont::Bold));
    resultado->setPos(400, 300);
    resultado->setZValue(400);
    addObject(resultado);

    QTimer::singleShot(2000, this, [this]() { hacerCaerResultados(); });
    QTimer::singleShot(7000, this, [this]() {
        bool ganado = (errores==0);
        emit juegoTerminado(ganado); });
}

void JuegoPolitica::hacerCaerResultados()
{
    QString rutaBuena = "://res/Objects/flower.png";
    QString rutaMala = "://res/Objects/estaca.png";

    int totalItems = 5;
    int x = 150;
    int espaciado = 150;

    for (int i = 0; i < totalItems; ++i) {
        QString ruta = (i < errores) ? rutaMala : rutaBuena;

        QPixmap pix(ruta);
        QGraphicsPixmapItem* item;
        if(errores==0){
            item = new QGraphicsPixmapItem(pix.scaled(100, 100));

        }else{
            item = new QGraphicsPixmapItem(pix.scaled(40, 80));

        }
        item->setPos(x + (i * espaciado), -100);
        item->setZValue(350);
        addObject(item);

        QTimer* timerCaida = new QTimer(this);
        connect(timerCaida, &QTimer::timeout, [=]() {
            item->moveBy(0, 10);
            if (item->y() >= 400) {
                timerCaida->stop();
                timerCaida->deleteLater();
            }
        });
        timerCaida->start(30);
    }
}

void JuegoPolitica::ocultarOpcionesYFondo()
{
    if (fondoOpciones) {
        fondoOpciones->setVisible(false);
    }

    for (QGraphicsTextItem* texto : textosOpciones) {
        if (texto) {
            texto->setVisible(false);
        }
    }
}
