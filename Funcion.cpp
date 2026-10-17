#include "Funcion.hpp"
#include "Arbol_Expresiones.hpp"
#include "Derivada.hpp"

#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <memory>

std::string Tokenizador::leerNumero(const std::vector<Token>& tokens, const std::string& expresion, size_t& i) {
    std::string token;
    
    while (i < expresion.size() && (std::isdigit(static_cast<unsigned char>(expresion[i])) || expresion[i] == '.')) {
        token.push_back(expresion[i]);
        i++;
    } // while
    
    int puntos = std::count(token.begin(), token.end(), '.');

    // El ciclo deja i en el primer carácter que no pertenece al número. El for de tokenizar incrementará i después
    if (i > 0) i--;
    if (token.empty() || puntos > 1 || token.front() == '.' || token.back() == '.' || terminaValor(tokens))
        throw std::invalid_argument("Sintaxis no valida en numero");
    
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
        for (size_t i=0; i<tokens.size(); i++){
            if (tokens[i].valor == "/"){
                if (i<2 || tokens[i-2].valor != "(" || tokens[i-1].tipo != TipoToken::NUMERO || tokens[i+1].tipo != TipoToken::NUMERO || tokens[i+2].valor != ")")
                    throw std::invalid_argument("Sintaxis no valida: la division solo se permite como fraccion numerica (a/b)");
                if (std::stod(tokens[i+1].valor) == 0) throw std::invalid_argument("Division entre cero");
            } // if
        }

        if (tokens.back().tipo == TipoToken::OPERADOR || (tokens.front().tipo == TipoToken::OPERADOR && tokens.front().valor != "neg"))
            throw std::invalid_argument("Sintaxis no valida: posicion invalida del operador binario");
        if (tokens.back().tipo == TipoToken::FUNCION)
            throw std::invalid_argument("Sintaxis no valida: FUNCION sin argumento a evaluar");
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
        if (!tokens.empty()) if (tokens.back().tipo == TipoToken::FUNCION && (funcion[i] != '(' && funcion[i] != ' '))
            throw std::invalid_argument("Sintaxis no valida: FUNCION sin parentesis");

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
            tokens.push_back(Token{TipoToken::VARIABLE, std::string(1,funcion[i])});
        
        // OPERADORES
        } else if (funcion[i] == '+' || funcion[i] == '*' || funcion[i] == '/' || funcion[i] == '^'){
            if(!tokens.empty()) if (tokens.back().tipo == TipoToken::OPERADOR) throw std::invalid_argument("Sintaxis no valida: operadores consecutivos");
            tokens.push_back(Token{TipoToken::OPERADOR, std::string(1,funcion[i])});
        // Menos (unario)
        } else if (funcion[i] == '-'){
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

Funcion::Funcion(const std::string& f){
    Tokenizador t;
    Derivador d;

    this->funcion = f;
    expresion.construirArbol(t.tokenizar(funcion));
    derivada.adoptar(d.derivar(expresion));
}
std::string Funcion::getFuncion() { return funcion; }
double Funcion::evaluar(double x) const { return expresion.evaluar(x); }
double Funcion::evaluarDerivada(double x) const { return derivada.evaluar(x); }