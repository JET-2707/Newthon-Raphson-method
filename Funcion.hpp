#ifndef FUNCION_H
#define FUNCION_H

#include <string>
#include <vector>

enum class TipoToken {
    NUMERO, VARIABLE, OPERADOR, PARENTESIS, FUNCION
};

struct Token{
    TipoToken tipo;
    std::string valor;
};

class Nodo{
private:
    Token dato; // Operador, número, variable o función
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

class Funcion{
private:
    std::string expresion;
    Nodo* raiz;
public:
    Funcion() : expresion(""), raiz(nullptr) {} 
    ~Funcion();

    Funcion(const Funcion&) = delete; // prohibe crear una Funcion copiando otra
    Funcion& operator=(const Funcion&) = delete; // prohibe asignar una Funcion a otra

    std::vector<Token> tokenizar(std::string);
};

#endif