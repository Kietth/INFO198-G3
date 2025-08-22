#include <iostream>
#include <cstring>
#include "usuario.h" // archivo de cabecera
using namespace std;

int main() {
    int opcion;
    string nombreArchivo = "datos_usuarios.bin"; //USAR ENV "HAY QUE CAMBIAR ESTO"

    do {
        // Mostrar el menú
        cout << "\n----- Administrador de usuarios -----" << endl;
        cout << "1. Crear usuario" << endl;
        cout << "2. Eliminar usuario" << endl;
        cout << "3. Listar usuarios" << endl;
        cout << "4. Salir" << endl;
        cout << "------------------------" << endl;
        cout << "Seleccione una opcion: ";
        cin >> opcion;

        switch (opcion) {
            case 1: {
                //CREAR INTERFAZ!!!
                //GUARDAR USUARIO
                break;
            }
            case 2: {
                //CREAR INTERFAZ!!!
                //ELIMINAR USUARIO
                break;
            }
            case 3: {
                listarUsuarios(nombreArchivo);
                break;
            }
            case 4: {
                cout << "Saliendo del programa." << endl;
                break;
            }
            default: {
                cout << "Opcion invalida. Intente de nuevo." << endl;
                break;
            }
        }
    } while (opcion != 4);

    return 0;
}
