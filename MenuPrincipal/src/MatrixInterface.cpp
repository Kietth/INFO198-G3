#include "MatrixInterface.h"
#include "ConfigManager.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <vector>
using namespace std;

bool MatrixInterface::validarArchivoExiste(const string& ruta) {
    ifstream archivo(ruta);
    return archivo.good();
}

void MatrixInterface::interfazMultiplicadorMatrices() {
    ConfigManager config;
    
    int n;
    string archivoA, archivoB;
    
    cout << "\n=== MULTIPLICADOR DE MATRICES ===" << endl;
    
    // Mostrar matrices disponibles
    mostrarMatricesDisponibles();
    
    cout << "\nTamaño de la matriz (N): ";
    cin >> n;
    
    cout << "Ruta del archivo de la matriz A: ";
    cin.ignore();
    getline(cin, archivoA);
    
    cout << "Ruta del archivo de la matriz B: ";
    getline(cin, archivoB);
    
    // Validar existencia de archivos
    if (!validarArchivoExiste(archivoA)) {
        cout << "Error: El archivo " << archivoA << " no existe." << endl;
        return;
    }
    
    if (!validarArchivoExiste(archivoB)) {
        cout << "Error: El archivo " << archivoB << " no existe." << endl;
        return;
    }
    
    cout << "Archivos validados correctamente." << endl;
    cout << "Ejecutando multiplicador de matrices..." << endl;
    
    // Obtener path desde variable de entorno usando ConfigManager
    string multiMPath = config.obtenerValor("MULTI_M");
    if (multiMPath.empty()) {
        multiMPath = "./MultMatrices/multmat";
    }
    
    // Construir comando con parámetros
    string comando = multiMPath + " \"" + archivoA + "\" \"" + archivoB + "\"";
    
    cout << "Ejecutando: " << comando << endl;
    int resultado = system(comando.c_str());
    
    if (resultado != 0) {
        cout << "Error al ejecutar el multiplicador de matrices." << endl;
    }
}

void MatrixInterface::mostrarMatricesDisponibles() {
    ConfigManager config;
    
    cout << "\n=== MATRICES DISPONIBLES ===" << endl;
    
    // Obtener path desde variable de entorno usando ConfigManager
    string directorio = config.obtenerValor("MATRICES_EJEMPLO");
    if (directorio.empty()) {
        directorio = "./MultMatrices/data";
    }
    
    cout << "Matrices 3x3:" << endl;
    cout << "- " << directorio << "/A_3x3.txt" << endl;
    cout << "- " << directorio << "/B_3x3.txt" << endl;
    
    cout << "\nMatrices 4x4:" << endl;
    cout << "- " << directorio << "/A_4x4.txt" << endl;
    cout << "- " << directorio << "/B_4x4.txt" << endl;
    
    cout << "\nNota: Puede usar cualquier archivo de matriz con el formato correcto." << endl;
}