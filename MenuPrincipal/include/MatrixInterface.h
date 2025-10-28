#ifndef MATRIX_INTERFACE_H
#define MATRIX_INTERFACE_H

#include <string>

class MatrixInterface {
public:
    static void interfazMultiplicadorMatrices();
    static void mostrarMatricesDisponibles();
    static bool validarArchivoExiste(const std::string& ruta);
};

#endif