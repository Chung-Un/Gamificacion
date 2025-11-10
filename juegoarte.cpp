#include "juegoarte.h"
#include <QGraphicsPixmapItem>
#include <QGraphicsTextItem>
#include <QPixmap>
#include <QPointF>
#include <QFont>
#include <QBrush>
#include <QGraphicsSceneMouseEvent>
#include <QCursor>

JuegoArte::JuegoArte(QObject *parent)
    : SceneBase(parent)
{
    this->name = "JuegoRuleta";
    cantCorrectas = 0;
    noPregunta = 1;
    instruccionesLeidas = false;
    interactionBlocked = false;
    time = 30;

    // Inicializar cola de respuestas correctas
    for(int i = 0; i < 5; i++){
        respondidasCorrectamente.enqueue(false);
    }

    // Pregunta 1
    preguntas.enqueue("Uno de los siguientes personajes fue el encargado de pintar la capilla Sixtina:");
    respuestasCorrectas.enqueue("Miguel Ángel");
    QStringList opciones1;
    opciones1 << "Miguel Ángel" << "Donatello" << "Leonardo Da Vinci" << "Francis Bacon";
    opciones.enqueue(opciones1);

    // Pregunta 2
    preguntas.enqueue("Genio del renacimiento que esculpió el Moisés, el David y la Pietá");
    respuestasCorrectas.enqueue("Miguel Ángel Buonarroti");
    QStringList opciones2;
    opciones2 << "Miguel Ángel Buonarroti" << "Leonardo Da Vinci" << "Rafael Sanzio" << "Galileo Galilei";
    opciones.enqueue(opciones2);

    // Pregunta 3
    preguntas.enqueue("Durante el renacimiento el estilo artístico que impregnó el arte, la filosofía, la pintura escritura fue el");
    respuestasCorrectas.enqueue("El barroco");
    QStringList opciones3;
    opciones3 << "El Gótico" << "El barroco" << "El Clasicismo" << "Romanticismo";
    opciones.enqueue(opciones3);

    // Pregunta 4
    preguntas.enqueue("Durante el renacimiento surge una nueva visión del hombre, que se vio reflejada en el arte, en la política y en las ciencias sociales y humanas, a lo que se denomina: ");
    respuestasCorrectas.enqueue("Humanismo");
    QStringList opciones4;
    opciones4 << "Antropocentrismo" << "Humanismo" << "Paradigma antropológico" << "Teocentrismo";
    opciones.enqueue(opciones4);

    // Pregunta 5
    preguntas.enqueue("Cuatro genios del renacimiento (Leonardo, Donatello, Rafael y Michelangelo) han sido llevados a la pantalla en los comics de:");
    respuestasCorrectas.enqueue("Las tortugas ninjas");
    QStringList opciones5;
    opciones5 << "Las tortugas ninjas" << "Los caballeros del Zodiaco" << "Los cuatro fantásticos" << "Los antagonistas de Attack on Titan";
    opciones.enqueue(opciones5);

    loadScene();

    timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, this, &JuegoArte::onTimeout);

    timerVisual = new QTimer(this);
    connect(timerVisual, &QTimer::timeout, this, &JuegoArte::actualizarTimer);

    reiniciarTimer();
}

void JuegoArte::loadScene()
{

    QPixmap bgPixmap("://res/bar.png");
    setBackground(bgPixmap.scaled(800,600 ));

    setupInteractiveObjects();
    setupInteractions();

}

void JuegoArte::setupInteractions()
{
    clearInteractiveAreas();

    for(int i=0;i<4;i++){
        addInteractiveArea(QRectF(160+i*150,260,114,108),"Click para seleccionar");
    }
}

void JuegoArte::mousePressEvent(QGraphicsSceneMouseEvent* event)
{

    qDebug()<<"click";
    QPointF pos = event->scenePos();

    // ✅ Obtener TODOS los items en la posición (ordenados por Z)
    QList<QGraphicsItem*> itemsAtPos = items(pos);


    // Buscar la primera bolsa cerrada clickeable
    for (QGraphicsItem* item : itemsAtPos) {

        // Buscar en bolsas cerradas
        for (int i = 0; i < bolsasCerradas.size(); ++i) {
            if (item == bolsasCerradas[i]) {
                onBolsaClicked(i);
                return; // Importante: salir después de procesar
            }
        }
    }

    // Propagar a la clase base
    QGraphicsScene::mousePressEvent(event);
}

void JuegoArte::setupInteractiveObjects()
{
    QPixmap glassPixmap("://res/Objects/glass.png");
    QGraphicsPixmapItem* glass = new QGraphicsPixmapItem(glassPixmap.scaled(46,105));
    glass->setPos(60,260);
    addObject(glass);

    QPixmap scrollPixmap("://res/Objects/scroll.png");
    QGraphicsPixmapItem* scroll = new QGraphicsPixmapItem(scrollPixmap.scaled(700,180));
    scroll->setPos(60,40);
    addObject(scroll);

    textoInstrucciones= new QGraphicsTextItem();
    textoInstrucciones->setPlainText("Diviertete preparando un delicioso coctel! Dale click a las respuestas correctas..si no, te arrepentiras");
    textoInstrucciones->setDefaultTextColor(Qt::black);
    textoInstrucciones->setFont(QFont("Minecraft",10,QFont::Bold));
    textoInstrucciones->setTextWidth(550);
    textoInstrucciones->setPos(140,100);
    textoInstrucciones->setZValue(10);
    addObject(textoInstrucciones);

    int x = 160;
    int y = 260;

    // Usar opciones.first() directamente
    if(!opciones.isEmpty()){
        QStringList opcionesActual = opciones.first();

        for(int i = 0; i < 4; i++){
            // BOLSA CERRADA
            QPixmap closedBagPixmap("://res/Objects/paperBag.png");
            QGraphicsPixmapItem* closedBag = new QGraphicsPixmapItem(closedBagPixmap.scaled(114,108));
            closedBag->setPos(x,y);
            closedBag->setData(0, i);
            closedBag->setCursor(QCursor(Qt::PointingHandCursor));
            closedBag->setAcceptedMouseButtons(Qt::LeftButton);
            addObject(closedBag);
            bolsasCerradas.append(closedBag);

            // BOLSA ABIERTA
            QPixmap openedBagPixmap("://res/Objects/openedBag.png");
            QGraphicsPixmapItem* openedBag = new QGraphicsPixmapItem(openedBagPixmap.scaled(114,108));
            openedBag->setPos(x,y);
            openedBag->setZValue(5);
            openedBag->setVisible(false);
            openedBag->setData(0, i);
            openedBag->setAcceptedMouseButtons(Qt::NoButton);
            addObject(openedBag);
            bolsasAbiertas.append(openedBag);

            // TEXTO - Usar opciones.first()[i]
            QGraphicsTextItem* textoOpcion = new QGraphicsTextItem();
            textoOpcion->setPlainText(opcionesActual[i]);
            textoOpcion->setDefaultTextColor(Qt::black);
            textoOpcion->setFont(QFont("Minecraft",10,QFont::Bold));
            textoOpcion->setTextWidth(83);
            textoOpcion->setPos(x+7,y+35);
            textoOpcion->setZValue(10);
            textoOpcion->setFlag(QGraphicsItem::ItemIsSelectable, false);
            textoOpcion->setFlag(QGraphicsItem::ItemIsFocusable, false);
            textoOpcion->setAcceptedMouseButtons(Qt::NoButton);
            addObject(textoOpcion);
            textosOpciones.append(textoOpcion);

            x += 150;
        }
    }

    textPregunta = new QGraphicsTextItem();
    textPregunta->setPlainText(preguntas.first()); // Usar first()
    textPregunta->setDefaultTextColor(Qt::white);
    textPregunta->setFont(QFont("Minecraft",18));
    textPregunta->setTextWidth(780);
    textPregunta->setPos(20,480);
    textPregunta->setZValue(20);
    addObject(textPregunta);

    textTiempo = new QGraphicsTextItem();
    textTiempo->setPlainText("30 segundos");
    textTiempo->setDefaultTextColor(Qt::white);
    textTiempo->setFont(QFont("Minecraft",15));
    textTiempo->setTextWidth(150);
    textTiempo->setPos(20,575);
    addObject(textTiempo);
}

void JuegoArte::cambiarOpciones()
{
    if(textosOpciones.size() == 4 && !opciones.isEmpty()){
        QStringList opcionesActual = opciones.first();
        for(int i = 0; i < 4; ++i){
            textosOpciones[i]->setPlainText(opcionesActual[i]);
        }
    }
}
void JuegoArte::actualizarTimer()
{
    if(time <= 0){
        timerVisual->stop();
        return;
    }
    time--;
    textTiempo->setPlainText(QString::number(time) + " segundos");
}

void JuegoArte::onTimeout()
{
    // Si se acaba el tiempo, contar como incorrecta
    mostrarResultado(false, -1);
}

void JuegoArte::reiniciarTimer()
{
    if (!preguntas.isEmpty()){ // Solo si hay preguntas
        textTiempo->setPlainText("30 segundos");
        time = 30;
        timer->stop();
        timerVisual->stop();

        timer->start(30000);
        timerVisual->start(1000);
    }
}

void JuegoArte::cambiarPregunta()
{
    if(preguntas.isEmpty()){
        terminarJuego();
        return;
    }

    // Usar first() para mostrar la pregunta actual
    textPregunta->setPlainText(preguntas.first());

    // Actualizar opciones
    cambiarOpciones();

    noPregunta++;
}
void JuegoArte::onBolsaClicked(int index)
{
    if (interactionBlocked || preguntas.isEmpty() || opciones.isEmpty()) return;
    interactionBlocked = true;

    // Guardar posiciones originales
    originalPositionsCerradas.clear();
    originalPositionsAbiertas.clear();
    originalPositionsTextos.clear();
    for (int i = 0; i < bolsasCerradas.size(); ++i) {
        originalPositionsCerradas.append(bolsasCerradas[i]->pos());
        originalPositionsAbiertas.append(bolsasAbiertas[i]->pos());
        originalPositionsTextos.append(textosOpciones[i]->pos());
    }

    // Mostrar solo la bolsa seleccionada y moverla al centro
    QPointF centerPos(300, 260);
    for (int i = 0; i < bolsasCerradas.size(); ++i) {
        if (i == index) {
            bolsasAbiertas[i]->setVisible(true);
            bolsasCerradas[i]->setVisible(false);
            textosOpciones[i]->setVisible(true);

            bolsasAbiertas[i]->setPos(centerPos);
            textosOpciones[i]->setPos(centerPos.x() + 7, centerPos.y() + 35);
        } else {
            bolsasAbiertas[i]->setVisible(false);
            bolsasCerradas[i]->setVisible(false);
            textosOpciones[i]->setVisible(false);
        }
    }

    // Determinar si la respuesta es correcta usando first()
    QString respuestaSeleccionada = opciones.first()[index];
    QString respuestaCorrecta = respuestasCorrectas.first();
    bool correcta = (respuestaSeleccionada == respuestaCorrecta);

    if(correcta){
        cantCorrectas++;
    }


    // Mostrar resultado
    mostrarResultado(correcta, index);
}

void JuegoArte::mostrarResultado(bool correcta, int index)
{
    timerVisual->stop();
    timer->stop();

    // Imagen de resultado
    QString rutaImagen = correcta ? "://res/Objects/blood.png" : "://res/Objects/garlic.png";
    QPixmap resultadoPixmap(rutaImagen);
    QGraphicsPixmapItem* resultadoItem = new QGraphicsPixmapItem(resultadoPixmap.scaled(120,100));

    // Posicionar imagen junto a la bolsa central
    QPointF posBolsa = bolsasAbiertas[index]->pos();
    resultadoItem->setPos(posBolsa.x() + 120, posBolsa.y() + 30);
    resultadoItem->setZValue(15);
    addObject(resultadoItem);

    textPregunta->setPlainText(correcta ? "Respuesta correcta" : "Respuesta incorrecta");

 QTimer::singleShot(5000, [=]() {
        removeItem(resultadoItem);
        delete resultadoItem;

        // Restaurar todas las bolsas y textos
        for (int i = 0; i < bolsasCerradas.size(); ++i) {
            bolsasCerradas[i]->setVisible(true);
            bolsasCerradas[i]->setPos(originalPositionsCerradas[i]);

            bolsasAbiertas[i]->setVisible(false);
            bolsasAbiertas[i]->setPos(originalPositionsAbiertas[i]);

            textosOpciones[i]->setVisible(true);
            textosOpciones[i]->setPos(originalPositionsTextos[i]);
        }

        if(!preguntas.isEmpty()) preguntas.dequeue();
        if(!respuestasCorrectas.isEmpty()) respuestasCorrectas.dequeue();
        if(!opciones.isEmpty()) opciones.dequeue();

        if(!preguntas.isEmpty()){
            cambiarPregunta();
            reiniciarTimer();
        } else {
            terminarJuego();
        }

        // Desbloquear interacción
        interactionBlocked = false;
    });
}
void JuegoArte::terminarJuego(){
    timerVisual->stop();
    timer->stop();

    for (QGraphicsPixmapItem* bolsa : bolsasCerradas) {
        bolsa->setVisible(false); // o bolsa->setVisible(false);
    }
    for (QGraphicsPixmapItem* bolsa : bolsasAbiertas) {
        bolsa->setVisible(false);  // o bolsa->setVisible(false);
    }
    for (QGraphicsTextItem* texto : textosOpciones) {
        texto->setVisible(false); // o texto->setVisible(false);
    }

    textPregunta->setVisible(false);
    textTiempo->setVisible(false);

    for (QGraphicsItem* item : items()) {
        if (QGraphicsPixmapItem* pixmapItem = dynamic_cast<QGraphicsPixmapItem*>(item)) {
            // Verificar si es el vaso por su posición o propiedades
            if (pixmapItem->pos() == QPointF(60, 260)) {
                pixmapItem->setVisible(false);  // o pixmapItem->setVisible(false);
                break;
            }
        }
    }

    QString ruta = (cantCorrectas==5) ? "://res/Objects/glassWin.png": "://res/Objects/glassLost.png";
    QPixmap resultadoFinalPixmap(ruta);
    QGraphicsPixmapItem*resultadoFinalItem = new QGraphicsPixmapItem(resultadoFinalPixmap.scaled(46,105));
    resultadoFinalItem->setPos(350,260);
    resultadoFinalItem->setZValue(15);
    addObject(resultadoFinalItem);


    textoInstrucciones->setPlainText((cantCorrectas==5)? "Disfruta de tu delicioso festin de sangre, has ganado!" :
                                     "Has perdido...");

    QTimer::singleShot(5000, [=]() {
        bool ganado = (cantCorrectas==5);
        qDebug() <<"arte ganado o perdido dentro de arte";
        emit juegoTerminado(ganado);
    });


}

