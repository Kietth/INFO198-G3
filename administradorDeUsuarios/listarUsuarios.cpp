#include "usuario.h"
#include <iostream>
#include <fstream>
#include <cstring>
using namespace std;

void listarUsuarios(const string& nombreArchivo) {
    ifstream archivo(nombreArchivo, ios::binary);
    if (!archivo.is_open()) {
        cerr << "Error: No se pudo abrir el archivo para lectura." << endl;
        return;
    }

    Usuario usuarioActual;
    cout << "Lista de usuarios:" << endl << "------------------------" << endl;
    
    // Leer cada usuario hasta el final del archivo
    while (archivo.read(reinterpret_cast<char*>(&usuarioActual), sizeof(Usuario))) {
        cout << "ID: " << usuarioActual.id << endl;
        cout << "Nombre: " << usuarioActual.nombre << endl;
        cout << "Perfil: " << usuarioActual.perfil << endl;
        cout << "------------------------" << endl;
    }

    archivo.close();
}