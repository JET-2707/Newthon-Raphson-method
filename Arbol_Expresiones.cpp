#include "Arbol_Expresiones.hpp"

#include <vector>
#include <stack>
#include <stdexcept>
#include <memory>
#include <cmath>

int ArbolExpresiones::prioridad(const Token& t){
    if (t.valor == "^") return 4;
    if (t.valor == "neg") return 3;
    if (t.valor == "*" || t.valor == "/") return 2;
    if (t.valor == "+" || t.valor == "-") return 1;
    return 0;
}
bool ArbolExpresiones::asociativoDerecha(const Token& t){
    return t.valor == "^" || t.valor == "neg";
}
std::vector<Token> ArbolExpresiones::transfPostfija(const std::vector<Token>& tokens) {
    if (tokens.empty()) throw std::invalid_argument("Expresion vacia");
    
    std::stack<Token> operadores;
    std::vector<Token> postfija;

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
void ArbolExpresiones::construirArbol(const std::vector<Token>& tokens) {
    std::vector<Token> postfija = transfPostfija(tokens);

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

            if (t.valor == "^" && (pila.back().get()->getDato().tipo != TipoToken::NUMERO || 
            (pila.back().get()->getDato().valor != "/" && 
            pila.back().get()->getDer()->getDato().tipo != TipoToken::NUMERO && 
            pila.back().get()->getIzq()->getDato().tipo != TipoToken::NUMERO)))
                throw std::invalid_argument("El exponente debe ser un numero positivo o una division (a/b)");

            auto n = std::make_unique<Nodo>(t);

            n->setDer(pila.back().release()); // Último elemento: operando derecho
            pila.pop_back();

            n->setIzq(pila.back().release()); // Elemento anterior: operando izquierdo
            pila.pop_back();

            pila.push_back(std::move(n));

        } else throw std::invalid_argument("Token inesperado en la expresion posfija");
    } // for

    // Un árbol por expresión
    if (pila.size() != 1) throw std::invalid_argument("Expresion no valida: sobran operandos");
    
    Nodo* nuevaRaiz = pila.back().release(); // Sustituimos la raíz anterior cuando todo salió bien
    delete raiz;
    raiz = nuevaRaiz;
}

double ArbolExpresiones::evaluarNodo(Nodo* n, double x) const {
    if (n->getDato().tipo == TipoToken::NUMERO){ 
        return std::stod(n->getDato().valor);

    } else if (n->getDato().tipo == TipoToken::VARIABLE){ 
        return x;

    } else if (n->getDato().tipo == TipoToken::OPERADOR){
        switch (n->getDato().valor[0]){
            case '+':   return evaluarNodo(n->getIzq(), x) + evaluarNodo(n->getDer(), x);
            case '-':   return evaluarNodo(n->getIzq(), x) - evaluarNodo(n->getDer(), x);
            case '*':   return evaluarNodo(n->getIzq(), x) * evaluarNodo(n->getDer(), x);
            case '/':   return evaluarNodo(n->getIzq(), x) / evaluarNodo(n->getDer(), x);
            case '^':   return pow(evaluarNodo(n->getIzq(), x), evaluarNodo(n->getDer(), x));
            case 'n': return -1 * evaluarNodo(n->getIzq(), x);
            default: throw std::logic_error("Tipo de token invalido");
        } // switch
        
    } else if (n->getDato().tipo == TipoToken::FUNCION){
        if (n->getDato().valor == "sin")
            return std::sin(evaluarNodo(n->getIzq(), x));
        if (n->getDato().valor == "cos")
            return std::cos(evaluarNodo(n->getIzq(), x));
        throw std::logic_error("Función no definida");
    } // if
    
    throw std::domain_error("Función no definida en x[i]");
}

double ArbolExpresiones::evaluar(double x) const {
    if (raiz == nullptr) throw std::invalid_argument("Expresion vacia");
    if (std::isfinite(evaluarNodo(raiz, x))) 
        return evaluarNodo(raiz, x);
    throw std::domain_error("Desbordamiento");
}