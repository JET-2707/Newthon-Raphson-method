#include "Funcion.hpp"
#include <iostream>
#include <cctype>
#include <stdexcept>

Nodo::~Nodo(){ delete izq; delete der; }

std::string Nodo::getDato() const { return dato; }
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
std::vector<std::string> Funcion::tokenizar(std::string funcion){
    std::vector<std::string> tokens;
    std::string token;

    for (char& c : funcion){
        if (isdigit(c) || c == '.') { token.push_back(c);
        } else if (c == 'x' || c == '+' || c == '-' || c == '*' || c == '/' || c == '^' || c == '(' || c == ')'){
            if (!token.empty()) { tokens.push_back(token); token.clear(); }
            tokens.push_back(std::string(1,c));
        } else if (c == ' '){
            if (!token.empty()) { tokens.push_back(token); token.clear(); }
        } else throw std::invalid_argument("Argumento no valido");
    } // for
    
    if (!token.empty()) { tokens.push_back(token); token.clear(); } // Revisa si no queda un token sin guardar
    return tokens;
}

int main(){
    Funcion f1;
    std::vector<std::string> t1 = f1.tokenizar("2*x+1");
    std::vector<std::string> t2 = f1.tokenizar("12.5 * (x - 3)");

    for (auto& s : t1) std::cout << "\"" << s << "\"  "; std::cout << std::endl;
    for (auto& s : t2) std::cout << "\"" << s << "\"  "; std::cout << std::endl;

    try {
        std::vector<std::string> t3 = f1.tokenizar("3 $ x");
        for (auto& s : t3) std::cout << "\"" << s << "\"  "; std::cout << std::endl;
    }
    catch (const std::exception& e){
        std::cout << "Error: " << e.what() << std::endl;
    }

    std::vector<std::string> t4 = f1.tokenizar("2 3");
    std::vector<std::string> t5 = f1.tokenizar("x^2");

    for (auto& s : t4) std::cout << "\"" << s << "\"  "; std::cout << std::endl;
    for (auto& s : t5) std::cout << "\"" << s << "\"  "; std::cout << std::endl;

    return 0;
}