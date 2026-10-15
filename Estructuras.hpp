#ifndef ESTRUCTURAS_HPP
#define ESTRUCTURAS_HPP

#include <string>

enum class TipoToken {
    NUMERO, VARIABLE, OPERADOR, PARENTESIS, FUNCION
};

struct Token{
    TipoToken tipo;
    std::string valor;
};

#endif