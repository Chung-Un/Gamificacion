#ifndef RULETASCENE_H
#define RULETASCENE_H

#include "scenebase.h"

#include <QGraphicsItemGroup>
#include <QGraphicsPathItem>
#include <QGraphicsPolygonItem>
#include <QGraphicsLineItem>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsTextItem>
#include <QGraphicsView>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QTimer>
#include <QScrollBar>

class RuletaScene : public SceneBase
{
    Q_OBJECT
public:
    explicit RuletaScene(QObject *parent = nullptr);

    QString getSceneName() const override { return "RuletaScene"; }
    void loadScene() override;

    // UX
    void girarRuleta();
    void detenerRuleta();
    void setSeccionEnJuego(int idx);

    // HUD anclado al viewport
    void initHud(QGraphicsView* view);

    // Resultado de minijuegos
    void agregarJuegoGanado();
    void agregarJuegoPerdido();

    // Vida inicial para batalla final (úsala desde .cpp)
    int vidaInicialCalculada() const { return juegosGanados * 25; }
    // Debug
    void debugVisualRuleta();
    void debugAbrirTodasLasPuertas();
    void debugGanarTodoYElegirBando();
    void debugPerderTodoYReiniciar();

signals:
    void bandoSeleccionado(bool esEmpirista, int vidaInicial);
    // **Nombre correcto** (con 'Al' en mayúscula y A-L):
    void irAlMapaTrasBando(bool esEmpirista, int vidaInicial);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void keyPressEvent(QKeyEvent* event) override;

public slots:
    void confirmarBandoYVolverAlMapa();

private slots:
    void actualizarAnimacionRuleta();
    void updateHudPos();
    void mostrarSeleccionBando();
    void seleccionarBando(bool esEmpirista);

private:
    // Setup
    void setupCollisions();
    void setupInteractiveObjects();
    void setupInteractions();

    // Lógica de ruleta/puertas
    void procesarSeleccion(int seccion);
    void marcarSeccionUsada(int seccion);
    void abrirPuerta(int seccion);
    void cerrarPuerta(int seccion);

    // Progresión y reinicio
    void registrarResultadoJuego(bool gano);
    void reiniciarRuletaYPuertas();

    // UI de progreso
    void actualizarCorazones();
    void cambiarCorazonLleno(int seccion); // si la usas

    // UI elegir bando
    void limpiarSeleccionBando();

    // ---- Estado interno ----
    // Ruleta
    QGraphicsItemGroup*      ruletaGroup = nullptr;
    QGraphicsPathItem*       ruleta      = nullptr;
    QGraphicsPolygonItem*    ruletaArrow = nullptr;
    QGraphicsItem*           legs        = nullptr; // si no usas, se queda null
    QTimer*                  animacionTimer = nullptr;

    double anguloRotacion  = 0;
    double velocidadRotacion = 0;
    double anguloFinal     = 0;
    int    seccionesGiro   = 0;
    int    seccionSeleccionada = -1;
    int seccionEnJuego = -1;   // NEW: sección que realmente se jugó
    bool   ruletaGirando   = false;

    // Puertas / textos
    QVector<QGraphicsPixmapItem*> puertasItems;         // cerradas
    QVector<QGraphicsPixmapItem*> puertasAbiertasItems; // abiertas
    QVector<QGraphicsSimpleTextItem*> textosRuleta;     // ARTE, CIENCIA...
    QVector<QGraphicsLineItem*> tachadosItems;          // línea sobre texto

    // Estados
    QVector<bool> seccionesUsadas;          // para tachar/apagar el texto al salir en ruleta
    QVector<bool> puertaAbiertaParaJugar;   // si se puede entrar (después de salir en ruleta)
    QVector<bool> seccionCompletada;        // se cerró tras jugar

    // Corazones HUD
    QGraphicsItemGroup* hudHeartsGroup = nullptr;
    QVector<QGraphicsPixmapItem*> corazones, corazonesVacios;
    QGraphicsView* attachedView = nullptr;

    // Progresión de juegos
    int juegosGanados = 0;
    int juegosJugados = 0;
    const int totalJuegos = 4;

    // UI elegir bando
    bool mostrarBandos = false;
    bool bandoElegidoEsEmpirista = true;
    int  vidaBase   = 4;
    int  bonusVida  = 0;
    bool puedeConfirmar = false;
    QGraphicsPixmapItem* badgeEmpirista = nullptr;
    QGraphicsPixmapItem* badgeRacionalista = nullptr;
    QGraphicsTextItem*   textoSeleccionBando = nullptr;
};

#endif // RULETASCENE_H
