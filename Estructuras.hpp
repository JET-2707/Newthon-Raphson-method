#ifndef ESTRUCTURAS_H
#define ESTRUCTURAS_H

#include <string>

enum class TipoToken {
    NUMERO, VARIABLE, OPERADOR, PARENTESIS, FUNCION
};

struct Token{
    TipoToken tipo;
    std::string valor;
};

#endif