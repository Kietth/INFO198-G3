#include "../include/funcmatrices.h"
#include "../include/op_archivos.h"
#include <iostream>
#include <cstdlib>
#ifdef _WIN32
#include <windows.h>
inline int obtenerPID() {return GetCurrentProcessId();}
#else
#include <unistd.h>
inline int obtenerPID() {return getpid();}
#endif

using namespace std;

int main(int argc, char* argv[]) {

    cout << "PID: " << obtenerPID() << endl;
    // Verificar que se ejecute desde el menú principal
    if (argc != 4) {
        cerr << "Error: Este programa solo puede ejecutarse desde el menú principal." << endl;
        cerr << "No está diseñado para ejecución manual." << endl;
        return 1;
    }

    // Obtener parámetros pasados por el menú principal
    string archivoA = argv[1];
    string archivoB = argv[2];
    int N = atoi(argv[3]);

    // Leer matrices desde archivos
    vector<vector<int>> matrizA = leeMatriz(archivoA);
    vector<vector<int>> matrizB = leeMatriz(archivoB);

    if (matrizA.empty() || matrizB.empty()) {
        cerr << "Error: No se pudieron leer las matrices desde los archivos." << endl;
        return 1;
    }

    // Verificar que las matrices sean de tamaño NxN
    if (matrizA.size() != N || matrizA[0].size() != N) {
        cerr << "Error: La matriz A no es de tamaño " << N << "x" << N << endl;
        return 1;
    }

    if (matrizB.size() != N || matrizB[0].size() != N) {
        cerr << "Error: La matriz B no es de tamaño " << N << "x" << N << endl;
        return 1;
    }

    // Verificar que las matrices sean multiplicables
    if (matrizA[0].size() != matrizB.size()) {
        cerr << "Error: Las matrices no se pueden multiplicar." << endl;
        cerr << "Columnas de A (" << matrizA[0].size() << ") ≠ Filas de B (" << matrizB.size() << ")" << endl;
        return 1;
    }

    // Mostrar matrices
    cout << "Matriz A: " << endl;
    imprimeMatriz(matrizA);

    cout << "Matriz B: " << endl;
    imprimeMatriz(matrizB);

    // Multiplicar matrices
    vector<vector<int>> MatrizC = multiplicaMatrices(matrizA, matrizB);

    cout << "Resultado de la multiplicación (A × B):" << endl;
    imprimeMatriz(MatrizC);

    return 0;
}