#include <iostream>
#include <cstdlib>
#include "Sistema.h"
using namespace std;

int main() {
    // Cargar variables de entorno desde archivo .env
    const char* userFile = getenv("USER_FILE");
    const char* profileFile = getenv("PROFILE_FILE");
    
    string archivoUsuarios = userFile ? userFile : "USUARIOS.TXT";
    string archivoPerfiles = profileFile ? profileFile : "PERFILES.TXT";
    
    cout << "Archivo de usuarios: " << archivoUsuarios << endl;
    cout << "Archivo de perfiles: " << archivoPerfiles << endl;
    
    Sistema sistema(archivoUsuarios, archivoPerfiles);
    sistema.ejecutar();
    
    return 0;
}