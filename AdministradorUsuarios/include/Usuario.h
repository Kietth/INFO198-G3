#ifndef USUARIO_H
#define USUARIO_H

#include <string>
using namespace std;

struct Usuario {
    int id;
    string nombre;
    string username;
    string password;
    string perfil;
    
    Usuario();
    Usuario(int id, const string& nombre, const string& username, 
            const string& password, const string& perfil);
    
    string toString() const;
    static Usuario fromString(const string& linea);
};

#endif