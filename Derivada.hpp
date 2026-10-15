#ifndef DERIVADA_HPP
#define DERIVADA_HPP

#include "Arbol_Expresiones.hpp"

#include <memory>

using Ptr = std::unique_ptr<Nodo>;

class Derivador {
private:
    Ptr derivarNodo(const Nodo* n);

    Ptr numero(double v);
    Ptr clonar(const Nodo* n);
    bool contieneX(const Nodo* n);
    bool esNum(const Nodo* n, double v);
    double valorConst(const Nodo* n);

    Ptr suma(Ptr a, Ptr b);
    Ptr resta(Ptr a, Ptr b);
    Ptr mult(Ptr a, Ptr b);
    Ptr neg(Ptr a);
    Ptr potencia(Ptr base, double n);
    Ptr funcion(std::string nombre, Ptr arg);

    Ptr nodoBinario(const std::string& operador, Ptr izq, Ptr der);
    Ptr nodoUnario(TipoToken tipo, const std::string& valor, Ptr hijo);

public:
    Nodo* derivar(const ArbolExpresiones& original);
};

#endif