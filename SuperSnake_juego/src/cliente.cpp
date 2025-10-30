#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <thread>
#include <iostream>
#include <system_error>
// --- AÑADIDO ---
#include <fstream>   // Para leer archivos
#include <string>    // Para std::string
#include <stdexcept> // Para std::invalid_argument, std::out_of_range
// --- FIN AÑADIDO ---


#include <sys/socket.h>
#include <unistd.h>

#include "cliente.h"
#include "estado_cliente.h"
#include "graficos.h"
#include "red_cliente.h"
#include "common.h" // Necesario para PORT como default

// --- Definición de globales de estado ---
std::mutex state_mutex;
GameState current_state;
std::string current_direction = "UP";
std::atomic<ClientState> g_client_state = MENU;
std::atomic<GameMode> g_game_mode = SOLO;
std::string g_lobby_message = "";
std::atomic<bool> g_is_game_over = false; // Indica si la PARTIDA ha terminado
std::atomic<int> g_my_player_id = -1;
sf::Color g_my_color = sf::Color::White;
int g_current_grid_size = 50;
std::atomic<bool> g_is_boosting = false;
std::atomic<int> g_client_socket = -1;
std::atomic<bool> g_receiver_should_run = false;

// --- AÑADIDO: Variables globales para IP y Puerto ---
std::string g_server_ip = "127.0.0.1"; // Default IP
int g_server_port = PORT;             // Default Port (de common.h)
// --- FIN AÑADIDO ---


// --- NUEVA FUNCIÓN: Leer .env ---
void loadEnv() {
    // Asume que .env está en el mismo directorio que el ejecutable (raíz)
    std::ifstream env_file(".env");
    std::string line;
    if (!env_file.is_open()) {
        std::cerr << "Advertencia: No se encontró el archivo .env. Usando valores por defecto ("
                  << g_server_ip << ":" << g_server_port << ")." << std::endl;
        return;
    }

    while (std::getline(env_file, line)) {
        // Ignorar líneas vacías o comentarios
        if (line.empty() || line.find_first_not_of(" \t") == std::string::npos || line[line.find_first_not_of(" \t")] == '#') {
            continue;
        }

        size_t separator_pos = line.find('=');
        if (separator_pos != std::string::npos) {
            std::string key = line.substr(0, separator_pos);
            std::string value = line.substr(separator_pos + 1);

            // Quitar espacios extra (simple)
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);


            if (key == "SERVER_IP") {
                g_server_ip = value;
            } else if (key == "SERVER_PORT") {
                try {
                    g_server_port = std::stoi(value); // Usar stoi para mejor manejo de errores
                } catch (const std::invalid_argument& e) {
                    std::cerr << "Error: Puerto inválido en .env: '" << value << "'. Usando puerto por defecto (" << PORT << ")." << std::endl;
                    g_server_port = PORT; // Revertir a default si es inválido
                } catch (const std::out_of_range& e) {
                     std::cerr << "Error: Puerto fuera de rango en .env: '" << value << "'. Usando puerto por defecto (" << PORT << ")." << std::endl;
                    g_server_port = PORT; // Revertir a default si está fuera de rango
                }
            }
        }
    }
    env_file.close();
    std::cout << "Configuración cargada: Conectando a " << g_server_ip << ":" << g_server_port << std::endl;
}
// --- FIN NUEVA FUNCIÓN ---


// --- Prototipos de manejo de eventos ---
void handleInput(sf::Event& event, std::thread& receiver_thread_obj);
void handleKeyRelease(sf::Event& event);
void sendDirectionIfNeeded(sf::Clock& input_clock);

int main() {
    // --- AÑADIDO: Cargar configuración al inicio ---
    loadEnv();
    // --- FIN AÑADIDO ---

    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Super Snake");
    window.setFramerateLimit(60);

    if (!cargarAssets()) {
        return -1;
    }

    std::thread receiver_thread_obj;
    sf::Clock input_clock;

    // --- BUCLE PRINCIPAL (sin cambios) ---
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                 g_receiver_should_run = false;
                 if (receiver_thread_obj.joinable()) {
                    std::cout << "DEBUG: Esperando hilo de recepción al cerrar ventana..." << std::endl;
                    receiver_thread_obj.join();
                    std::cout << "DEBUG: Hilo de recepción terminado al cerrar ventana." << std::endl;
                 }
                 window.close();
            }
            if (event.type == sf::Event::KeyPressed) {
                handleInput(event, receiver_thread_obj);
            }
            else if (event.type == sf::Event::KeyReleased) {
                handleKeyRelease(event);
            }
        }

        sendDirectionIfNeeded(input_clock);

        window.clear(sf::Color::Black);
        dibujarUI(window);
        dibujarEstado(window);
        window.display();
    }
    // --- FIN BUCLE PRINCIPAL ---


    // --- LIMPIEZA FINAL (sin cambios) ---
    g_receiver_should_run = false;
    int finalSocket = g_client_socket.exchange(-1);
    if (finalSocket != -1) {
        std::cout << "DEBUG: Cerrando socket " << finalSocket << " desde main." << std::endl;
        shutdown(finalSocket, SHUT_RDWR);
        close(finalSocket);
    }
     if (receiver_thread_obj.joinable()) {
        std::cout << "DEBUG: Esperando hilo de recepción al final..." << std::endl;
        receiver_thread_obj.join();
        std::cout << "DEBUG: Hilo de recepción terminado al final." << std::endl;
    }
    return 0;
}


// --- handleInput (sin cambios funcionales, pero verifica) ---
// (Esta es la versión de la respuesta anterior que incluye
// el regreso al menú desde LOBBY y GAMEOVER)
void handleInput(sf::Event& event, std::thread& receiver_thread_obj) {
    ClientState state_copy_event = g_client_state.load();
    int my_id = g_my_player_id.load();

    // --- Lógica del Menú Principal ---
    if (state_copy_event == MENU && (event.key.code == sf::Keyboard::Num1 || event.key.code == sf::Keyboard::Num2)) {
         if (event.key.code == sf::Keyboard::Num1) g_game_mode = SOLO;
         else if (event.key.code == sf::Keyboard::Num2) g_game_mode = DUO;

         if (g_client_socket == -1 && state_copy_event != CONNECTING) {
             std::cout << "DEBUG: Tecla de modo presionada, cambiando a CONNECTING..." << std::endl;
             g_client_state = CONNECTING;
             std::thread connect_thread([&]() {
                int tempSocket = -1;
                if (connect_to_server(tempSocket)) { // connect_to_server usará las globales
                    GameMode modo_seleccionado = g_game_mode.load();
                    std::string mode_msg = (modo_seleccionado == DUO) ? "DUO" : "SOLO";
                    if (send(tempSocket, mode_msg.c_str(), mode_msg.length(), 0) <= 0) {
                        std::cerr << "Error al enviar modo de juego." << std::endl;
                        close(tempSocket); g_client_state = MENU; return;
                    }
                    std::cout << "DEBUG: Modo " << mode_msg << " enviado." << std::endl;
                    g_client_socket = tempSocket;
                    try {
                        std::cout << "DEBUG: Conexión OK. Iniciando hilo receptor..." << std::endl;
                        if (g_receiver_should_run && receiver_thread_obj.joinable()) {
                            g_receiver_should_run=false; receiver_thread_obj.join();
                         }
                        g_receiver_should_run = true;
                        receiver_thread_obj = std::thread(receive_data_thread);
                    } catch (const std::system_error& e) {
                         std::cerr << "Error crítico al crear hilo de recepción: " << e.what() << std::endl;
                         close(g_client_socket.exchange(-1)); g_client_state = MENU;
                    }
                } else {
                    std::cerr << "DEBUG: Falló connect_to_server(). Volviendo a MENU." << std::endl;
                    g_client_socket = -1; g_client_state = MENU;
                }
            });
             if (connect_thread.joinable()) connect_thread.detach();
        }
    }
    // --- Lógica de Juego (PLAYING) ---
    else if (state_copy_event == PLAYING) {
        bool is_game_over_state = g_is_game_over.load();
        bool am_i_alive = false;
        {
            std::lock_guard<std::mutex> lock(state_mutex);
             for(const auto& snake : current_state.all_snakes) {
                if (snake.id == my_id) { am_i_alive = snake.isAlive; break; }
            }
        }

        // Volver al menú si estoy muerto (espectando) y presiono ENTER
        if (!am_i_alive && event.key.code == sf::Keyboard::Enter) {
             std::cout << "DEBUG: Enter presionado mientras espectador, volviendo al MENU..." << std::endl;
             {
                 std::lock_guard<std::mutex> lock(state_mutex);
                 g_client_state = MENU;
                 g_is_game_over = false;
                 current_state = GameState();
                 g_is_boosting = false;
                 g_receiver_should_run = false;
                 int sock_to_close = g_client_socket.exchange(-1);
                 if (sock_to_close != -1) {
                     shutdown(sock_to_close, SHUT_RDWR);
                     close(sock_to_close);
                     std::cout << "DEBUG: Socket " << sock_to_close << " cerrado desde handleInput (espectador)." << std::endl;
                 }
             }
             if (receiver_thread_obj.joinable()) receiver_thread_obj.join();
        }
        // Moverse solo si estoy vivo y la partida no ha terminado
        else if (am_i_alive && !is_game_over_state) {
            std::lock_guard<std::mutex> lock(state_mutex);
            std::string new_dir=current_direction;
            if(event.key.code==sf::Keyboard::Up)new_dir="UP";
            else if(event.key.code==sf::Keyboard::Down)new_dir="DOWN";
            else if(event.key.code==sf::Keyboard::Left)new_dir="LEFT";
            else if(event.key.code==sf::Keyboard::Right)new_dir="RIGHT";
            if(new_dir!=current_direction)current_direction=new_dir;
        }
    }
    // --- Lógica de Fin de Partida (GAMEOVER) ---
    else if (state_copy_event == GAMEOVER && event.key.code == sf::Keyboard::Enter) {
        std::cout << "DEBUG: Enter presionado en GAMEOVER, volviendo al MENU..." << std::endl;
        {
            std::lock_guard<std::mutex> lock(state_mutex);
            g_client_state = MENU;
            g_is_game_over = false;
            current_state = GameState();
            g_is_boosting = false;
            g_receiver_should_run = false;
            int sock_to_close = g_client_socket.exchange(-1);
            if (sock_to_close != -1) {
                 shutdown(sock_to_close, SHUT_RDWR);
                 close(sock_to_close);
                 std::cout << "DEBUG: Socket " << sock_to_close << " cerrado desde handleInput (gameover)." << std::endl;
            }
        }
        if (receiver_thread_obj.joinable()) receiver_thread_obj.join();
    }
    // --- Volver al Menú desde LOBBY ---
    else if (state_copy_event == LOBBY && event.key.code == sf::Keyboard::Enter) {
        std::cout << "DEBUG: Enter presionado en LOBBY, volviendo al MENU..." << std::endl;
         {
            std::lock_guard<std::mutex> lock(state_mutex);
            g_client_state = MENU;
            g_is_game_over = false;
            current_state = GameState();
            g_is_boosting = false;
            g_receiver_should_run = false;
            int sock_to_close = g_client_socket.exchange(-1);
            if (sock_to_close != -1) {
                 shutdown(sock_to_close, SHUT_RDWR);
                 close(sock_to_close);
                 std::cout << "DEBUG: Socket " << sock_to_close << " cerrado desde handleInput (lobby)." << std::endl;
            }
         }
         if (receiver_thread_obj.joinable()) receiver_thread_obj.join();
    }

    // Boost
    if (state_copy_event == PLAYING && event.key.code == sf::Keyboard::Space) {
        bool is_game_over_state = g_is_game_over.load();
        bool am_i_alive = false;
        {
            std::lock_guard<std::mutex> lock(state_mutex);
             for(const auto& snake : current_state.all_snakes) {
                if (snake.id == my_id) { am_i_alive = snake.isAlive; break; }
            }
        }
        if (am_i_alive && !is_game_over_state) {
            bool was_boosting = g_is_boosting.load();
            g_is_boosting = true;
            if(!was_boosting && g_client_socket != -1){
                send(g_client_socket.load(),"BOOST_ON",8,0);
            }
        }
    }
}


// --- handleKeyRelease (sin cambios funcionales) ---
void handleKeyRelease(sf::Event& event) {
    ClientState state_copy_event = g_client_state.load();
    if (state_copy_event == PLAYING && event.key.code == sf::Keyboard::Space) {
        if (g_is_boosting.load()) {
            g_is_boosting = false;
            if(g_client_socket != -1){
                send(g_client_socket.load(),"BOOST_OFF",9, MSG_NOSIGNAL);
            }
        }
    }
}


// --- sendDirectionIfNeeded (sin cambios funcionales) ---
void sendDirectionIfNeeded(sf::Clock& input_clock) {
    sf::Time input_interval = sf::milliseconds(100);
    int currentSocket = g_client_socket.load();
    ClientState currentState = g_client_state.load();
    bool isGameOver = g_is_game_over.load();
    int my_id = g_my_player_id.load();
    bool am_i_alive = false;
     {
        std::lock_guard<std::mutex> lock(state_mutex);
        for(const auto& snake : current_state.all_snakes) {
            if (snake.id == my_id) { am_i_alive = snake.isAlive; break; }
        }
     }

    if (currentState == PLAYING && am_i_alive && !isGameOver && currentSocket != -1 && input_clock.getElapsedTime() >= input_interval) {
         std::string dir_to_send;
         { std::lock_guard<std::mutex> lock(state_mutex); dir_to_send = current_direction; }
         if(send(currentSocket, dir_to_send.c_str(), dir_to_send.length(), MSG_NOSIGNAL) <= 0) {
             if (errno != EPIPE && errno != ECONNRESET && g_client_state.load() != MENU) {
                // perror("sendDirection error"); // Comentado para reducir spam
             }
             std::cout << "DEBUG: sendDirection error or disconnected." << std::endl;
             std::lock_guard<std::mutex> lock(state_mutex);
             if (g_client_state.load() != MENU) {
                 g_client_state = MENU;
                 g_lobby_message = "Desconectado.";
                 g_receiver_should_run = false;
                 int sock_to_close = g_client_socket.exchange(-1);
                 if (sock_to_close != -1) {
                    shutdown(sock_to_close, SHUT_RDWR);
                    close(sock_to_close);
                    std::cout << "DEBUG: Socket " << sock_to_close << " cerrado desde sendDirectionIfNeeded." << std::endl;
                 }
             }
         }
         input_clock.restart();
    }
}