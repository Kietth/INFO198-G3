#include "usuario.h"
#include <iostream>
#include <fstream>
#include <cstdio> // Necesario para remove() y rename()
#include <cstring>
using namespace std;

//Recibe un ID y elimina ese usuario
void eliminarUsuario(int usuarioId, const string& nombreArchivo) {
    ifstream archivoLectura(nombreArchivo, ios::binary);
    if (!archivoLectura.is_open()) {
        cerr << "Error: No se pudo abrir el archivo para lectura." << endl;
        return;
    }

    ofstream archivoTemporal("temp.bin", ios::binary);
    if (!archivoTemporal.is_open()) {
        cerr << "Error: No se pudo crear el archivo temporal." << endl;
        archivoLectura.close();
        return;
    }

    Usuario usuarioActual;
    bool encontrado = false;

    // Leer el archivo original y escribir en el archivo temporal
    while (archivoLectura.read(reinterpret_cast<char*>(&usuarioActual), sizeof(Usuario))) {
        if (usuarioActual.id != usuarioId) {
            archivoTemporal.write(reinterpret_cast<const char*>(&usuarioActual), sizeof(Usuario));
        } else {
            encontrado = true;
        }
    }

    archivoLectura.close();
    archivoTemporal.close();

    // Reemplazar el archivo original con el archivo temporal
    if (encontrado) {
        remove(nombreArchivo.c_str());
        rename("temp.bin", nombreArchivo.c_str());
        cout << "Usuario con ID " << usuarioId << " eliminado exitosamente." << endl;
    } else {
        remove("temp.bin"); // Eliminar el archivo temporal si no se encontró el usuario
        cerr << "Error: No se encontró un usuario con el ID " << usuarioId << "." << endl;
    }
}