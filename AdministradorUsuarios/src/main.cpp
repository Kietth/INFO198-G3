#include <iostream>
#include <cstdlib>
#include <fstream>
#include "Sistema.h"

using namespace std;

int main() {
     //para verificar
    cout << "Tamaño del struct Usuario: " << sizeof(Usuario) << " bytes" << endl;
    // Cargar variables de entorno desde archivo .env
    const char* userFile = getenv("USER_FILE");
    const char* profileFile = getenv("PROFILE_FILE");
    
    string archivoUsuarios = userFile ? userFile : "USUARIOS.TXT";
    string archivoPerfiles = profileFile ? profileFile : "PERFILES.TXT";
    
    cout << "=== SISTEMA ADMINISTRADOR DE USUARIOS ==="<< endl;
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