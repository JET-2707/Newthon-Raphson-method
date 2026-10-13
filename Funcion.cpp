#include "Funcion.hpp"

#include <iostream>
#include <algorithm>
#include <stack>
#include <cctype>
#include <stdexcept>
#include <memory>

Nodo::~Nodo(){ delete izq; delete der; }

Token Nodo::getDato() const { return dato; }
Nodo* Nodo::getIzq() const { return izq; }
Nodo* Nodo::getDer() const { return der; }

void Nodo::setIzq(Nodo* iz) { this->izq = iz; }
void Nodo::setDer(Nodo* dr) { this->der = dr; }

Nodo* Nodo::clonar() const {
    auto copia = std::make_unique<Nodo>(dato);

    if (izq != nullptr) copia->setIzq(izq->clonar());
    if (der != nullptr) copia->setDer(der->clonar());

    return copia.release();
}

std::string Tokenizador::leerNumero(const std::vector<Token>& tokens, const std::string& expresion, size_t& i) {
    std::string token;
    int puntos = 0;

    while (i < expresion.size() && (std::isdigit(static_cast<unsigned char>(expresion[i])) || expresion[i] == '.')) {
        if (expresion[i] == '.') puntos++;
        token.push_back(expresion[i]);
        i++;
    } // while

    // El ciclo deja i en el primer carácter que no pertenece al número. El for de tokenizar incrementará i después
    if (i > 0) i--;
    if (token.empty() || puntos > 1 || token.front() == '.' || token.back() == '.' || terminaValor(tokens))
        throw std::invalid_argument("Sintaxis no valida en numero");
    if (!tokens.empty() && tokens.back().tipo == TipoToken::FUNCION)
        throw std::invalid_argument("Sintaxis no valida: faltan parentesis en la funcion");

    return token;
}
bool Tokenizador::terminaValor(const std::vector<Token> &tokens){
    if (tokens.empty()) return false;
    return (tokens.back().tipo == TipoToken::NUMERO || tokens.back().tipo == TipoToken::VARIABLE || tokens.back().valor == ")" );
}
void Tokenizador::multImpl(std::vector<Token> &tokens){
    if (!tokens.empty() && terminaValor(tokens))
        tokens.push_back(Token{TipoToken::OPERADOR, "*"});
}
void Tokenizador::validarSintaxis(const std::vector<Token>& tokens){
    if (!tokens.empty()){
        if (tokens.back().tipo == TipoToken::OPERADOR || (tokens.front().tipo == TipoToken::OPERADOR && tokens.front().valor != "neg"))
            throw std::invalid_argument("Sintaxis no valida: posicion invalida del operador binario");
    } else throw std::invalid_argument("Expresion vacia");
}
void Tokenizador::validarParentesis(const std::vector<Token>& tokens){
    int abiertos = 0;

    for (const Token& t : tokens){
        if (t.tipo == TipoToken::PARENTESIS){
            if (t.valor == "("){
                abiertos++;
            } else if (t.valor == ")"){
                abiertos--;

                if (abiertos < 0)
                    throw std::invalid_argument("Sintaxis no valida: parentesis no validos");
            }
        }
    }

    if (abiertos != 0)
        throw std::invalid_argument("Sintaxis no valida: parentesis no validos");
}
std::vector<Token> Tokenizador::tokenizar(std::string funcion){
    std::vector<Token> tokens;
    
    for (size_t i=0; i<funcion.size(); i++){
        // NÚMERO
        if (isdigit(funcion[i]) || funcion[i] == '.'){
            tokens.push_back(Token{TipoToken::NUMERO, leerNumero(tokens, funcion, i)});
        
        // FUNCIONES (sin, cos)
        } else if (funcion.substr(i, 3) == "sin" || funcion.substr(i, 3) == "cos"){
            multImpl(tokens);
            tokens.push_back(Token{TipoToken::FUNCION, funcion.substr(i, 3)}); i+=2;
        
        // VARIABLE
        } else if (funcion[i] == 'x'){
            multImpl(tokens);
            if(!tokens.empty()) if (tokens.back().tipo == TipoToken::FUNCION) throw std::invalid_argument("Sintaxis no valida: falta de parentesis entre FUNCION y VARIABLE");
            tokens.push_back(Token{TipoToken::VARIABLE, std::string(1,funcion[i])});
            
        
        // OPERADORES
        } else if (funcion[i] == '+' || funcion[i] == '*' || funcion[i] == '/' || funcion[i] == '^'){
            if(!tokens.empty()) if (tokens.back().tipo == TipoToken::OPERADOR) throw std::invalid_argument("Sintaxis no valida: operadores consecutivos");
            tokens.push_back(Token{TipoToken::OPERADOR, std::string(1,funcion[i])});
        // Menos (unario)
        } else if (funcion[i] == '-'){
            // if(!tokens.empty()) if (tokens.back().tipo == TipoToken::OPERADOR) throw std::invalid_argument("Sintaxis no valida: operadores consecutivos");
            if (terminaValor(tokens)){ tokens.push_back(Token{TipoToken::OPERADOR, "-"});
            } else tokens.push_back(Token{TipoToken::OPERADOR, "neg"});

        // PARENTESIS
        } else if (funcion[i] == '('){
            multImpl(tokens);
            tokens.push_back(Token{TipoToken::PARENTESIS, std::string(1,funcion[i])});
        } else if (funcion[i] == ')'){
            if (!tokens.empty()) if (tokens.back().valor == "(") throw std::invalid_argument("Sintaxis no valida: parentesis vacios");
            tokens.push_back(Token{TipoToken::PARENTESIS, std::string(1,funcion[i])});

        // SÍMBOLOS INVALIDOS
        } else if (funcion[i] != ' ') throw std::invalid_argument("Argumento no valido");
    }

    validarParentesis(tokens);
    validarSintaxis(tokens);
    return tokens;
}

Funcion::~Funcion(){ delete raiz; }

int Funcion::prioridad(const Token& t){
    if (t.valor == "^") return 4;
    if (t.valor == "neg") return 3;
    if (t.valor == "*" || t.valor == "/") return 2;
    if (t.valor == "+" || t.valor == "-") return 1;
    return 0;
}
bool Funcion::asociativoDerecha(const Token& t){
    return t.valor == "^" || t.valor == "neg";
}
std::vector<Token> Funcion::transfPosfija(const std::vector<Token>& tokens) {
    if (tokens.empty()) throw std::invalid_argument("Expresion vacia");

    std::vector<Token> postfija;
    std::stack<Token> operadores;

    for (const Token& t : tokens) {
        if (t.tipo == TipoToken::NUMERO || t.tipo == TipoToken::VARIABLE) {
            postfija.push_back(t);

        } else if (t.tipo == TipoToken::FUNCION) {
            operadores.push(t);

        } else if (t.tipo == TipoToken::PARENTESIS) {
            if (t.valor == "(") {
                operadores.push(t);

            } else if (t.valor == ")") {
                while (!operadores.empty() && !(operadores.top().tipo == TipoToken::PARENTESIS && operadores.top().valor == "(")) {
                    postfija.push_back(operadores.top());
                    operadores.pop();
                } // while

                if (operadores.empty())
                    throw std::invalid_argument("Sintaxis no valida: parentesis no validos");

                operadores.pop(); // Retirar "("

                // Función que precede al paréntesis
                if (!operadores.empty() && operadores.top().tipo == TipoToken::FUNCION) {
                    postfija.push_back(operadores.top());
                    operadores.pop();
                } // if
            } else  throw std::invalid_argument("Tipo de parentesis no valido");

        } else if (t.tipo == TipoToken::OPERADOR) {

            // El menos unario no debe sacar operadores de la pila
            if (t.valor == "neg") {
                operadores.push(t);
                continue;
            } // if

            while (!operadores.empty() && operadores.top().tipo == TipoToken::OPERADOR) {
                const Token& cima = operadores.top();
                const bool sacar = asociativoDerecha(t) ? prioridad(t) < prioridad(cima) : prioridad(t) <= prioridad(cima);

                if (!sacar) break;
                postfija.push_back(cima);
                operadores.pop();
            } // while

            operadores.push(t);

        } else throw std::invalid_argument("Tipo de token no valido");
    } // for

    while (!operadores.empty()) {
        if (operadores.top().tipo == TipoToken::PARENTESIS)
            throw std::invalid_argument("Sintaxis no valida: parentesis no validos");
        postfija.push_back(operadores.top());
        operadores.pop();
    } // while

    return postfija;
}


void Funcion::construirArbol(const std::vector<Token>& tokens) {
    const std::vector<Token> postfija = transfPosfija(tokens);

    std::vector<std::unique_ptr<Nodo>> pila;
    pila.reserve(postfija.size());

    for (const Token& t : postfija) {
        if (t.tipo == TipoToken::NUMERO || t.tipo == TipoToken::VARIABLE) {
            pila.push_back(std::make_unique<Nodo>(t));

        } else if (t.tipo == TipoToken::FUNCION || (t.tipo == TipoToken::OPERADOR && t.valor == "neg")) {
            // Funciones y negación: operadores unarios
            if (pila.empty()) throw std::invalid_argument("Falta el operando de un operador unario");

            auto n = std::make_unique<Nodo>(t);

            n->setIzq(pila.back().release());
            pila.pop_back();
            pila.push_back(std::move(n));

        } else if (t.tipo == TipoToken::OPERADOR) {
            // Demás operadores son binarios
            if (pila.size() < 2) 
                throw std::invalid_argument("Faltan operandos para un operador binario");

            auto n = std::make_unique<Nodo>(t);

            n->setDer(pila.back().release()); // Último elemento: operando derecho
            pila.pop_back();

            n->setIzq(pila.back().release()); // Elemento anterior: operando izquierdo
            pila.pop_back();

            pila.push_back(std::move(n));

        } else throw std::invalid_argument("Token inesperado en la expresion posfija");
    } // for

    // Un árbol por expresión
    if (pila.size() != 1)
        throw std::invalid_argument("Expresion no valida: sobran operandos");

    
    Nodo* nuevaRaiz = pila.back().release(); // Sustituimos la raíz anterior cuando todo salió bien
    delete raiz;
    raiz = nuevaRaiz;
}

int main(){
    Funcion f;
    Tokenizador t;
    std::vector<Token> funcion = t.tokenizar("xcos(x)+(12.6-x^2)");

    f.construirArbol(funcion);
    // Función para ver árbol (?)

    return 0;
}