#include "servidor.h"
#include <system_error> // Para std::system_error

// Faltaban estos includes para las funciones de red usadas en main()
#include <sys/socket.h> 
#include <unistd.h>     
#include <arpa/inet.h>
#include <cstring>      // Para strerror

int main() {
    srand(time(NULL));
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) { std::cerr << "Error al crear socket: " << strerror(errno) << std::endl; return -1;}
    
    int opt = 1; 
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET; 
    serverAddress.sin_port = htons(PORT); 
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) { 
        std::cerr << "Bind error: " << strerror(errno) << std::endl; 
        close(serverSocket); 
        return -1; 
    }
    
    if (listen(serverSocket, 10) == -1) { 
        std::cerr << "Listen error: " << strerror(errno) << std::endl; 
        close(serverSocket); 
        return -1; 
    }
    
    std::cout << "Servidor esperando en puerto " << PORT << "..." << std::endl;

    std::thread game_thread;
     try { 
         // gameLoop() se declara en logica.h y se define en logica.cpp
         game_thread = std::thread(gameLoop); 
         game_thread.detach(); 
     }
     catch (const std::system_error& e) { 
         std::cerr << "Error al crear hilo gameLoop: " << e.what() << std::endl; 
         close(serverSocket); 
         return -1; 
     }

    while (true) {
        int clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket == -1) { 
            if (errno != EINTR) { 
                std::cerr << "Accept error: " << strerror(errno) << std::endl; 
            } 
            continue; 
        }
        
        int current_id;
        { 
            std::lock_guard<std::mutex> lock(g_mutex); // g_mutex se define en logica.cpp
            current_id = g_next_player_id++;           // g_next_player_id se define en logica.cpp
        }
        
        try { 
            // manejarCliente se declara en red_servidor.h y se define en red_servidor.cpp
            std::thread clientThread(manejarCliente, clientSocket, current_id); 
            clientThread.detach(); 
        }
        catch (const std::system_error& e) { 
            std::cerr << "Error al crear hilo para cliente #" << current_id << ": " << e.what() << std::endl; 
            close(clientSocket); 
        }
    }
    
    close(serverSocket); 
    return 0;
}