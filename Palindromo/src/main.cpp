#include <iostream>
#include <string>
#include <cctype>
#ifdef _WIN32
#include <windows.h>
inline int obtenerPID() {return GetCurrentProcessId();}
#else
#include <unistd.h>
inline int obtenerPID() {return getpid();}
#endif
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

    cout << "PID: " << obtenerPID() << endl;
    cout << "Ingresa un texto para verificar si es un palindromo: ";
    getline(cin, entrada);
    
    if (es_palindromo(entrada)) {
            cout << "El texto \"" << entrada << "\" es palindromo" << endl;
        } else {
            cout << "El texto \"" << entrada << "\" NO es palindromo" << endl;
        }
    

    return 0;
}