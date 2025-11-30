#include <iostream>
#include "funcion.h"
#ifdef _WIN32
#include <windows.h>
inline int obtenerPID() {return GetCurrentProcessId();}
#else
#include <unistd.h>
inline int obtenerPID() {return getpid();}
#endif
using namespace std;

int main() {
    int opcion;
    do {
        cout << "===== Sistema de Funciones ===== PID: "<< obtenerPID() << endl;
        cout << "1. Evaluar f(x) = x^2 + 2x + 8\n";
        cout << "0. Salir\n";
        cout << "Selecciona una opcion: ";
        cin >> opcion;

        switch(opcion) {
            case 1: {
                double x;
                cout << "Ingresa un valor para x: ";
                cin >> x;
                double resultado = f(x);
                cout << "f(" << x << ") = " << resultado << endl << endl;
                break;
            }
            case 0: {
                cout << "Saliendo...\n";
                break;
            }
            default:
                cout << "Opcion invalida, intenta de nuevo.\n\n";
        }

    } while(opcion != 0);

    return 0;
}
