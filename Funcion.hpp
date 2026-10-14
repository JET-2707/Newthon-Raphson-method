#ifndef FUNCION_H
#define FUNCION_H

#include "Estructuras.hpp"
#include "Arbol_Expresiones.hpp"

#include <vector>
#include <stack>
#include <stdexcept>

class Tokenizador{
private:
    bool terminaValor(const std::vector<Token>& tokens);
    void multImpl(std::vector<Token>& tokens);
    std::string leerNumero(const std::vector<Token>& tokens, const std::string& expresion, size_t& i);

    void validarSintaxis(const std::vector<Token>& tokens);
    void validarParentesis(const std::vector<Token>& tokens);

public:
    std::vector<Token> tokenizar(std::string);
};

class Funcion{
private:
    ArbolExpresiones expresion, derivada;
public:
    // Funcion(ArbolExpresiones ex, ArbolExpresiones dv) : expresion(ex), derivada(dv) {}
    ~Funcion();

    Funcion(const Funcion&) = delete; // prohibe crear una Funcion copiando otra
    Funcion& operator=(const Funcion&) = delete; // prohibe asignar una Funcion a otra
};

#endif