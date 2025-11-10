#include "videoscene.h"
#include <QGraphicsTextItem>
#include <QPixmap>
#include <QDir>
#include <QCoreApplication>
#include <QUrl>

VideoScene::VideoScene(QObject* parent)
    : SceneBase(parent)
{
    loadScene();
}

void VideoScene::loadScene() {
    setSceneRect(0, 0, 800, 600);

    QPixmap bg(":/res/castle.jpg");
    if (!bg.isNull()) {
        setBackground(bg.scaled(sceneRect().size().toSize(),
                                Qt::KeepAspectRatioByExpanding,
                                Qt::SmoothTransformation));
    }

    videoItem = new QGraphicsVideoItem;
    videoItem->setSize(QSizeF(800, 600));
    videoItem->setZValue(5);
    addItem(videoItem);
    videoItem->setPos(
        (sceneRect().width()  - 800) / 2.0,
        (sceneRect().height() - 600) / 2.0
        );

    player = new QMediaPlayer(this);
    audio  = new QAudioOutput(this);
    player->setAudioOutput(audio);
    audio->setVolume(0.8);
    player->setVideoOutput(videoItem);

    auto* hintBg = new QGraphicsRectItem(0, 0, 800, 32);
    hintBg->setBrush(QColor(0, 0, 0, 140));
    hintBg->setPen(Qt::NoPen);
    hintBg->setZValue(8);
    hintBg->setPos(0, 568);
    addItem(hintBg);

    auto* hint = new QGraphicsTextItem("Presiona ESPACIO para saltarte el video");
    hint->setDefaultTextColor(Qt::white);
    hint->setFont(QFont("Minecraft", 12, QFont::Bold));
    hint->setZValue(9);
    addItem(hint);
    const qreal hx = (sceneRect().width() - hint->boundingRect().width()) / 2.0;
    hint->setPos(hx, 572);

    startVideo();

    connect(player, &QMediaPlayer::errorOccurred, this, [this](QMediaPlayer::Error){
        auto* t = new QGraphicsTextItem(
            "No se pudo cargar el video.\nPulsa ESPACIO para continuar.");
        t->setDefaultTextColor(Qt::white);
        t->setFont(QFont("Minecraft", 20, QFont::Bold));
        addItem(t);
        t->setZValue(10);
        const QRectF r = t->boundingRect();
        t->setPos((sceneRect().width()-r.width())/2.0,
                  (sceneRect().height()-r.height())/2.0);
    });

    connect(player, &QMediaPlayer::mediaStatusChanged, this,
            [this](QMediaPlayer::MediaStatus st){
                if (st == QMediaPlayer::EndOfMedia) emit videoTerminado();
            });
}


void VideoScene::startVideo() {
    const QString abs = "C:/Users/chung/OneDrive/Documents/Estru2Parcial1Qt/Video.mp4";
    player->setSource(QUrl::fromLocalFile(abs));
    player->play();


}

QString VideoScene::resolveVideoPath() const {
    return QDir(QCoreApplication::applicationDirPath()).filePath("res/Video.mp4");
}

void VideoScene::keyPressEvent(QKeyEvent* e) {
    if (e->key() == Qt::Key_Space) {
        if (player) player->stop();
        emit videoTerminado();
        return;
    }
    SceneBase::keyPressEvent(e);
}
