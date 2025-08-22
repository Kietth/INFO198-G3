#include "usuario.h"
#include <iostream>
#include <fstream>
#include <cstring>
using namespace std;

//Recibe un usuario y lo inggresa al archivo 
void guardarUsuario(const Usuario& usuario, const string& nombreArchivo) {
    ofstream archivo(nombreArchivo, ios::binary | ios::app); //agrega al final del archivo
    if (archivo.is_open()) {
        archivo.write(reinterpret_cast<const char*>(&usuario), sizeof(Usuario));
        archivo.close();
        cout << "Usuario guardado exitosamente." << endl;
    } else {
        cerr << "Error: No se pudo abrir o guardar el archivo." << endl;
    }
}