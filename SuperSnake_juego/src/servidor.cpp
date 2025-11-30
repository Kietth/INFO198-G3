#include "servidor.h"
#include <system_error> // Para system_error
#include <sys/socket.h> 
#include <unistd.h>     
#include <arpa/inet.h>
#include <cstring>      // Para strerror

using namespace std;

int main() {
    srand(time(NULL));
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) { cerr << "Error al crear socket: " << strerror(errno) << endl; return -1;}
    
    int opt = 1; 
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET; 
    serverAddress.sin_port = htons(PORT); 
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) { 
        cerr << "Bind error: " << strerror(errno) << endl; 
        close(serverSocket); 
        return -1; 
    }
    
    if (listen(serverSocket, 10) == -1) { 
        cerr << "Listen error: " << strerror(errno) << endl; 
        close(serverSocket); 
        return -1; 
    }
    
    cout << "Servidor esperando en puerto " << PORT << "..." << endl;

    thread game_thread;
     try { 
         // gameLoop() se declara en logica.h y se define en logica.cpp
         game_thread = thread(gameLoop); 
         game_thread.detach(); 
     }
     catch (const system_error& e) { 
         cerr << "Error al crear hilo gameLoop: " << e.what() << endl; 
         close(serverSocket); 
         return -1; 
     }

    while (true) {
        int clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket == -1) { 
            if (errno != EINTR) { 
                cerr << "Accept error: " << strerror(errno) << endl; 
            } 
            continue; 
        }
        
        int current_id;
        { 
            lock_guard<mutex> lock(g_mutex); // g_mutex se define en logica.cpp
            current_id = g_next_player_id++;           // g_next_player_id se define en logica.cpp
        }
        
        try { 
            // manejarCliente se declara en red_servidor.h y se define en red_servidor.cpp
            thread clientThread(manejarCliente, clientSocket, current_id); 
            clientThread.detach(); 
        }
        catch (const system_error& e) { 
            cerr << "Error al crear hilo para cliente #" << current_id << ": " << e.what() << endl; 
            close(clientSocket); 
        }
    }
    
    close(serverSocket); 
    return 0;
}