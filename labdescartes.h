#ifndef LABDESCARTES_H
#define LABDESCARTES_H

#include "scenebase.h"

#include <QSet>
#include <QVector>
#include <QPixmap>
#include <QSize>
#include <QPointF>
#include <QTimer>
#include <QStringList>

class QEvent;
class QGraphicsView;
class QGraphicsItemGroup;
class QGraphicsPixmapItem;
class QGraphicsRectItem;
class QGraphicsTextItem;

class LabDescartes : public SceneBase {
    Q_OBJECT
public:
    explicit LabDescartes(QObject* parent=nullptr);

    enum class Phase { EVIDENCIA=0, ANALISIS, SINTESIS, ENUMERACION, FINAL_READY, FINISHED };
    QString getSceneName() const override { return "LabDescartes"; }


    // SceneBase overrides
    void loadScene() override;
    void colocarProps() ;
    void setupCollisions() ;
    void setupInteractions() ;
    void initHud(QGraphicsView* view) ;
    bool manejarTecla(int key) ;
    bool eventFilter(QObject* obj, QEvent* ev) ;

    // Diálogo e interacciones
    void iniciarDialogoDescartes(const QStringList& lineas);
    bool avanzarDialogoSiActivo();
    bool tryHandleInteraction(const QString& msg);
    void marcarRespondidoExterno(const QString& id);


signals:
    void laboratorioResuelto();

private:
    // ---- Fases / gating ----
    // En labdescartes.h (valores por defecto en la declaración)
    Phase phase = Phase::EVIDENCIA;

    bool haveEvidencia = false;
    bool haveEstatua   = false;
    bool haveLazo      = false;
    bool haveOrden     = false;

    // Nuevo: visibilidad del HUD de inventario al presionar V
    bool inventoryVisible = true;



    // ---- Inventario + HUD ----
    QGraphicsItemGroup* hudInvGroup=nullptr;
    QGraphicsTextItem* hudInvTitle = nullptr;
    QGraphicsPixmapItem *iconEvid=nullptr, *iconEstatua=nullptr, *iconLazo=nullptr, *iconOrden=nullptr;
    void updateInventoryHud();
    void showEvidenceGrid();
    void showAnalysisQuestion();
    void showSynthesisQuestion();
    void showEnumerationQuestion();

    // ---- HUD diálogo anclado ----
    QGraphicsItemGroup* hudDialogGroup=nullptr;
    QGraphicsRectItem*  barraDialogo=nullptr;
    QGraphicsTextItem*  textoDialogo=nullptr;
    QGraphicsTextItem*  hintDialogo=nullptr;
    void mostrarBarraDialogo();
    void ocultarBarraDialogo();
    void refrescarLineaActual();
    void posicionarHud();

    QStringList dialog_lineas;
    int  dialog_idx = 0;
    bool dialog_activo = false;

    // ---- Pregunta final ----
    bool finalMostrada=false;
    QGraphicsRectItem* panelPregunta=nullptr;
    QGraphicsTextItem* txtPregunta=nullptr;
    QList<QGraphicsTextItem*> txtOpciones;
    void mostrarPreguntaFinal() ;
    void ocultarPregunta();
    void checarLanzarFinal();

    // ---- Panel “focus” por mesa ----
    bool panelActivo=false;
    QGraphicsRectItem* panelFocus=nullptr;
    QGraphicsTextItem* txtFocus=nullptr;
    QVector<QGraphicsTextItem*> focusOpciones;
    QChar respuestaCorrecta='A';
    void showQuestion(const QString& enunciado,
                      const QStringList& opciones,
                      QChar correcta,
                      const QList<QPixmap>& extras = {});
    void closeQuestion();
    void resolveAnswer(QChar letter);
    void advancePhase();
    void maybePromptTalkToDescartes();
    void startBatAnimation();
    void stopBatAnimation();// En labdescartes.h (privado)
    void clearPanelFocusChildren();
    void recreatePanelFocus(); // destruye el panel previo (si existe) y crea uno nuevo


    // ---- Props y animación ----
    QGraphicsPixmapItem *lamparaL=nullptr, *lamparaR=nullptr;
    QGraphicsPixmapItem *poster1=nullptr, *poster2=nullptr;
    QGraphicsPixmapItem *mesaEvidencia=nullptr, *mesaAnalisis=nullptr, *mesaSintesis=nullptr, *mesaEnumeracion=nullptr;

    QVector<QPixmap> batFrames;
    QSize  batTargetSize = QSize(100,80);
    QPointF batBasePos = QPointF(460,320);
    QGraphicsPixmapItem* descartesItem=nullptr;
    QTimer batTimer;
    int batFrameIndex=0;
    int bobTick=0;
    bool resolvingAnswer=false;

    // ---- Otros ----
    QSet<QString> respondidosExternos;
    // Para imágenes de preguntas específicas
    QGraphicsPixmapItem* analysisPic = nullptr; // pista en ANÁLISIS
    QGraphicsPixmapItem* enumPic     = nullptr; // imagen que cambia en ENUMERACIÓN

    QGraphicsView* attachedView=nullptr;
};

#endif // LABDESCARTES_H
