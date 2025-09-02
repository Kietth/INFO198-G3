#include "Usuario.h"
#include <sstream>
#include <vector>
#include <algorithm>
#include <cctype>
using namespace std;

Usuario::Usuario() : id(0) {}

Usuario::Usuario(int id, const string& nombre, const string& username, 
                 const string& password, const string& perfil)
    : id(id), nombre(nombre), username(username), password(password) {
    // Convertir perfil a mayúsculas al construirlo
    this->perfil = perfil;
    transform(this->perfil.begin(), this->perfil.end(), this->perfil.begin(), ::toupper);
}

string Usuario::toString() const {
    stringstream ss;
    ss << id << ";" << nombre << ";" << username << ";" << password << ";" << perfil;
    return ss.str();
}

Usuario Usuario::fromString(const string& linea) {
    vector<string> partes;
    stringstream ss(linea);
    string item;
    
    while (getline(ss, item, ';')) {
        partes.push_back(item);
    }
    
    if (partes.size() == 5) {
        int id = stoi(partes[0]);
        // El perfil se convertirá a mayúsculas en el constructor
        return Usuario(id, partes[1], partes[2], partes[3], partes[4]);
    }
    return Usuario();
}