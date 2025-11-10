#ifndef VIDEOSCENE_H
#define VIDEOSCENE_H

#include "scenebase.h"
#include <QGraphicsVideoItem>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QKeyEvent>

class VideoScene : public SceneBase {
    Q_OBJECT
public:
    explicit VideoScene(QObject* parent = nullptr);

    QString getSceneName() const override { return "VideoScene"; }

signals:
    void videoTerminado();

protected:
    void loadScene() override;
    void setupCollisions()  {}
    void setupInteractions()  {}
    void keyPressEvent(QKeyEvent* e) override;

private:
    QGraphicsVideoItem* videoItem = nullptr;
    QMediaPlayer* player = nullptr;
    QAudioOutput* audio = nullptr;

    void startVideo();
    QString resolveVideoPath() const; // busca "<exe>/res/Video.mp4"
};

#endif // VIDEOSCENE_H
