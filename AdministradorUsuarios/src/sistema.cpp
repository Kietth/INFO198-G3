#include "Sistema.h"
#include <iostream>
#include <algorithm>
#include <limits>
#include <cstring> 
using namespace std;

Sistema::Sistema(const string& archivoUsuarios, const string& archivoPerfiles)
    : archivoManager(archivoUsuarios), perfilManager(archivoPerfiles), datosEnMemoria(false) {}

void Sistema::cargarUsuarios() {
    if (!datosEnMemoria) {
        usuarios = archivoManager.leerUsuarios();
        datosEnMemoria = true;
    }
}

void Sistema::mostrarMenu() const {
    cout << "\n=== ADMINISTRADOR DE USUARIOS ===" << endl;
    cout << "1. Ingresar usuario" << endl;
    cout << "2. Listar usuarios" << endl;
    cout << "3. Eliminar usuario" << endl;
    cout << "4. Gestionar perfiles" << endl;
    cout << "0. Salir" << endl;
    cout << "Seleccione una opcion: ";
}

void Sistema::mostrarMenuPerfiles() const {
    cout << "\n=== GESTION DE PERFILES ===" << endl;
    cout << "1. Listar perfiles y permisos" << endl;
    cout << "0. Volver al menu principal" << endl;
    cout << "Seleccione una opcion: ";
}

void Sistema::ingresarUsuario() {
    string nombre, username, password, perfil;
    int id = archivoManager.obtenerProximoId();
    
    cout << "\n--- Ingresar Nuevo Usuario ---" << endl;
    
    cin.ignore();
    cout << "Nombre completo: ";
    getline(cin, nombre);
    
    cout << "Username (nickname): ";
    getline(cin, username);
    
    cout << "Password: ";
    getline(cin, password);
    
    vector<string> perfiles = perfilManager.getPerfiles();
    
    // Verificar si hay perfiles disponibles
    if (perfiles.empty()) {
        cout << "ERROR: No hay perfiles configurados en el sistema." << endl;
        cout << "Por favor, cree el archivo PERFILES.TXT con los perfiles necesarios." << endl;
        cout << "\nPresione Enter para volver...";
        cin.ignore();
        cin.get();
        return;
    }
    
    cout << "Perfiles disponibles: ";
    for (size_t i = 0; i < perfiles.size(); ++i) {
        cout << perfiles[i];
        if (i < perfiles.size() - 1) cout << ", ";
    }
    cout << endl;
    
    cout << "Perfil: ";
    getline(cin, perfil);
    
    // Convertir a mayúsculas para validación
    string perfilUpper = perfil;
    transform(perfilUpper.begin(), perfilUpper.end(), perfilUpper.begin(), ::toupper);
    
    if (!perfilManager.existePerfil(perfil) && !perfilManager.existePerfil(perfilUpper)) {
        cout << "Error: Perfil no valido. Usando 'GENERAL' por defecto." << endl;
        perfil = "GENERAL";
    }
    
    
    Usuario nuevoUsuario(id, nombre.c_str(), username.c_str(), password.c_str(), perfil.c_str());
    
    char confirmar;
    cout << "\nDesea guardar este usuario? (s/n): ";
    cin >> confirmar;
    
    if (confirmar == 's' || confirmar == 'S') {
        archivoManager.guardarUsuario(nuevoUsuario);
        usuarios.push_back(nuevoUsuario);
        cout << "Usuario guardado exitosamente!" << endl;
    } else {
        cout << "Operacion cancelada." << endl;
    }
}

void Sistema::listarUsuarios() const {
    cout << "\n--- Lista de Usuarios ---" << endl;
    
    if (usuarios.empty()) {
        cout << "No hay usuarios registrados." << endl;
        cout << "\n1) Para volver: ";
        int opcion;
        cin >> opcion;
        return;
    }
    
    cout << "ID | Nombre | Username | Perfil" << endl;
    cout << "--------------------------------" << endl;
    
    for (const auto& usuario : usuarios) {
        cout << usuario.id << " | " << usuario.nombre << " | " 
             << usuario.username << " | " << usuario.perfil << endl;
    }
    
    cout << "\n1) Para volver: ";
    int opcion;
    cin >> opcion;
}

void Sistema::eliminarUsuario() {
    int id;
    cout << "\n--- Eliminar Usuario ---" << endl;
    cout << "ID del usuario a eliminar: ";
    cin >> id;
    
    auto it = find_if(usuarios.begin(), usuarios.end(), 
        [id](const Usuario& u) { return u.id == id; });
    
    if (it != usuarios.end()) {
        // Usar strcmp para comparar arrays de char
        if (strcmp(it->perfil, "ADMIN") == 0) {
            cout << "ALERTA: Esta intentando eliminar un usuario con perfil ADMIN." << endl;
            cout << "Esta seguro que desea continuar? (s/n): ";
            
            char confirmar;
            cin >> confirmar;
            
            if (confirmar != 's' && confirmar != 'S') {
                cout << "Operacion cancelada." << endl;
                return;
            }
        }
        
        archivoManager.eliminarUsuario(id);
        usuarios.erase(it);
        cout << "Usuario eliminado exitosamente!" << endl;
    } else {
        cout << "Error: Usuario con ID " << id << " no encontrado." << endl;
    }
}

void Sistema::listarPerfiles() const {
    cout << "\n--- Perfiles y Permisos ---" << endl;
    auto perfiles = perfilManager.getPerfiles();
    
    if (perfiles.empty()) {
        cout << "ERROR: No hay perfiles configurados en el sistema." << endl;
        cout << "Verifique que el archivo PERFILES.TXT exista y tenga el formato correcto." << endl;
        cout << "\n1) Para volver: ";
        int opcion;
        cin >> opcion;
        return;
    }
    
    for (const auto& perfil : perfiles) {
        cout << "Perfil: " << perfil << " - Permisos: ";
        auto permisos = perfilManager.getPermisos(perfil);
        for (size_t i = 0; i < permisos.size(); ++i) {
            cout << permisos[i];
            if (i < permisos.size() - 1) cout << ", ";
        }
        cout << endl;
    }
    
    cout << "\n1) Para volver: ";
    int opcion;
    cin >> opcion;
}

void Sistema::gestionarPerfiles() {
    int opcion;
    do {
        mostrarMenuPerfiles();
        cin >> opcion;
        
        switch (opcion) {
            case 1:
                listarPerfiles();
                break;
            case 0:
                cout << "Volviendo al menu principal..." << endl;
                break;
            default:
                cout << "Opcion no valida. Intente de nuevo." << endl;
        }
        
    } while (opcion != 0);
}

void Sistema::ejecutar() {
    cargarUsuarios();
    
    int opcion;
    do {
        mostrarMenu();
        cin >> opcion;
        
        switch (opcion) {
            case 1:
                ingresarUsuario();
                break;
            case 2:
                listarUsuarios();
                break;
            case 3:
                eliminarUsuario();
                break;
            case 4:
                gestionarPerfiles();
                break;
            case 0:
                cout << "Saliendo del sistema..." << endl;
                break;
            default:
                cout << "Opcion no valida. Intente de nuevo." << endl;
        }
        
        if (opcion != 0) {
            cout << "\nPresione Enter para continuar...";
            cin.ignore();
            cin.get();
        }
        
    } while (opcion != 0);
}