#ifndef VERTICE_H
#define VERTICE_H

#include <string>

class Vertice {
private:
    std::string nombre;
    int id;

public:
    Vertice() : nombre(""), id(-1) {}
    explicit Vertice(const std::string& nombre) : nombre(nombre), id(-1) {}
    Vertice(const std::string& nombre, int id) : nombre(nombre), id(id) {}

    const std::string& getNombre() const { return nombre; }
    int getId() const { return id; }

    void setNombre(const std::string& nuevo) { nombre = nuevo; }
    void setId(int nId) { id = nId; }

    bool operator==(const Vertice& otro) const { return nombre == otro.nombre; }
};

#endif // VERTICE_H
