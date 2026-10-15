#ifndef ARBOL_EXPRESIONES_HPP
#define ARBOL_EXPRESIONES_HPP

#include "Estructuras.hpp"

#include <vector>

class Nodo{
private:
    Token dato; 
    Nodo *izq, *der;
public:
    Nodo(Token t) : dato(t), izq(nullptr), der(nullptr) {} 
    ~Nodo();

    Nodo(const Nodo&) = delete; // prohibe crear un Nodo copiando otro
    Nodo& operator=(const Nodo&) = delete; // prohibe asignar un Nodo a otro

    Token getDato() const;
    Nodo* getIzq() const;
    Nodo* getDer() const;
    
    void setIzq(Nodo* iz);
    void setDer(Nodo* dr);

    Nodo* clonar() const;
};

class ArbolExpresiones{
private:
    Nodo* raiz;
    
    int prioridad(const Token &t);
    bool asociativoDerecha(const Token &t);
    std::vector<Token> transfPostfija(const std::vector<Token> &tokens);

    double evaluarNodo(Nodo* n, double x) const;
public:
    ArbolExpresiones() : raiz(nullptr) {}
    ~ArbolExpresiones() { delete raiz; }
    
    ArbolExpresiones(const ArbolExpresiones&) = delete;
    ArbolExpresiones& operator=(const ArbolExpresiones&) = delete;
    
    const Nodo* getRaiz() const;

    void construirArbol(const std::vector<Token> &tokens);
    double evaluar(double x) const;

    void adoptar(Nodo* nueva);
};

#endif