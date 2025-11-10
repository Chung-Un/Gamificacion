#ifndef VERTICE_H
#define VERTICE_H

#include <string>
using namespace std;

class Vertice{
private:
    string nombre;
    int id;
public:
    Vertice():nombre(""),id(-1){}            // ctor default
    Vertice(const string& nombre):nombre(nombre),id(-1){}
    Vertice(const string& nombre, int id):nombre(nombre),id(id){}

    const string getNombre() const { return nombre; }
    const int    getId() const     { return id; }

    void setNombre(const string& nuevo){ nombre = nuevo; }
    void setId(int nId){ id = nId; }

    bool operator==(const Vertice& otro) const { return nombre == otro.nombre; }
};

#endif
