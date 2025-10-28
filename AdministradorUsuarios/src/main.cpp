// === MODIFICACIÓN EN AdministradorUsuarios/src/main.cpp ===
#include <iostream>
#include <cstdlib>
#include <fstream>
#include "Sistema.h"
using namespace std;

int main(int argc, char* argv[]) {
    // Obtener rutas de los archivos desde argumentos o usar valores por defecto
    string archivoUsuarios = "USUARIOS.TXT";
    string archivoPerfiles = "PERFILES.TXT";
    
    if (argc >= 3) {
        archivoUsuarios = argv[1];
        archivoPerfiles = argv[2];
    }
    
    cout << "Tamaño del struct Usuario: " << sizeof(Usuario) << " bytes" << endl;
    cout << "=== SISTEMA ADMINISTRADOR DE USUARIOS ===" << endl;
    cout << "Archivo de usuarios: " << archivoUsuarios << endl;
    cout << "Archivo de perfiles: " << archivoPerfiles << endl;
    cout << "=========================================" << endl;
    
    // Verificar si los archivos existen
    ifstream usuariosFile(archivoUsuarios);
    if (!usuariosFile.is_open()) {
        cout << "AVISO: El archivo de usuarios no existe." << endl;
        cout << "Se creará cuando agregue el primer usuario." << endl;
    } else {
        usuariosFile.close();
    }
    
    ifstream perfilesFile(archivoPerfiles);
    if (!perfilesFile.is_open()) {
        cout << "ERROR: El archivo de perfiles no existe." << endl;
        cout << "Por favor, cree el archivo PERFILES.TXT con la configuración de perfiles." << endl;
        return 1; // Salir
    } else {
        perfilesFile.close();
    }

    cout << endl;
    
    Sistema sistema(archivoUsuarios, archivoPerfiles);
    sistema.ejecutar();
    
    return 0;
}