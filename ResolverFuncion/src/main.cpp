#include <iostream>
#include "funcion.h"

using namespace std;

int main() {
    int opcion;
    do {
        cout << "===== Sistema de Funciones =====\n";
        cout << "1. Evaluar f(x) = x^2 + 2x + 8\n";
        cout << "2. Salir\n";
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
            case 2:
                cout << "Saliendo...\n";
                break;
            default:
                cout << "Opcion invalida, intenta de nuevo.\n\n";
        }

    } while(opcion != 2);

    return 0;
}
