#include <iostream>
#include <string>
#include <cctype>

using namespace std;

bool es_palindromo(const string& palabra) {
    int i = 0;
    int j = palabra.length() - 1;

    while (i < j) {
        while (i < j && !isalnum(palabra[i])) {
            i++;
        }
        while (i < j && !isalnum(palabra[j])) {
            j--;
        }
        if (tolower(palabra[i]) != tolower(palabra[j])) {
            return false;
        }

        i++;
        j--;
    }
    return true;
}

int main() {
    string entrada;

    cout << "Ingresa una palabra para verificar si es un palindromo: ";
    getline(cin, entrada);

    // Verificamos si la cadena contiene un espacio
    if (entrada.find(' ') != string::npos) {
        cout << "Error: Por favor, ingresa una sola palabra" << endl;
    } else {
        if (es_palindromo(entrada)) {
            cout << "La palabra \"" << entrada << "\" es palindromo" << endl;
        } else {
            cout << "La palabra \"" << entrada << "\" NO es palindromo" << endl;
        }
    }

    return 0;
}
