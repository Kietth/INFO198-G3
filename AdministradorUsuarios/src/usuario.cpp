#include "Usuario.h"
#include <algorithm>
#include <cctype>
#include <cstring>
using namespace std;

Usuario::Usuario() : id(0) {
    strcpy(nombre, "");
    strcpy(username, "");
    strcpy(password, "");
    strcpy(perfil, "");
}

Usuario::Usuario(int id, const char* nombre, const char* username,
                 const char* password, const char* perfil) : id(id) {
    strncpy(this->nombre, nombre, sizeof(this->nombre) - 1);
    this->nombre[sizeof(this->nombre) - 1] = '\0';
    
    strncpy(this->username, username, sizeof(this->username) - 1);
    this->username[sizeof(this->username) - 1] = '\0';
    
    strncpy(this->password, password, sizeof(this->password) - 1);
    this->password[sizeof(this->password) - 1] = '\0';
    
    strncpy(this->perfil, perfil, sizeof(this->perfil) - 1);
    this->perfil[sizeof(this->perfil) - 1] = '\0';
    
    toUpperPerfil();
}

void Usuario::toUpperPerfil() {
    for (int i = 0; perfil[i] != '\0'; i++) {
        perfil[i] = toupper(perfil[i]);
    }
}

bool Usuario::isEmpty() const {
    return id == 0 && nombre[0] == '\0' && username[0] == '\0' && 
           password[0] == '\0' && perfil[0] == '\0';
}