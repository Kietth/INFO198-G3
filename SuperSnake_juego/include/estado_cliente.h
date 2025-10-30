#ifndef ESTADO_CLIENTE_H
#define ESTADO_CLIENTE_H

#include <atomic>
#include <mutex>
#include <string>
#include <vector>
#include <SFML/Graphics.hpp>
#include "common.h"

struct SnakeData { 
    int id; 
    bool isAlive; 
    std::vector<Posicion> body;
    std::string direction; 
    int equipo_id = -1; 
};

struct GameState { 
    std::vector<SnakeData> all_snakes; 
    std::vector<Posicion> food;
};

enum ClientState { MENU, CONNECTING, LOBBY, PLAYING, GAMEOVER };

enum GameMode { SOLO, DUO };

// Constantes de UI
extern const int TILE_SIZE;
extern const int TOP_MARGIN;
extern const int VIEWPORT_TILES_X; 
extern const int VIEWPORT_TILES_Y; 
extern const int WINDOW_WIDTH;
extern const int WINDOW_HEIGHT;

// Estado global
extern std::mutex state_mutex;
extern GameState current_state;
extern std::string current_direction;
extern std::atomic<ClientState> g_client_state;
extern std::atomic<GameMode> g_game_mode; 
extern std::string g_lobby_message;
extern std::atomic<bool> g_is_game_over;
extern std::atomic<int> g_my_player_id;
extern sf::Color g_my_color;
extern int g_current_grid_size; // <-- AÑADIDO
extern std::atomic<bool> g_is_boosting;
extern std::atomic<int> g_client_socket;
extern std::atomic<bool> g_receiver_should_run;

// Assets
extern std::vector<std::string> headFiles;
extern std::vector<sf::Color> playerColors;
extern std::map<int, sf::Texture> headTextures;
extern std::map<int, sf::Sprite> headSprites;
extern std::map<int, sf::Texture> bodyTextures;
extern std::map<int, sf::Sprite> bodySprites;
extern sf::Texture foodTexture;
extern sf::Sprite foodSprite;
extern sf::Texture titleTexture;
extern sf::Sprite titleSprite;
extern sf::Font font;

#endif // ESTADO_CLIENTE_H