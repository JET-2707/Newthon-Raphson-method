#include "Funcion.hpp"
#include <iostream>
#include <cctype>
#include <stdexcept>

Nodo::~Nodo(){ delete izq; delete der; }

Token Nodo::getDato() const { return dato; }
Nodo* Nodo::getIzq() const { return izq; }
Nodo* Nodo::getDer() const { return der; }

void Nodo::setIzq(Nodo* iz) { this->izq = iz; }
void Nodo::setDer(Nodo* dr) { this->der = dr; }

Nodo* Nodo::clonar() const {
    Nodo* temp = new Nodo(dato);
    if (izq != nullptr) temp->izq = izq->clonar();
    if (der != nullptr) temp->der = der->clonar();

    return temp;
}

Funcion::~Funcion(){ delete raiz; }

std::string Tokenizador::leerNumero(const std::vector<Token>& tokens, const std::string& expresion, size_t& i){
    std::string token;
    int punto=0;
    while (isdigit(expresion[i]) || expresion[i] == '.') { 
        token.push_back(expresion[i]); i++; 
        if (expresion[i] == '.') punto++;
    } i--;
    
    if ((punto != 0 && punto!=1) || token.front() == '.' || token.front() == '.' || terminaValor(tokens)){
        throw std::invalid_argument("Sintaxis no valida");
    } // fi
    
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
    if (tokens.back().tipo == TipoToken::OPERADOR || (tokens.front().tipo == TipoToken::OPERADOR && tokens.front().valor != "neg"))
        throw std::invalid_argument("Sintaxis no valida: posicion invalida del operador binario");
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
    std::string token;
    
    for (size_t i=0; i<funcion.size(); i++){
        // NÚMERO
        if (isdigit(funcion[i]) || funcion[i] == '.'){
            tokens.push_back(Token{TipoToken::NUMERO, leerNumero(tokens, funcion, i)}); token.clear();
        
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
            if(!tokens.empty()) if (tokens.back().tipo == TipoToken::OPERADOR) throw std::invalid_argument("Sintaxis no valida: operadores consecutivos");
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

// El enum class no se imprime solo: esta función da un nombre a cada tipo
std::string nombreTipo(TipoToken t){
    switch (t){
        case TipoToken::NUMERO:     return "NUM";
        case TipoToken::VARIABLE:   return "VAR";
        case TipoToken::OPERADOR:   return "OP";
        case TipoToken::PARENTESIS: return "PAR";
        case TipoToken::FUNCION:    return "FUN";
    }
    return "?";
}

// Une los valores con espacios: "2 * x + 1"
std::string aTexto(const std::vector<Token>& tokens){
    std::string s;
    for (size_t i = 0; i < tokens.size(); i++){
        if (i > 0) s += " ";
        s += tokens[i].valor;
    }
    return s;
}

// Imprime cada token como valor:TIPO
void imprimir(const std::vector<Token>& tokens){
    for (const Token& t : tokens)
        std::cout << t.valor << ":" << nombreTipo(t.tipo) << "  ";
    std::cout << std::endl;
}

struct Caso {
    std::string entrada;
    std::string esperado;   // "ERROR" si debe lanzar excepción
};

int main(){
    Tokenizador t;

    std::vector<Caso> casos = {
        // Básicos
        {"2*x+1",           "2 * x + 1"},
        {"12.5 * (x - 3)",  "12.5 * ( x - 3 )"},
        {"x^2",             "x ^ 2"},
        {"2^x",             "2 ^ x"},
        {"x+1",             "x + 1"},

        // Funciones
        {"sin(x)",          "sin ( x )"},
        {"cos(x+1)",         "cos ( x + 1 )"},

        // Menos unario
        {"-x",              "neg x"},
        {"3-x",             "3 - x"},
        {"3 - x",           "3 - x"},
        {"-x^2",            "neg x ^ 2"},
        {"(-x)",            "( neg x )"},
        {"-sin(x)",         "neg sin ( x )"},
        
        // Multiplicación implícita
        {"2x",              "2 * x"},
        {"2x^2",            "2 * x ^ 2"},
        {"3sin(x)",         "3 * sin ( x )"},
        {"xcos(x)",         "x * cos ( x )"},
        {"2(x+1)",          "2 * ( x + 1 )"},
        {"x(x-1)",          "x * ( x - 1 )"},
        {"(x+1)(x-1)",      "( x + 1 ) * ( x - 1 )"},
        {"-2x",             "neg 2 * x"},
        
        // Deben fallar
        {"2*-x",            "ERROR"},
        {"x--x",            "ERROR"},
        {"3 $ x",           "ERROR"},
        {"tan(x)",          "ERROR"},
        {"2 3",             "ERROR"},
        {"x2",              "ERROR"},
        {"x()2",            "ERROR"},
        {"(x)2",            "ERROR"}
    };

    int fallas = 0;
    for (const Caso& c : casos){
        std::cout << "Entrada: \"" << c.entrada << "\"" << std::endl;
        std::string obtenido;

        try {
            std::vector<Token> tokens = t.tokenizar(c.entrada);
            obtenido = aTexto(tokens);
            std::cout << "  ";
            imprimir(tokens);
        }
        catch (const std::exception& e){
            obtenido = "ERROR";
            std::cout << "  Error: " << e.what() << std::endl;
        }

        if (obtenido == c.esperado) {
            std::cout << "  OK" << std::endl;
        } else {
            std::cout << "  FALLA (esperado: " << c.esperado
                      << " | obtenido: " << obtenido << ")" << std::endl;
            fallas++;
        }
    }

    std::cout << "\nFallas: " << fallas << " de " << casos.size() << std::endl;
    return 0;
}