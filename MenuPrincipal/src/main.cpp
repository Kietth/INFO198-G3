#include "Menu.h"
#include "Auth.h"
#include <iostream>
#include <cstdlib>
#include <limits>
using namespace std;

void esperar() {
    cout << "\nPresione Enter para continuar...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

void Menu::mostrar(const string& user, const string& pass) {
    Auth auth("AdministradorUsuarios/USUARIOS.TXT",
              "AdministradorUsuarios/PERFILES.TXT");

    if (!auth.login(user, pass)) {
        cout << "Usuario o contraseña inválidos\n";
        return;
    }

    cout << "Bienvenido, " << auth.getNombre()
         << " (perfil: " << auth.getPerfil() << ")\n";

    int opcion;
    do {
        cout << "\n=== MENU PRINCIPAL ===\n";
        cout << "0. Salir\n";

        if (auth.tienePermiso(1)) cout << "1. Administrador de usuarios y perfiles\n";
        if (auth.tienePermiso(2)) cout << "2. Multiplica matrices NxN\n";
        if (auth.tienePermiso(3)) cout << "3. Juego\n";
        if (auth.tienePermiso(4)) cout << "4. ¿Es palíndromo?\n";
        if (auth.tienePermiso(5)) cout << "5. Calcular f(x)=x^2+2x+8\n";
        if (auth.tienePermiso(6)) cout << "6. Conteo sobre texto\n";

        cout << "Seleccione: ";
        cin >> opcion;

        if (opcion == 0) {
            cout << "Saliendo...\n";
            break;
        }

        if (!auth.tienePermiso(opcion)) {
            cout << "No tiene permiso para esta opción.\n";
            esperar();
            continue;
        }

        switch(opcion) {
            case 1:
                cout << "Modulo Administrador de usuarios y perfiles (en construcción)\n";
                esperar();
                break;
            case 2:
                cout << "Modulo Multiplica matrices NxN (en construcción)\n";
                esperar();
                break;
            case 3:
                cout << "Juego (en construcción)\n";
                esperar();
                break;
            case 4:
                system("./Palindromo/palindromo");
                esperar();
                break;
            case 5:
                system("./ResolverFuncion/resolver_funcion");
                esperar();
                break;
            case 6:
                cout << "Conteo sobre texto (en construcción)\n";
                esperar();
                break;
            default:
                cout << "Opción inválida\n";
                esperar();
        }
    } while(true);
}


int main(int argc, char* argv[]) {
    string user, pass;

    // Leer parámetros -u y -p
    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        if (arg == "-u" && i + 1 < argc) {
            user = argv[++i];
        } else if (arg == "-p" && i + 1 < argc) {
            pass = argv[++i];
        }
    }

    if (user.empty() || pass.empty()) {
        cerr << "Uso: " << argv[0] << " -u <usuario> -p <password>\n";
        return 1;
    }

    Menu menu;
    menu.mostrar(user, pass);

    return 0;
}
