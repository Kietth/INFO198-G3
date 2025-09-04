#include "../include/op_archivos.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

vector<vector<int>> leeMatriz(const string& nombreArchivo) {
    ifstream archivo(nombreArchivo);
    vector<vector<int>> matriz;
    
    if (!archivo.is_open()) {
        cerr << "Error: No se pudo abrir el archivo " << nombreArchivo << endl;
        return matriz;
    }
    
    string linea;
    int columnas_esperadas = -1;
    while (getline(archivo, linea)) {
        vector<int> fila;
        stringstream ss(linea);
        string token;
        
        while (ss >> token) {
            try{
                int valor = stoll(token);
                fila.push_back(valor);
            } catch (const exception& e){
                cerr << "Error: Valor no valido '" << token << "'" << endl;
                matriz.clear();
                archivo.close();
                return matriz;
            }
        }
        
        if (!fila.empty()) {
            if(columnas_esperadas == -1){
                columnas_esperadas = fila.size();
            }else if (fila.size() != columnas_esperadas){
                cerr << "Error: Filas de tamaño inconsistente en " << nombreArchivo << endl;
                matriz.clear();
                archivo.close();
                return matriz;
            }
            matriz.push_back(fila);
        }
    }
    
    archivo.close();
    return matriz;
}