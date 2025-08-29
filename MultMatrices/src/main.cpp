#include "../include/funcmatrices.h"
#include "../include/op_archivos.h"
#include <iostream>

using namespace std;

int main(){
    cout << "Leyendo archivos... " << endl;
    
    vector<vector<int>> matrizA = leeMatriz("data/A.txt");
    vector<vector<int>> matrizB = leeMatriz("data/B.txt");

    if (matrizA.empty() || matrizB.empty()){
        cerr << "Error: No se pudieron leer las matrices" << endl;
        return 1;
    }

    cout << "Matriz A: " << endl;
    imprimeMatriz(matrizA);

    cout << "Matriz B: " << endl;
    imprimeMatriz(matrizB);

    if (!sonMultiplicables(matrizA, matrizB)){
        cerr << "Error: Las matrices no se pueden multiplicar" << endl;
        cerr << "Columnas de Matriz A (" << matrizA[0].size() << ") debe ser igual a filas de Matriz B (" << matrizB.size() << ")" << endl;
        return 1;
    }

    vector<vector<int>> MatrizC = multiplicaMatrices(matrizA, matrizB);

    cout << "Resultado de la multiplicación entre A y B:" << endl;
    imprimeMatriz(MatrizC); 

    return 0;
}