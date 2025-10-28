#ifndef USUARIO_H
#define USUARIO_H

#include <cstring>

struct Usuario {
    int id;
    char nombre[100];
    char username[50];
    char password[50];
    char perfil[20];
    
    Usuario();
    Usuario(int id, const char* nombre, const char* username, 
            const char* password, const char* perfil);
    
    void toUpperPerfil();
    bool isEmpty() const;
};

#endif