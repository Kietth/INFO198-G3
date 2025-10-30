#include "red_cliente.h"
#include "estado_cliente.h" // Incluimos los globales
#include "cliente.h" // Para g_server_ip y g_server_port

#include <iostream>
#include <sstream>
#include <algorithm>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <cstring> // para strerror
#include <fcntl.h> // Para fcntl (timeout)

using namespace std;

vector<string> split(const string& s, char delimiter) {
    vector<string> tokens; string token; istringstream ss(s);
    while (getline(ss, token, delimiter)) { if (!token.empty()) tokens.push_back(token); } return tokens;
}

GameState parse_game_state(const string& data) {
    GameState newState;

    string game_data = data;
    if (data.rfind("GS,", 0) == 0) {
        vector<string> gs_parts = split(data, ',');
        if (gs_parts.size() >= 3 && gs_parts[0] == "GS") {
            try {
                g_current_grid_size = stoi(gs_parts[1]);
                size_t first_comma = data.find(',');
                size_t second_comma = data.find(',', first_comma + 1);
                game_data = data.substr(second_comma + 1);
            } catch(...) {
                cerr << "Error al parsear GS tag" << endl;
                g_current_grid_size = 50;
            }
        }
    }

    size_t food_pos = game_data.find('F'); if (food_pos == string::npos) return newState;
    string players_data = game_data.substr(0, food_pos);
    string food_data = game_data.substr(food_pos + 1);

    vector<string> players_tokens = split(players_data, '|');
    for (const string& t : players_tokens) {
        vector<string> parts = split(t, ',');
        if (parts.size() >= 5 && parts[0][0] == 'J' && parts[2][0] == 'T') {
            SnakeData snake;
            try {
                snake.id = stoi(parts[0].substr(1));
                snake.isAlive = (parts[1] == "1");
                snake.equipo_id = stoi(parts[2].substr(1));

                for (size_t i = 3; i < parts.size(); i += 2) {
                    if (i + 1 < parts.size()) snake.body.push_back({stoi(parts[i]), stoi(parts[i+1])});
                }

                if (snake.body.size() >= 2) {
                    Posicion h=snake.body[0],n=snake.body[1];
                    if(h.x>n.x)snake.direction="RIGHT";
                    else if(h.x<n.x)snake.direction="LEFT";
                    else if(h.y>n.y)snake.direction="DOWN";
                    else snake.direction="UP";
                } else snake.direction="UP";

                if (!snake.body.empty()) newState.all_snakes.push_back(snake);
            } catch (...) {}
        }
    }
    vector<string> food_parts = split(food_data, ',');
    for (size_t i = 0; i < food_parts.size(); i += 2) { if (i + 1 < food_parts.size()) { try { newState.food.push_back({stoi(food_parts[i]), stoi(food_parts[i+1])}); } catch (...) {} } }
    return newState;
}

void receive_data_thread() {
    int thread_local_socket = g_client_socket.load();
    if (thread_local_socket == -1) {
        cerr << "ERROR: receive_data_thread iniciado con socket global inválido (-1)." << endl;
        g_receiver_should_run = false;
        return;
    }

    char buffer[4096]; string persistent_buffer = "";
    cout << "DEBUG: Hilo de recepción iniciado para socket " << thread_local_socket << endl;

    while (g_receiver_should_run) {
        fd_set readfds; FD_ZERO(&readfds); FD_SET(thread_local_socket, &readfds);
        struct timeval tv; tv.tv_sec = 0; tv.tv_usec = 50000;

        // Comprobar si debemos terminar
        if (!g_receiver_should_run || g_client_socket.load() != thread_local_socket) {
             cout << "DEBUG: Señal de detener o socket global cambió, cerrando hilo receptor para socket " << thread_local_socket << "." << endl;
             break;
        }

        int activity = select(thread_local_socket + 1, &readfds, NULL, NULL, &tv);

        if (activity < 0 && errno != EINTR) { cerr << "receive_data_thread: select() error: " << strerror(errno) << endl; break; }

        if (FD_ISSET(thread_local_socket, &readfds)) {
            int bytesRead = recv(thread_local_socket, buffer, sizeof(buffer) - 1, 0);

            if (bytesRead <= 0) {
                if (g_receiver_should_run) {
                    if (bytesRead == 0) cout << "Servidor cerró conexión para socket " << thread_local_socket << "." << endl;
                    else if (errno != ECONNRESET) cerr << "recv() error en socket " << thread_local_socket << ": " << strerror(errno) << endl;
                }
                g_receiver_should_run = false;
                break;
            }
            buffer[bytesRead] = '\0'; persistent_buffer += string(buffer);
            size_t msg_end_pos;
            while ((msg_end_pos = persistent_buffer.find_first_of(';')) != string::npos) {
                 string full_message = persistent_buffer.substr(0, msg_end_pos + 1); persistent_buffer.erase(0, msg_end_pos + 1);

                if (full_message.length() <= 1) continue;
                string received_data = full_message.substr(0, full_message.length() - 1);

                bool player_is_dead_flag = false;
                size_t gameover_pos = received_data.find(";GAMEOVER");
                if (gameover_pos != string::npos) {
                    player_is_dead_flag = true;
                    received_data.erase(gameover_pos, 9);
                }

                size_t comma_pos = received_data.find_first_of(',');
                string comando = "";
                string contenido = "";

                if (comma_pos != string::npos) {
                    comando = received_data.substr(0, comma_pos);
                    if (received_data.length() > comma_pos + 1) {
                        contenido = received_data.substr(comma_pos + 1);
                    }
                } else {
                    continue;
                }

                { lock_guard<mutex> lock(state_mutex);
                    if (g_client_state.load() == MENU) continue;

                    ClientState old_state = g_client_state.load();
                    ClientState new_state = old_state;
                    bool update_game_state = false;
                    int temp_player_id = g_my_player_id.load();
                    sf::Color temp_color = g_my_color;
                    bool temp_game_over_state = g_is_game_over.load();

                    if (comando == "WELCOME") {
                        try {
                            temp_player_id = stoi(contenido);
                            temp_color = playerColors[temp_player_id % playerColors.size()];
                            new_state = LOBBY;
                        } catch(...) {
                            temp_player_id = -1;
                            new_state = MENU;
                        }
                        g_my_player_id = temp_player_id;
                    }
                    else if (comando == "LOBBY") { new_state = LOBBY; g_lobby_message = contenido; }
                    else if (comando == "COUNTDOWN") { new_state = LOBBY; g_lobby_message = "Partida: " + contenido + "s"; }
                    else if (comando == "VICTORY") {
                        new_state = GAMEOVER;
                        temp_game_over_state = true;
                        g_lobby_message = contenido;
                    }
                    else if (comando == "PLAYING") {
                        if (old_state != GAMEOVER) new_state = PLAYING;

                        if (player_is_dead_flag && old_state != GAMEOVER) {
                             if (g_lobby_message != "¡VICTORIA!" && g_lobby_message != "¡VICTORIA DE EQUIPO!") {
                                 g_lobby_message = "Derrota.";
                             }
                        }
                        update_game_state = true;
                    }

                    if (new_state != old_state) { g_client_state.store(new_state); cout << "DEBUG: Estado cambiado -> " << new_state << endl; }
                    g_is_game_over.store(temp_game_over_state);
                    g_my_color = temp_color;
                    if (update_game_state) {
                        current_state = parse_game_state(contenido);
                    }
                } // Fin del lock
            } // Fin while find(';')
        } // Fin if FD_ISSET
    } // Fin while g_receiver_should_run

    cout << "DEBUG: Hilo de recepción para socket " << thread_local_socket << " terminando bucle..." << endl;
    g_receiver_should_run = false;

    {
        lock_guard<mutex> lock(state_mutex);
        int current_global_socket = g_client_socket.load();

        if (current_global_socket == thread_local_socket && g_client_state.load() != MENU) {
            cout << "DEBUG: Hilo de recepción (socket " << thread_local_socket << ") cambiando estado a MENU y limpiando socket global." << endl;
            g_client_state = MENU;
            g_lobby_message = "Desconectado.";
            g_is_game_over = false;
            g_my_player_id = -1;
            current_state = GameState();
            g_client_socket.store(-1); 

            shutdown(thread_local_socket, SHUT_RDWR);
            close(thread_local_socket);
            cout << "DEBUG: Hilo de recepción cerró socket " << thread_local_socket << "." << endl;
        } else {
             cout << "DEBUG: Hilo de recepción (socket " << thread_local_socket << ") terminando sin limpiar estado global (socket global: " << current_global_socket << ", estado: " << g_client_state.load() << ")." << endl;
             // Si el socket global es -1 (el usuario volvió al menú), solo cerramos nuestro socket local si no ha sido cerrado ya
             if (current_global_socket == -1) {
                 shutdown(thread_local_socket, SHUT_RDWR);
                 close(thread_local_socket);
                 cout << "DEBUG: Hilo de recepción cerró socket obsoleto " << thread_local_socket << "." << endl;
             }
        }
    }
    cout << "DEBUG: Hilo de recepción para socket " << thread_local_socket << " terminado completamente." << endl;
}


bool connect_to_server(int& outSocket) {
    // Usa g_server_ip y g_server_port leídos del .env (o los defaults)
    cout << "Intentando conectar a " << g_server_ip << ":" << g_server_port << "..." << endl;
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        cerr << "Error socket cliente." << endl;
        return false;
    }
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(g_server_port); 

    // <-- Usa IP global
    if (inet_pton(AF_INET, g_server_ip.c_str(), &serverAddress.sin_addr) <= 0) {
        cerr << "IP inválida: " << g_server_ip << endl;
        close(sock);
        return false;
    }

    // --- Timeout para la conexión ---
    int flags = fcntl(sock, F_GETFL, 0);
    if (flags == -1) { perror("fcntl get flags error"); close(sock); return false; }
    if (fcntl(sock, F_SETFL, flags | O_NONBLOCK) == -1) { perror("fcntl set nonblock error"); close(sock); return false; }

    int connect_res = connect(sock, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
    if (connect_res < 0) {
        if (errno == EINPROGRESS) {
            fd_set writefds;
            FD_ZERO(&writefds);
            FD_SET(sock, &writefds);
            struct timeval tv;
            tv.tv_sec = 5; // 5 segundos de timeout
            tv.tv_usec = 0;

            int select_res = select(sock + 1, NULL, &writefds, NULL, &tv);
            if (select_res > 0) {
                int so_error;
                socklen_t len = sizeof(so_error);
                getsockopt(sock, SOL_SOCKET, SO_ERROR, &so_error, &len);
                if (so_error == 0) {
                    connect_res = 0; // Conexión exitosa
                } else {
                    cerr << "Error connect (después de select): " << strerror(so_error) << endl;
                }
            } else if (select_res == 0) {
                cerr << "Error connect: Timeout." << endl;
            } else {
                 cerr << "Error select en connect: " << strerror(errno) << endl;
            }
        } else {
            cerr << "Error connect: " << strerror(errno) << endl;
        }
    }

    // Restaurar modo bloqueante
    if (fcntl(sock, F_SETFL, flags) == -1) { perror("fcntl restore block error"); }

    if (connect_res != 0) {
         close(sock);
         return false;
    }


    cout << "Conectado." << endl;
    outSocket = sock;
    return true;
}
