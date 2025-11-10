#ifndef GRAFOS_H
#define GRAFOS_H

#include "Vertice.h"
#include <vector>
#include <list>
#include <string>
#include <utility>   // std::pair

class Grafos {
private:
    int capacidad;
    int numVertices;
    std::vector<Vertice> vertices;
    std::vector<std::vector<int>> matrizA;
    std::vector<std::list<std::pair<int,int>>> listaA;

    int  encontrarIndiceVertice(const std::string& nombre) const;
    void verificarIndice(int indice) const;

public:
    explicit Grafos(int maxVertices);

    // básicos
    int  getNumVertices() const { return numVertices; }
    int  getCapacidad()  const { return capacidad; }
    bool estaLleno()     const { return numVertices >= capacidad; }

    void agregarVertice(const std::string& nombre);
    void agregarArco(const std::string& origen, const std::string& destino, int peso = 1);

    bool sonAdyacentes(const std::string& origen, const std::string& destino) const;
    int  getPesoArco(const std::string& origen, const std::string& destino) const;

    const Vertice& getVertice(int indice) const;
    const Vertice& getVertice(const std::string& nombre) const;

    // depuración
    void imprimirMatriz() const;
    void imprimirLista() const;

    // helper para el mapa
    const std::list<std::pair<int,int>>& adyacentesDe(const std::string& nombre) const;
};

#endif // GRAFOS_H
