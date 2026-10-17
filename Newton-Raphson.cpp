#include "Newton-Raphson.hpp"
#include "Derivada.hpp"
#include "Funcion.hpp"
#include "Arbol_Expresiones.hpp"

#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <string>
#include <cmath>


size_t validarIteraciones(int iteraciones){
    if (iteraciones < 1 ) return 1;
    if (iteraciones > 100) return 100;
    return iteraciones;
}

double Newton_Raphson::redondeo(double val) const{
    double p = pow(10, 4);
    return round(val*p)/p + 0.0;
}
void Newton_Raphson::mostrarIteracion(double f_x, double fder_x, size_t i) const {
    const int w = std::max(12, 11);   // ancho de cada columna numerica

    if (i == 0){
        std::cout << "| " << std::setw(3) << "i" << " | " << std::setw(w) << "x_(i-1)" << " | " << std::setw(w) << "f(x_(i-1))"
                  << " | " << std::setw(w) << "f'(x_(i-1))" << " | " << std::setw(w) << "x_i" << " | " << std::setw(w) << "Error" << " |" << std::endl;

        std::cout << "------";
        for (int c = 0; c < 5; c++) std::cout << std::string(w + 3, '-') ;
        std::cout << "-\n";
    }
    std::cout << "| " << std::setw(3) << i + 1 << " | " << std::setw(w) << redondeo(x1) << " | " << std::setw(w) << redondeo(f_x)
              << " | " << std::setw(w) << redondeo(fder_x) << " | " << std::setw(w) << redondeo(x2) << " | " << std::setw(w) << redondeo(error) << " |" << std::endl;
}
void Newton_Raphson::metodo(size_t iteraciones){
    bool convergio = false;
    size_t usadas = 0;

    std::cout << std::fixed << std::setprecision(4);

    for (size_t i=0; i<iteraciones; i++){
        double f_x, fder_x;
        try{
            f_x = funciones.evaluar(x1);
            fder_x = funciones.evaluarDerivada(x1);
        } catch (const std::domain_error& e) {
            throw std::domain_error(std::string(e.what()) + " (x = " + std::to_string(x1) + ")");
        } // try/catch  
        
        if (std::fabs(fder_x) < 1e-12) throw std::domain_error("Derivada de f(x) igual a cero (x = " + std::to_string(x1) + ")");

        x2 = x1-(f_x/fder_x);
        error = std::fabs(x2-x1);
        mostrarIteracion(f_x, fder_x, i);
        usadas = i+1;

        if (error<tol) { convergio = true; break; }
        x1 = x2;
    } // for

    std::cout << std::endl;
    if (convergio) std::cout << "Converge en " << usadas << " iteraciones." << std::endl;
    else           std::cout << "No se alcanzo la convergencia en " << iteraciones << " iteraciones." << std::endl;
    std::cout << "Ultima aproximacion a la raiz: x = " << redondeo(x2) << std::endl;

    try {   // validar la raiz: f(x) debe ser cercano a cero
        double f_raiz = funciones.evaluar(x2);
        std::cout << std::scientific << std::setprecision(3) << "f(x) en la aproximacion = " << f_raiz
                  << std::fixed << std::setprecision(4) << std::endl;
        if (std::fabs(f_raiz) > std::pow(10.0, -4))
            std::cout << "Advertencia: |f(x)| no es cercano a cero; la aproximacion puede no ser una raiz." << std::endl;
    } catch (const std::domain_error&) {
        std::cout << "Advertencia: f(x) no esta definida en la aproximacion final." << std::endl;
    }
}

int main(){
    try{
        std::string f;
        double x_0;
        int iteraciones;
        
        std::cout << "\t // METODO NEWTON-RAPHSON \\\\ \t" << std::endl << std::endl;
        std::cout << "Funcion [f(x)]: "; getline(std::cin, f);
        std::cout << "Valor inicial [x_0]: ";
        if (!(std::cin >> x_0)) throw std::invalid_argument("Valor inicial no valido");
        std::cout << "Iteraciones [1-100]: ";
        if (!(std::cin >> iteraciones)) throw std::invalid_argument("Numero de iteraciones no valido");
        
        Newton_Raphson nr(f, x_0);
        std::cout << std::endl << "f(x) = " << f << std::endl << std::endl;
        nr.metodo(validarIteraciones(iteraciones));

    } catch (const std::invalid_argument& ia) {
        std::cout << ia.what() << std::endl; // Manejo de la excepción
    } catch (const std::logic_error& le) {
        std::cout << le.what() << std::endl; // Manejo de la excepción
    }

    return 0;
}