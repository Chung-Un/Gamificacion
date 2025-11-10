#include "puertajuegohistoria.h"
#include <QGraphicsSceneMouseEvent>
#include <QDebug>

PuertaJuegoHistoria::PuertaJuegoHistoria(int llavesObtenidas, QObject *parent)
    : SceneBase(parent),
    llaves(llavesObtenidas),
    candadosAbiertos(0),
    mensajeFinal(nullptr),
    fondoMensaje(nullptr)
{
    qDebug() << "🔑 PuertaJuegoHistoria creada con" << llavesObtenidas << "llaves";
    loadScene();
}

void PuertaJuegoHistoria::loadScene()
{
    // --- Configurar tamaño de escena PRIMERO ---
    setSceneRect(0, 0, 800, 600);

    // --- Fondo general ---
    QPixmap fondo("://res/pared.png");
    setBackground(fondo.scaled(800, 600, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

    // --- Puerta ---
    QPixmap puertaPixmap("://res/Objects/closedDoorColored.png");
    QGraphicsPixmapItem* puerta = new QGraphicsPixmapItem(puertaPixmap.scaled(291, 438));
    puerta->setPos(260, 154); // Centrada horizontalmente: (800-194)/2 = 303
    puerta->setZValue(5);
    addObject(puerta);

    // --- Cadena ---
    QPixmap cadenaPixmap("://res/Objects/cadena.png");
    QGraphicsPixmapItem* cadena = new QGraphicsPixmapItem(cadenaPixmap.scaled(300, 19));
    cadena->setPos(260, 380); // Centrada sobre los candados
    cadena->setZValue(8);
    addObject(cadena);

    // --- Texto de instrucciones ---
    QGraphicsTextItem* instrucciones = new QGraphicsTextItem();
    instrucciones->setPlainText(QString("Haz clic en los candados para usar tus %1 llaves").arg(llaves));
    instrucciones->setDefaultTextColor(Qt::white);
    instrucciones->setFont(QFont("Minecraft", 14, QFont::Bold));
    instrucciones->setPos(180, 70);
    instrucciones->setZValue(20);
    addObject(instrucciones);

    // --- Crear 5 candados clicables ---
    for (int i = 0; i < 5; ++i) {
        QGraphicsPixmapItem *candado = new QGraphicsPixmapItem(QPixmap("://res/Objects/candado.png").scaled(30, 30));
        candado->setPos(280 + i * 60, 395); // Espaciados uniformemente
        candado->setZValue(10);
        candado->setData(0, i); // Guardar índice del candado
        addObject(candado);
        candados.append(candado);
    }

    // --- Contador de llaves restantes ---
    contadorLlaves = new QGraphicsTextItem();
    contadorLlaves->setPlainText(QString("Llaves restantes: %1").arg(llaves));
    contadorLlaves->setDefaultTextColor(Qt::white);
    contadorLlaves->setFont(QFont("Minecraft", 16, QFont::Bold));
    contadorLlaves->setPos(300, 570);
    contadorLlaves->setZValue(20);
    addObject(contadorLlaves);

    fondoInstru= new QGraphicsRectItem(0,0,800,40);
    fondoInstru->setBrush(QBrush(QColor(72, 16, 86)));
    fondoInstru->setPos(0,50);
    fondoInstru->setZValue(10);
    fondoInstru->setVisible(true);
    addObject(fondoInstru);

    fondoLlave= new QGraphicsRectItem(0,0,800,100);
    fondoLlave->setBrush(QBrush(QColor(72, 16, 86)));
    fondoLlave->setPos(0,570);
    fondoLlave->setZValue(10);
    fondoLlave->setVisible(true);
    addObject(fondoLlave);

    // --- Mensaje final oculto ---
    fondoMensaje = new QGraphicsRectItem(0, 0, 800, 150);
    fondoMensaje->setBrush(QBrush(QColor(0, 0, 0, 200)));
    fondoMensaje->setPos(0, 225);
    fondoMensaje->setZValue(50);
    fondoMensaje->setVisible(false);
    addObject(fondoMensaje);

    mensajeFinal = new QGraphicsTextItem();
    mensajeFinal->setDefaultTextColor(Qt::white);
    mensajeFinal->setFont(QFont("Minecraft", 18, QFont::Bold));
    mensajeFinal->setTextWidth(700);
    mensajeFinal->setZValue(60);
    mensajeFinal->setPos(50, 260);
    mensajeFinal->setVisible(false);
    addObject(mensajeFinal);
}

void PuertaJuegoHistoria::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    // Si ya usó todas sus llaves, no hacer nada
    if (llaves <= 0) {
        qDebug() << "❌ No quedan llaves disponibles";
        return;
    }

    QPointF clickPos = event->scenePos();

    // Verificar si hizo clic en un candado
    for (QGraphicsPixmapItem *candado : candados) {
        if (candado->isVisible() && candado->contains(candado->mapFromScene(clickPos))) {
            // Abrir el candado
            candado->setVisible(false);
            candadosAbiertos++;
            llaves--; // Usar una llave

            qDebug() << "🔓 Candado abierto. Candados abiertos:" << candadosAbiertos << "| Llaves restantes:" << llaves;

            // Actualizar contador
            contadorLlaves->setPlainText(QString("Llaves restantes: %1").arg(llaves));

            // Verificar si ya usó todas sus llaves o abrió todos los candados
            if (llaves <= 0 || candadosAbiertos >= 5) {
                verificarFin();
            }

            break; // Solo abrir un candado por clic
        }
    }

    SceneBase::mousePressEvent(event);
}

void PuertaJuegoHistoria::verificarFin()
{
    qDebug() << "🎯 Verificando fin del juego. Candados abiertos:" << candadosAbiertos;

    // Determinar si ganó
    bool gano = (candadosAbiertos >= 5);

    mostrarMensaje(gano);
}

void PuertaJuegoHistoria::mostrarMensaje(bool ganado)
{
    fondoMensaje->setVisible(true);
    mensajeFinal->setVisible(true);

    if (ganado) {
        mensajeFinal->setPlainText("¡Felicidades! Has abierto todos los candados\ny escapaste del laberinto.");
        mensajeFinal->setDefaultTextColor(Qt::green);
    } else {
        mensajeFinal->setPlainText(QString("No conseguiste suficientes llaves...\nSolo abriste %1 de 5 candados.").arg(candadosAbiertos));
        mensajeFinal->setDefaultTextColor(Qt::red);
    }

    qDebug() << "📢 Mostrando mensaje:" << (ganado ? "GANÓ" : "PERDIÓ");

    // Esperar 4 segundos y luego emitir la señal
    QTimer::singleShot(4000, [this, ganado]() {
        qDebug() << "🔄 Emitiendo juegoTerminado con ganado =" << ganado;
        emit juegoTerminado(ganado);
    });
}
