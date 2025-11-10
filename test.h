#ifndef TEST_H
#define TEST_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QTimer>
#include <QList>
#include <QKeyEvent>
#include <QGraphicsTextItem>
#include <QKeyEvent>

class Test : public QMainWindow
{
    Q_OBJECT

public:
    Test(QWidget *parent = nullptr);
    ~Test();

protected:
    void keyPressEvent(QKeyEvent * event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private slots:
    void updateAnimation();

private:
    QGraphicsView *view;
    QGraphicsScene * scene;
    QGraphicsPixmapItem* player;
    QGraphicsPixmapItem* background;

    QGraphicsTextItem * interactionText;
    bool isInteracting;
    QString currentInteraction;

    void loadPlayerSprites();
    void updatePlayerDirection();
    void checkCollisions();
    void checkInteractions();
    void showInteractionText(const QString &text);
    void hideInteractionText();
    void updateMovement();

    QVector<QPixmap> idleFrames;
    QVector<QPixmap> walkupFrames;
    QVector<QPixmap> walkdownFrames;
    QVector<QPixmap> walkleftFrames;
    QVector<QPixmap> walkrightFrames;
    int currentFrame;
    QTimer* animationTimer;
    QTimer * movementTimer;

    double playerX,playerY;
    bool movingUp,movingDown,movingLeft,movingRight;

    QList<QRectF> interactiveAreas;
    QList<QString> interactionMessage;
    QList<QRectF> collisionAreas;

    enum class Direction {UP,DOWN,LEFT,RIGHT,IDLE};
    Direction currentDirection;
    Direction lastDirection;

};
#endif // TEST_H
