#ifndef NEWTON_RAPHSON_HPP
#define NEWTON_RAPHSON_HPP

#include "Arbol_Expresiones.hpp"
#include "Derivada.hpp"
#include "Funcion.hpp"

#include <string>
#include <cstddef>
#include <cmath>

class Newton_Raphson{
private:
    Funcion funciones;
    double x1, x2, error;
    double tol;
    
    double redondeo(double val) const;
    void mostrarIteracion(double f_x, double fder_x, size_t i) const;
public:
    Newton_Raphson(const std::string& fun, double x) : funciones(fun), x1(x), x2(0), error(0) {}
    void metodo(size_t iteraciones);
};

#endif