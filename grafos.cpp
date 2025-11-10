#include "Grafos.h"
#include <stdexcept>
#include <iostream>

Grafos::Grafos(int maxVertices)
    : capacidad(maxVertices),
    numVertices(0),
    vertices(maxVertices),
    matrizA(maxVertices, std::vector<int>(maxVertices, 0)),
    listaA(maxVertices) {}

int Grafos::encontrarIndiceVertice(const std::string& nombre) const {
    for (int i = 0; i < numVertices; ++i)
        if (vertices[i].getNombre() == nombre) return i;
    return -1;
}

void Grafos::verificarIndice(int indice) const {
    if (indice < 0 || indice >= numVertices)
        throw std::out_of_range("Indice de vértice inválido");
}

void Grafos::agregarVertice(const std::string& nombre) {
    if (estaLleno()) throw std::runtime_error("Capacidad maxima del grafo alcanzada");
    if (encontrarIndiceVertice(nombre) != -1) throw std::runtime_error("El vertice ya existe");

    vertices[numVertices] = Vertice(nombre, numVertices);
    ++numVertices;
}

void Grafos::agregarArco(const std::string& origen, const std::string& destino, int peso) {
    int idOrigen  = encontrarIndiceVertice(origen);
    int idDestino = encontrarIndiceVertice(destino);
    if (idOrigen  == -1) throw std::runtime_error("Vertice origen no encontrado");
    if (idDestino == -1) throw std::runtime_error("Vertice destino no encontrado");

    matrizA[idOrigen][idDestino] = peso;

    bool encontrado = false;
    for (auto it = listaA[idOrigen].begin(); it != listaA[idOrigen].end(); ++it) {
        if (it->first == idDestino) { it->second = peso; encontrado = true; break; }
    }
    if (!encontrado) listaA[idOrigen].push_back(std::make_pair(idDestino, peso));
}

bool Grafos::sonAdyacentes(const std::string& origen, const std::string& destino) const {
    int idOrigen  = encontrarIndiceVertice(origen);
    int idDestino = encontrarIndiceVertice(destino);
    if (idOrigen == -1 || idDestino == -1) throw std::runtime_error("Vertice no encontrado");
    return matrizA[idOrigen][idDestino] != 0;
}

int Grafos::getPesoArco(const std::string& origen, const std::string& destino) const {
    int idOrigen  = encontrarIndiceVertice(origen);
    int idDestino = encontrarIndiceVertice(destino);
    if (idOrigen == -1 || idDestino == -1) throw std::runtime_error("Vertice no encontrado");
    return matrizA[idOrigen][idDestino];
}

const Vertice& Grafos::getVertice(int indice) const {
    verificarIndice(indice);
    return vertices[indice];
}

const Vertice& Grafos::getVertice(const std::string& nombre) const {
    int idx = encontrarIndiceVertice(nombre);
    if (idx == -1) throw std::runtime_error("Vertice no encontrado");
    return vertices[idx];
}

void Grafos::imprimirMatriz() const {
    std::cout << "---- MATRIZ DE ADYACENCIA ----\n   ";
    for (int i = 0; i < numVertices; ++i) std::cout << vertices[i].getNombre() << " ";
    std::cout << "\n";
    for (int f = 0; f < numVertices; ++f) {
        std::cout << vertices[f].getNombre() << ": ";
        for (int c = 0; c < numVertices; ++c) std::cout << matrizA[f][c] << " ";
        std::cout << "\n";
    }
}

void Grafos::imprimirLista() const {
    std::cout << "---- LISTA DE ADYACENCIA ----\n";
    for (int i = 0; i < numVertices; ++i) {
        std::cout << vertices[i].getNombre() << " -> ";
        for (auto it = listaA[i].begin(); it != listaA[i].end(); ++it)
            std::cout << vertices[it->first].getNombre() << "(" << it->second << ") ";
        std::cout << "\n";
    }
}

const std::list<std::pair<int,int>>&
Grafos::adyacentesDe(const std::string& nombre) const {
    int id = encontrarIndiceVertice(nombre);
    if (id == -1) throw std::runtime_error("Vertice no encontrado");
    return listaA[id];
}
