#include "test.h"
#include <QApplication>

Test::Test(QWidget *parent)
    : QMainWindow(parent)
    ,playerX(400),playerY(500)
    ,scene(nullptr)
    ,background(nullptr)
    ,player(nullptr)
    ,view(nullptr)
    ,currentFrame(0),currentDirection(Direction::IDLE)
    ,lastDirection(Direction::DOWN)
    ,movingUp(false),movingDown(false),movingLeft(false)
    ,movingRight(false)
    ,interactionText(nullptr),isInteracting(false)
{
    scene = new QGraphicsScene(this);
    scene->setSceneRect(0,0,1000,1000);

    QPixmap bgPixmap("://res/terrace.jpg");
    background = new QGraphicsPixmapItem(bgPixmap.scaled(1000,1000));
    scene->addItem(background);

    player = new QGraphicsPixmapItem();
    player->setPos(playerX,playerY);
    scene->addItem(player);

    loadPlayerSprites();

    view = new QGraphicsView(scene,this);
    view->setFixedSize(800,600);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setCentralWidget(view);
    view->centerOn(player);

    interactiveAreas<< QRectF(970,360,50,225);

    interactionMessage<<"Presiona e para salir del area";

    interactionText = new QGraphicsTextItem();
    interactionText->setDefaultTextColor(Qt::white);
    interactionText->setFont(QFont("System",14,QFont::Bold));
    interactionText->setZValue(1000);
    interactionText->setVisible(false);

    QGraphicsRectItem* textBackground = new QGraphicsRectItem();
    textBackground->setBrush(QBrush(QColor(0,0,0,180)));
    textBackground->setPen(Qt::NoPen);
    textBackground->setZValue(1000);
    textBackground->setVisible(false);

    scene->addItem(textBackground);
    scene->addItem(interactionText);

    collisionAreas << QRectF(35,300,1000,-250)
                   << QRectF(980,360,30,225)
                   <<  QRectF(35,380,-30,220); //donde empieza en x,y, anchura, altura*/



    //ver colisiones
    // for (const QRectF &area : collisionAreas) {
    //     QGraphicsRectItem *collisionBox = new QGraphicsRectItem(area);
    //     collisionBox->setBrush(QBrush(Qt::red));
    //     collisionBox->setOpacity(0.3);
    //     scene->addItem(collisionBox);
    // }


    for (int i = 0; i < interactiveAreas.size(); i++) {
        QGraphicsRectItem *interactiveBox = new QGraphicsRectItem(interactiveAreas[i]);
        interactiveBox->setBrush(QBrush(Qt::green));
        interactiveBox->setOpacity(0.3);
        scene->addItem(interactiveBox);

        // Etiqueta para identificar
        QGraphicsTextItem *label = new QGraphicsTextItem(QString("Objeto %1").arg(i+1));
        label->setPos(interactiveAreas[i].x(), interactiveAreas[i].y() - 20);
        label->setDefaultTextColor(Qt::green);
        scene->addItem(label);
    }

    animationTimer = new QTimer(this);
    connect(animationTimer,&QTimer::timeout, this, &Test::updateAnimation);
    animationTimer->start(100);

    movementTimer = new QTimer(this);
    connect(movementTimer, &QTimer::timeout, this, &Test::updateMovement);
    movementTimer->start(50);



}

Test::~Test()
{
}
void Test::loadPlayerSprites(){
    idleFrames.clear();
    walkdownFrames.clear();
    walkupFrames.clear();
    walkleftFrames.clear();
    walkrightFrames.clear();

    for(int i=1;i<=6;i++){
        QString path = QString("://res/Player/idleGirl%1.png").arg(i);
        QPixmap frame(path);
        if(!frame.isNull()){
            idleFrames<<frame.scaled(110,150);
        }
    }

    for(int i=1;i<=6;i++){
        QString path = QString("://res/Player/leftGirl%1.png").arg(i);
        QPixmap frame(path);
        if(!frame.isNull()){
            walkleftFrames<<frame.scaled(110,150);
        }
    }

    for(int i=1;i<=6;i++){
        QString path = QString("://res/Player/rightGirl%1.png").arg(i);
        QPixmap frame(path);
        if(!frame.isNull()){
            walkrightFrames<<frame.scaled(110,150);
        }
    }

    for(int i=1;i<=6;i++){
        QString path = QString("://res/Player/rightGirl%1.png").arg(i);
        QPixmap frame(path);
        if(!frame.isNull()){
            walkupFrames<<frame.scaled(110,150);
        }
    }

    for(int i=1;i<=6;i++){
        QString path = QString("://res/Player/leftGirl%1.png").arg(i);
        QPixmap frame(path);
        if(!frame.isNull()){
            walkdownFrames<<frame.scaled(110,150);
        }
    }
}

void Test::updateAnimation(){
    QVector<QPixmap> * currentAnimation = nullptr;
    bool isMoving = false;

    // Determinar la animación actual basada en el movimiento
    if(movingUp){
        currentAnimation = &walkupFrames;
        lastDirection = Direction::UP;
        isMoving = true;
    } else if(movingDown){
        currentAnimation = &walkdownFrames;
        lastDirection = Direction::DOWN;
        isMoving = true;
    } else if(movingLeft){
        currentAnimation = &walkleftFrames;
        lastDirection = Direction::LEFT;
        isMoving = true;
    } else if(movingRight){
        currentAnimation = &walkrightFrames;
        lastDirection = Direction::RIGHT;
        isMoving = true;
    }

    // Si no hay movimiento, usar animación idle o frame estático de última dirección
    if(!isMoving){
        currentAnimation = &idleFrames;  // O usa la última dirección si prefieres
        currentFrame = 0;  // Solo reinicia cuando NO se mueve
    } else {
        // Avanzar frame solo cuando hay movimiento
        currentFrame = (currentFrame + 1) % 6;
    }

    // Actualizar sprite
    if(currentAnimation && !currentAnimation->isEmpty() && currentFrame < currentAnimation->size()){
        player->setPixmap((*currentAnimation)[currentFrame]);
    }
}

void Test::keyReleaseEvent(QKeyEvent *event){
    if(event->isAutoRepeat()) return;

    switch(event->key()){
        case Qt::Key_W: movingUp = false; break;
        case Qt::Key_S: movingDown = false; break;
        case Qt::Key_A: movingLeft = false; break;
        case Qt::Key_D: movingRight = false; break;
    }

    updatePlayerDirection();
}

void Test::updatePlayerDirection(){
    if (movingUp) currentDirection = Direction::UP;
    else if (movingDown) currentDirection = Direction::DOWN;
    else if (movingLeft) currentDirection = Direction::LEFT;
    else if (movingRight) currentDirection = Direction::RIGHT;
    else currentDirection = Direction::IDLE;
}

void Test::checkCollisions(){
    QRectF playerRect(playerX,playerY,110,150);

    for(const QRectF &obstacle: collisionAreas){
        if(playerRect.intersects(obstacle)){
            qDebug()<< "colision";
            break;
        }
    }
}
void Test::keyPressEvent(QKeyEvent *event){
    if(event->isAutoRepeat()) return;

    switch(event->key()){
        case Qt::Key_W: movingUp = true; break;
        case Qt::Key_S: movingDown = true; break;
        case Qt::Key_A: movingLeft = true; break;
        case Qt::Key_D: movingRight = true; break;
        case Qt::Key_Space:  // Add spacebar to print position
            qDebug() << "Player position - X:" << playerX << "Y:" << playerY;
            return;
        case Qt::Key_E:
            if(isInteracting){
                qDebug() <<"interactuo";
            }
            return;
    }

    updatePlayerDirection();
}

void Test::updateMovement(){
    if(!movingUp && !movingDown && !movingLeft && !movingRight) return;

    double step= 15;
    double newX = playerX;
    double newY = playerY;

    if (movingUp) newY -= step;
    if (movingDown) newY += step;
    if (movingLeft) newX -= step;
    if (movingRight) newX += step;

    newX= qMax(30.0,qMin(newX,970.0));
    newY = qMax(30.0, qMin(newY, 970.0));

    double originalX = playerX;
    double originalY = playerY;

    playerX = newX;
    playerY = newY;

    checkCollisions();
    checkInteractions();

    QRectF playerRect(playerX, playerY, 110, 150);
    for (const QRectF &obstacle : collisionAreas) {
        if (playerRect.intersects(obstacle)) {
            playerX = originalX;
            playerY = originalY;
            break;
        }
    }

    player->setPos(playerX, playerY);

    if(isInteracting){
        double textX = playerX - (interactionText->boundingRect().width()/2);
        double textY = playerY-30;
        interactionText->setPos(textX,textY);
    }
    view->centerOn(player);
}

void Test::checkInteractions(){
    QRectF playerRect(playerX,playerY,110,150);
    bool foundInteraction = false;

    for (int i = 0; i < interactiveAreas.size(); i++) {
        if (playerRect.intersects(interactiveAreas[i])) {
            showInteractionText(interactionMessage[i]);
            currentInteraction = interactionMessage[i];
            foundInteraction = true;
            break;
        }
    }

    if(!foundInteraction && isInteracting){
        hideInteractionText();
    }
}

void Test::showInteractionText(const QString &text){
    if(!isInteracting){
        interactionText->setPlainText(text);

        double textX = playerX - (interactionText->boundingRect().width()/2);
        double textY = playerY - 30;

        interactionText->setPos(textX,textY);
        interactionText->setVisible(true);
        isInteracting= true;

    }
}

void Test::hideInteractionText(){
    interactionText->setVisible(false);
    isInteracting = false;
    currentInteraction.clear();
}
