#include "../include/funcmatrices.h"
#include <iostream>
#include <vector>


using namespace std;

void imprimeMatriz(const vector<vector<int>>& matriz){
    int filas = matriz.size();
    for (int i=0; i<filas; i++){
        int columnas = matriz[i].size();

        for (int j=0; j<columnas; j++){
            cout << matriz[i][j] << "\t";
        }
        cout << endl;
    };
}

bool sonMultiplicables(const vector<vector<int>>& matrizA, const vector<vector<int>>& matrizB){
    return (matrizA[0].size() == matrizB.size());
}

vector<vector<int>> multiplicaMatrices(const vector<vector<int>>& matrizA, const vector<vector<int>>& matrizB){
    int filasA = matrizA.size();
    int columnasA = matrizA[0].size();
    int columnasB = matrizB[0].size();

    vector<vector<int>> matrizC(filasA, vector<int>(columnasB, 0));

    for(int i=0; i<filasA; i++){
        for(int j=0; j<columnasB; j++){
            for(int k=0; k<columnasA; k++){
                matrizC[i][j] += matrizA[i][k] * matrizB[k][j];
            }
        }
    }

    return matrizC;
}