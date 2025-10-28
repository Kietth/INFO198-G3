#ifndef FUNC_MAT_H
#define FUNC_MAT_H

#include <vector>
#include <string>

using namespace std;

vector<vector<int>> multiplicaMatrices(const vector<vector<int>>& matrizA, const vector<vector<int>>& matrizB);

bool sonMultiplicables(const vector<vector<int>>& matrizA, const vector<vector<int>>& matrizB);

void imprimeMatriz(const vector<vector<int>>& matriz);


#endif