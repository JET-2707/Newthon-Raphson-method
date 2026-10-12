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
std::vector<Token> Funcion::tokenizar(std::string funcion){
    std::vector<Token> tokens;
    std::string token;
    
    for (int i=0; i<funcion.size(); i++){
        // NÚMERO
        if (isdigit(funcion[i]) || funcion[i] == '.'){
            while (isdigit(funcion[i]) || funcion[i] == '.') { token.push_back(funcion[i]); i++; }
            i--; tokens.push_back(Token{TipoToken::NUMERO, token}); token.clear();
        
        // FUNCIONES (sin, cos, ln)
        } else if (funcion.substr(i, 3) == "sin" || funcion.substr(i, 3) == "cos"){
            tokens.push_back(Token{TipoToken::FUNCION, funcion.substr(i, 3)}); i+=2;
        } else if (funcion.substr(i, 2) == "ln"){
            tokens.push_back(Token{TipoToken::FUNCION, "ln"}); i++;
        
        // VARIABLE
        } else if (funcion[i] == 'x'){
            tokens.push_back(Token{TipoToken::VARIABLE, std::string(1,funcion[i])});
        
        // OPERADORES
        } else if (funcion[i] == '+' || funcion[i] == '*' || funcion[i] == '/' || funcion[i] == '^'){
            tokens.push_back(Token{TipoToken::OPERADOR, std::string(1,funcion[i])});
        // Menos (unario)
        } else if (funcion[i] == '-'){
            if (!tokens.empty()){
                if (tokens.back().tipo == TipoToken::NUMERO || tokens.back().tipo == TipoToken::VARIABLE || tokens.back().valor == ")" ){
                    tokens.push_back(Token{TipoToken::OPERADOR, "-"});
                } else tokens.push_back(Token{TipoToken::OPERADOR, "neg"});
            } else tokens.push_back(Token{TipoToken::OPERADOR, "neg"});

        // PARENTESIS
        } else if (funcion[i] == '(' || funcion[i] == ')'){
            tokens.push_back(Token{TipoToken::PARENTESIS, std::string(1,funcion[i])});
        
        // SÍMBOLOS INVALIDOS
        } else if (funcion[i] != ' ') throw std::invalid_argument("Argumento no valido");
    }

    return tokens;
}

int main(){
    Funcion f1;

    // FUNCIONES BÁSICAS 

    std::vector<Token> t1 = f1.tokenizar("2*x+1");
    std::vector<Token> t2 = f1.tokenizar("12.5 * (x - 3)");
    
    for (auto& s : t1) std::cout << "\"" << s.valor << "\"  "; std::cout << std::endl;
    for (auto& s : t2) std::cout << "\"" << s.valor << "\"  "; std::cout << std::endl;
    
    try {
        std::vector<Token> t3 = f1.tokenizar("3 $ x");
        for (auto& s : t3) std::cout << "\"" << s.valor << "\"  "; std::cout << std::endl;
    }
    catch (const std::exception& e){
        std::cout << "Error: " << e.what() << std::endl;
    }
    
    std::vector<Token> t4 = f1.tokenizar("2 3");
    std::vector<Token> t5 = f1.tokenizar("x^2");
    
    for (auto& s : t4) std::cout << "\"" << s.valor << "\"  "; std::cout << std::endl;
    for (auto& s : t5) std::cout << "\"" << s.valor << "\"  "; std::cout << std::endl;

    // MENOS UNARIO

    std::vector<Token> t6 = f1.tokenizar("-x");
    std::vector<Token> t7 = f1.tokenizar("3-x");
    std::vector<Token> t8 = f1.tokenizar("3 - x");
    std::vector<Token> t9 = f1.tokenizar("-x^2");
    std::vector<Token> t10 = f1.tokenizar("2*-x");
    std::vector<Token> t11 = f1.tokenizar("(-x)");
    std::vector<Token> t12 = f1.tokenizar("x--x");
    std::vector<Token> t13 = f1.tokenizar("-sin(x)");

    for (auto& s : t6) std::cout << "\"" << s.valor << "\"  "; std::cout << std::endl;
    for (auto& s : t7) std::cout << "\"" << s.valor << "\"  "; std::cout << std::endl;
    for (auto& s : t8) std::cout << "\"" << s.valor << "\"  "; std::cout << std::endl;
    for (auto& s : t9) std::cout << "\"" << s.valor << "\"  "; std::cout << std::endl;
    for (auto& s : t10) std::cout << "\"" << s.valor << "\"  "; std::cout << std::endl;
    for (auto& s : t11) std::cout << "\"" << s.valor << "\"  "; std::cout << std::endl;
    for (auto& s : t12) std::cout << "\"" << s.valor << "\"  "; std::cout << std::endl;
    for (auto& s : t13) std::cout << "\"" << s.valor << "\"  "; std::cout << std::endl;

    return 0;
}