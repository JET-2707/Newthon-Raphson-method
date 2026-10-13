#ifndef FUNCION_H
#define FUNCION_H

#include <string>
#include <vector>
#include <stack>
#include <stdexcept>

enum class TipoToken {
    NUMERO, VARIABLE, OPERADOR, PARENTESIS, FUNCION
};

struct Token{
    TipoToken tipo;
    std::string valor;
};

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

class Tokenizador{

private:
    // std::string expresion;
    // std::vector<Token> tokens;
    bool terminaValor(const std::vector<Token>& tokens);
    void multImpl(std::vector<Token>& tokens);
    std::string leerNumero(const std::vector<Token>& tokens, const std::string& expresion, size_t& i);

    void validarSintaxis(const std::vector<Token>& tokens);
    void validarParentesis(const std::vector<Token>& tokens);

public:
    // std::vector<Token> getTokens();
    std::vector<Token> tokenizar(std::string);
};

class Funcion{
private:
    Nodo* raiz;
    
    int prioridad(const Token &t);
    bool asociativoDerecha(const Token &t);
    
    std::vector<Token> transfPosfija(const std::vector<Token> &tokens);
public:
    Funcion() : raiz(nullptr) {} 
    ~Funcion();

    Funcion(const Funcion&) = delete; // prohibe crear una Funcion copiando otra
    Funcion& operator=(const Funcion&) = delete; // prohibe asignar una Funcion a otra

    void construirArbol(const std::vector<Token> &tokens);
};

#endif