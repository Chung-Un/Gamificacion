#ifndef AULAKANTSCENE_H
#define AULAKANTSCENE_H

#include "scenebase.h"
#include <QStringList>
#include <QMap>
#include <QSet>
#include <QList>
#include <QChar>

class QGraphicsItemGroup;
class QGraphicsPixmapItem;
class QGraphicsRectItem;
class QGraphicsTextItem;
class QGraphicsView;

class AulaKantScene : public SceneBase {
    Q_OBJECT
public:
    explicit AulaKantScene(QObject* parent = nullptr);

    // HUD anclado a la view
    void initHud(QGraphicsView* view);

    // Preguntas por objeto (las muestra y las contesta)
    void mostrarPreguntaDesdeMensaje(const QString& mensaje); // llamado desde Game al pulsar E
    bool responderOpcionIfActivo(QChar letra);                // llamado desde Game al pulsar A/B/C/D
    bool togglePistasIfHud(QChar tecla);                      // llamado desde Game al pulsar V

    // Diálogo de Kant (barra en esquina inferior)
    void iniciarDialogoKant(const QStringList& lineas);
    bool avanzarDialogoSiActivo();

    QString getSceneName() const override { return "AulaKant"; }
    bool manejarTecla(int key);           // true si consumió la tecla
    bool preguntaEnCurso() const { return preguntaActiva; }
    void addPistaDesdeFuera(const QString& pista);
    void marcarRespondidoExterno(const QString& id);



signals:
    void dialogoIniciado();
    void dialogoTerminado();
    void aulaResuelta();          // emitir cuando aciertan la pregunta final

protected:
    void loadScene();
    void setupCollisions();
    void setupInteractions();
    void colocarProps();

private:
    QGraphicsPixmapItem* addProp(const QString& resPath, const QPointF& pos,
                                 const QSize& targetSize, qreal z = 0);

    // ---- Props ----
    QGraphicsPixmapItem *libreroItem = nullptr, *pizarraItem = nullptr, *posterItem = nullptr;
    QGraphicsPixmapItem *plantaL = nullptr, *plantaR = nullptr, *lamparaR = nullptr, *lamparaL = nullptr;
    QGraphicsPixmapItem *pupitre0 = nullptr, *pupitre1 = nullptr, *pupitre2 = nullptr, *kant = nullptr;

    // ---- HUD diálogo (abajo–izquierda, anclado a view) ----
    QGraphicsItemGroup* hudDialogGroup = nullptr;
    QGraphicsRectItem*  barraDialogo    = nullptr;
    QGraphicsTextItem*  textoDialogo    = nullptr;
    QGraphicsTextItem*  hintDialogo     = nullptr;

    // ---- HUD de pistas (arriba–derecha, anclado a view) ----
    QGraphicsItemGroup* hudPistasGroup  = nullptr;
    QGraphicsTextItem*  textoPistasHud  = nullptr;
    QGraphicsView*      attachedView    = nullptr;

    // ---- Preguntas y pistas ----
    struct Pregunta {
        QString enunciado;
        QStringList opciones;
        QChar correcta;
        QString pista;
    };
    QList<Pregunta> preguntas;                 // 6 preguntas
    QMap<QString,int> mapaObjetoPregunta;      // "planta1","planta2","poster","pup1","pup2","pup3"
    QSet<QString> objetosResueltos;
    QStringList pistasDesbloqueadas;

    // ---- UI modal de pregunta ----
    bool preguntaActiva = false;
    QString objetoActual;                       // key del objeto actual o "final"
    QGraphicsRectItem* panelPregunta = nullptr;
    QGraphicsTextItem*  txtPregunta   = nullptr;
    QList<QGraphicsTextItem*> txtOpciones;

    // ---- Overlay de pistas (V) ----
    bool overlayPistasVisible = false;
    QGraphicsRectItem* overlayPistas = nullptr;
    QList<QGraphicsTextItem*> overlayLineas;

    QSet<int> librosRespondidos;     // << NUEVO: 1 ó 2, respondidos (bien o mal)
    QSet<int> librosConPista;

    // ---- Diálogo Kant ----
    bool dialog_activo = false;
    QStringList dialog_lineas;
    int dialog_idx = 0;

    // en aulakantscene.h (privado)
    static constexpr int TOTAL_PISTAS_NECESARIAS = 6;
    bool finalMostrada = false;

    // declara:
    void checarLanzarFinal();

    // Respuestas externas (subescenas)
    QSet<QString> respondidosExternos;
    // Cambia el nombre si quieres, pero que signifique "total de respuestas requeridas"
    static constexpr int TOTAL_RESPUESTAS_NECESARIAS = 6;

    // Marca una respuesta hecha desde subescena (librero/pizarra)
    QChar finalCorrecta = 'A';
    QString finalEnunciado;
    QStringList finalOpciones;

    // Cambia la firma para no depender de constantes globales:


private slots:
    // Reposicionar HUDs cuando se mueva la cámara
    void updateHudPos();

    // Diálogo
    void mostrarBarraDialogo();
    void ocultarBarraDialogo();
    void refrescarLineaActual();
    void reconstruirOpcionesFinal(const QStringList& opciones);

    // Preguntas
    void construirBancoPreguntas();
    void mostrarPregunta(int idx);
    void ocultarPregunta();
    void resolverPregunta(bool correcta);
    void mostrarPreguntaFinal();         // llamada cuando se resuelvan los 6 objetos
    void actualizarHudPistas();

    // Overlay de pistas
    void mostrarOverlayPistas();
    void ocultarOverlayPistas();
};

#endif // AULAKANTSCENE_H
