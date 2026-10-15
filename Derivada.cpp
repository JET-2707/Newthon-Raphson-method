#include "Derivada.hpp"

#include <sstream>
#include <iomanip>
#include <stdexcept>

Ptr Derivador::numero(double v){
    std::ostringstream num;
    num << std::setprecision(17) << v;
    
    return std::make_unique<Nodo>(Token{TipoToken::NUMERO, num.str()});
}
bool Derivador::esNum(const Nodo* n, double v){
    if (n->getDato().tipo == TipoToken::NUMERO && std::stod(n->getDato().valor) == v) return true;
    return false; 
}
bool Derivador::contieneX(const Nodo* n){
    if (n == nullptr) return false;
    if (n->getDato().tipo == TipoToken::VARIABLE) return true;
    return (contieneX(n->getIzq()) || contieneX(n->getDer()));
}
double Derivador::valorConst(const Nodo* n){
    if (n->getDato().tipo == TipoToken::NUMERO) return std::stod(n->getDato().valor);
    if (n->getDato().valor == "/") return std::stod(n->getIzq()->getDato().valor) / std::stod(n->getDer()->getDato().valor);
    throw std::logic_error("No deberias poder ver esto: valor de la potencia mal evaluado");
}
Ptr Derivador::clonar(const Nodo* n){
    return Ptr(n->clonar());
}

Ptr Derivador::nodoBinario(const std::string& operador, Ptr izq, Ptr der){
    auto nBin = std::make_unique<Nodo>(Token{TipoToken::OPERADOR, operador});
    
    nBin.get()->setIzq(izq.release());
    nBin.get()->setDer(der.release());
    
    return nBin;
}
Ptr Derivador::nodoUnario(TipoToken tipo, const std::string& valor, Ptr hijo){
    auto nUno = std::make_unique<Nodo>(Token{tipo, valor});
    nUno.get()->setIzq(hijo.release());

    return nUno;
}

Ptr Derivador::suma(Ptr a, Ptr b){
    if (esNum(a.get(), 0)) return b;
    if (esNum(b.get(), 0)) return a;
    if (a.get()->getDato().tipo == TipoToken::NUMERO && b.get()->getDato().tipo == TipoToken::NUMERO) 
        return numero(std::stod(a.get()->getDato().valor)+std::stod(b.get()->getDato().valor));
    return nodoBinario("+", std::move(a), std::move(b));
}
Ptr Derivador::neg(Ptr a){
    if (a.get()->getDato().tipo == TipoToken::NUMERO) return numero(0-(std::stod(a.get()->getDato().valor)));
    return nodoUnario(TipoToken::OPERADOR, "neg", std::move(a));
}
Ptr Derivador::resta(Ptr a, Ptr b){
    if (esNum(b.get(), 0)) return a;
    if (esNum(a.get(), 0)) return neg(std::move(b));
    if (a.get()->getDato().tipo == TipoToken::NUMERO && b.get()->getDato().tipo == TipoToken::NUMERO) 
        return numero(std::stod(a.get()->getDato().valor)-std::stod(b.get()->getDato().valor));
    return nodoBinario("-", std::move(a), std::move(b));
}
Ptr Derivador::mult(Ptr a, Ptr b){
    if (esNum(a.get(), 0) || esNum(b.get(), 0)) return numero(0);
    if (esNum(a.get(), 1)) return b;
    if (esNum(b.get(), 1)) return a;
    if (a.get()->getDato().tipo == TipoToken::NUMERO && b.get()->getDato().tipo == TipoToken::NUMERO) 
        return numero(std::stod(a.get()->getDato().valor)*std::stod(b.get()->getDato().valor));
    return nodoBinario("*", std::move(a), std::move(b));
}
Ptr Derivador::potencia(Ptr base, double n){
    if(n == 0) return numero(1);
    if(n == 1) return base;
    return nodoBinario("^", std::move(base), numero(n));
}
Ptr Derivador::funcion(std::string nombre, Ptr arg){
    return nodoUnario(TipoToken::FUNCION, nombre, std::move(arg));
}

Ptr Derivador::derivarNodo(const Nodo* n){
    if (n == nullptr) throw std::invalid_argument("Nodo nulo");

    Token t = n->getDato();

    if (t.tipo == TipoToken::NUMERO) return numero(0);
    if (t.tipo == TipoToken::VARIABLE) return numero(1);
    
    if (t.tipo == TipoToken::OPERADOR){
        switch(t.valor[0]){
            case 'n': return neg(derivarNodo(n->getIzq()));
            case '+': return suma(derivarNodo(n->getIzq()), derivarNodo(n->getDer()));
            case '-': return resta(derivarNodo(n->getIzq()), derivarNodo(n->getDer()));
            case '*': { //Regla del producto: u'v + uv'
                const Nodo* izq = n->getIzq();
                const Nodo* der = n->getDer();

                Ptr primerTermino = mult(derivarNodo(izq), clonar(der));
                Ptr segundoTermino = mult(clonar(izq), derivarNodo(der));

                return suma(std::move(primerTermino), std::move(segundoTermino));
            } // (*)
            case '^': {// Regla de la potencia: k*u^(k-1)*u'
                const Nodo* base = n->getIzq();
                const Nodo* exponente = n->getDer();
                if (contieneX(exponente)) throw std::invalid_argument("El exponente debe de ser constante");
                
                double k = valorConst(exponente);
                if (k==0) return numero(0);
                
                Ptr derivadaBase = derivarNodo(base);
                Ptr nuevaPotencia = potencia(clonar(base), k-1);

                return mult(mult(numero(k), std::move(nuevaPotencia)), std::move(derivadaBase));
            } // (^)
            case '/': // Comprobar división (constantes)
                if (contieneX(n)) throw std::invalid_argument("No se admite derivar una division con variables");
                return numero(0);

            default: throw std::logic_error("Operador inexistente");
        } // switch
    } // if

    if (t.tipo == TipoToken::FUNCION){
        const Nodo* argumento = n->getIzq();
        Ptr derivadaArgumento = derivarNodo(argumento);

        if (t.valor == "sin"){ // Regla de sin: (sin (u))' = cos(u)*u'
            return mult(funcion("cos", clonar(argumento)), std::move(derivadaArgumento));
        } // if
        
        if (t.valor == "cos"){ // Regla de cos: (cos(u))' = -sin(u)*u'
            return neg(mult(funcion("sin", clonar(argumento)), std::move(derivadaArgumento)));
        } // if
        throw std::logic_error("Funcion no soportada");
    }
    
    throw std::logic_error("Tipo de nodo no soportado"); 
}
Nodo* Derivador::derivar(const ArbolExpresiones& original){
    const Nodo* raiz = original.getRaiz();
    
    if (raiz == nullptr) throw std::invalid_argument("No hay una expresion que derivar");
    Ptr resultado = derivarNodo(raiz);

    return resultado.release();
}