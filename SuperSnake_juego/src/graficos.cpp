#include "graficos.h"
#include "estado_cliente.h" // Incluimos los globales
#include <iostream>
#include <cmath>
#include <map>
#include <set> // Para equipos únicos

using namespace std;

// --- Definición de globales de assets ---
vector<string> headFiles = {"green", "blue", "pink", "skyblue", "red", "yellow"};
vector<sf::Color> playerColors = {sf::Color::Green, sf::Color::Blue, sf::Color(255, 105, 180), sf::Color::Cyan, sf::Color::Red, sf::Color::Yellow };
map<int, sf::Texture> headTextures;
map<int, sf::Sprite> headSprites;
map<int, sf::Texture> bodyTextures;
map<int, sf::Sprite> bodySprites;
sf::Texture foodTexture;
sf::Sprite foodSprite;
sf::Texture titleTexture;
sf::Sprite titleSprite;
sf::Font font;

// --- Definición de constantes de UI ---
const int TILE_SIZE = 32;
const int TOP_MARGIN = 40;
const int VIEWPORT_TILES_X = 20;
const int VIEWPORT_TILES_Y = 20;
const int WINDOW_WIDTH = VIEWPORT_TILES_X * TILE_SIZE;
const int WINDOW_HEIGHT = VIEWPORT_TILES_Y * TILE_SIZE + TOP_MARGIN;

bool cargarAssets() {
    if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
        if (!font.loadFromFile("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf")) {
            cerr << "Error: Fuente no encontrada." << endl;
            return false;
        }
    }
    if (!titleTexture.loadFromFile("assets/title.png")) { cerr << "Error assets/title.png\n"; return false; }
    titleSprite.setTexture(titleTexture);
    titleSprite.setOrigin(titleTexture.getSize().x/2.f, titleTexture.getSize().y/2.f);
    titleSprite.setScale(3.f, 3.f);

    if (!foodTexture.loadFromFile("assets/food.png")) { cerr << "Error assets/food.png\n"; return false; }
    foodSprite.setTexture(foodTexture);

    for(size_t i = 0; i < headFiles.size(); ++i) {
        if (!headTextures[i].loadFromFile("assets/heads/" + headFiles[i] + ".png")) { cerr << "Error assets/heads/\n"; return false; }
        headSprites[i].setTexture(headTextures[i]);
        headSprites[i].setOrigin(headTextures[i].getSize().x/2.f, headTextures[i].getSize().y/2.f);

        if (!bodyTextures[i].loadFromFile("assets/body/" + headFiles[i] + ".png")) { cerr << "Error assets/body/\n"; return false; }
        bodySprites[i].setTexture(bodyTextures[i]);
    }
    return true;
}

Posicion getCameraFocusPos() {
    int my_id = g_my_player_id.load();
    bool is_me_alive = false;
    int my_team_id = -1;
    Posicion my_pos = {VIEWPORT_TILES_X / 2, VIEWPORT_TILES_Y / 2};

    lock_guard<mutex> lock(state_mutex);

    for (const auto& snake : current_state.all_snakes) {
        if (snake.id == my_id) {
            my_team_id = snake.equipo_id;
            is_me_alive = snake.isAlive;
            if (!snake.body.empty()) {
                my_pos = snake.body[0];
            }
            break;
        }
    }

    if (is_me_alive) {
        return my_pos;
    }

    if (g_game_mode.load() == DUO && my_team_id != -1) {
        for (const auto& snake : current_state.all_snakes) {
            if (snake.id != my_id && snake.equipo_id == my_team_id && snake.isAlive && !snake.body.empty()) {
                return snake.body[0];
            }
        }
    }
    return my_pos;
}


void drawGrid(sf::RenderWindow& window) {
    Posicion camera_pos = getCameraFocusPos();
    float camera_grid_x = camera_pos.x - (VIEWPORT_TILES_X / 2.0f);
    float camera_grid_y = camera_pos.y - (VIEWPORT_TILES_Y / 2.0f);
    sf::Color gridColor(30, 30, 30);
    float gridTop = (float)TOP_MARGIN;
    float gridWidth = (float)WINDOW_WIDTH;
    float gridBottom = (float)WINDOW_HEIGHT;
    float offset_x_px = -(fmod(camera_grid_x, 1.0f)) * TILE_SIZE;
    float offset_y_px = -(fmod(camera_grid_y, 1.0f)) * TILE_SIZE;
    for (int i = 0; i <= VIEWPORT_TILES_X + 1; ++i) {
        float xPos = (float)(i * TILE_SIZE) + offset_x_px;
        if (xPos < 0 || xPos > gridWidth) continue;
        sf::Vertex v[] = {sf::Vertex(sf::Vector2f(xPos, gridTop), gridColor), sf::Vertex(sf::Vector2f(xPos, gridBottom), gridColor)};
        window.draw(v, 2, sf::Lines);
    }
    for (int i = 0; i <= VIEWPORT_TILES_Y + 1; ++i) {
        float yPos = (float)(i * TILE_SIZE) + gridTop + offset_y_px;
        if (yPos < gridTop || yPos > gridBottom + 1.0f) continue;
        sf::Vertex h[] = {sf::Vertex(sf::Vector2f(0.f, yPos), gridColor), sf::Vertex(sf::Vector2f(gridWidth, yPos), gridColor)};
        window.draw(h, 2, sf::Lines);
    }
}


void drawGame(sf::RenderWindow& window) {
    GameState state_copy_local;
    int my_id = g_my_player_id.load();
    Posicion camera_pos = getCameraFocusPos();
    bool am_i_alive = false;

    {
        lock_guard<mutex> lock(state_mutex);
        state_copy_local = current_state;
        for(const auto& snake : state_copy_local.all_snakes) {
            if (snake.id == my_id) {
                am_i_alive = snake.isAlive;
                break;
            }
        }
    }

    float camera_grid_x = camera_pos.x - (VIEWPORT_TILES_X / 2.0f);
    float camera_grid_y = camera_pos.y - (VIEWPORT_TILES_Y / 2.0f);

    // Dibujar Comida
    for (const auto& pos : state_copy_local.food) {
        float screen_x = (pos.x - camera_grid_x) * TILE_SIZE;
        float screen_y = (pos.y - camera_grid_y) * TILE_SIZE + TOP_MARGIN;
        if (screen_x >= -TILE_SIZE && screen_x <= WINDOW_WIDTH && screen_y >= TOP_MARGIN-TILE_SIZE && screen_y <= WINDOW_HEIGHT) {
            foodSprite.setPosition(screen_x, screen_y);
            window.draw(foodSprite);
        }
    }

    // Dibujar Serpientes
    for (const auto& snake : state_copy_local.all_snakes) {
        if (snake.id == my_id) {
             lock_guard<mutex> lock(state_mutex);
             if (!am_i_alive) {
                 g_my_color = sf::Color(100,100,100,150);
             } else {
                 int my_color_index = snake.id % headFiles.size();
                 if (g_game_mode.load() == DUO) {
                     my_color_index = snake.equipo_id % headFiles.size();
                 }
                 g_my_color = playerColors[my_color_index];
             }
        }
        if (!snake.isAlive) continue;

        int color_index = snake.id % headFiles.size();
        if (g_game_mode.load() == DUO) {
            color_index = snake.equipo_id % headFiles.size();
        }

        // Cuerpo
        if (bodySprites.count(color_index)) {
            sf::Sprite& bodySprite = bodySprites[color_index];
            for (size_t i = 1; i < snake.body.size(); ++i) {
                const auto& pos=snake.body[i];
                float screen_x = (pos.x - camera_grid_x) * TILE_SIZE;
                float screen_y = (pos.y - camera_grid_y) * TILE_SIZE + TOP_MARGIN;

                if (screen_x >= -TILE_SIZE && screen_x <= WINDOW_WIDTH && screen_y >= TOP_MARGIN-TILE_SIZE && screen_y <= WINDOW_HEIGHT) {
                    bodySprite.setPosition(screen_x, screen_y);
                    bodySprite.setColor(sf::Color::White);
                    window.draw(bodySprite);
                }
            }
        }

        // Cabeza
        if (!snake.body.empty() && headSprites.count(color_index)) {
            sf::Sprite& headSprite = headSprites[color_index];
            const auto& headPos=snake.body[0];
            float screen_x = (headPos.x - camera_grid_x) * TILE_SIZE;
            float screen_y = (headPos.y - camera_grid_y) * TILE_SIZE + TOP_MARGIN;

            if (screen_x >= -TILE_SIZE && screen_x <= WINDOW_WIDTH && screen_y >= TOP_MARGIN-TILE_SIZE && screen_y <= WINDOW_HEIGHT) {
                headSprite.setPosition(screen_x + TILE_SIZE / 2.f, screen_y + TILE_SIZE / 2.f);
                if(snake.direction=="UP")headSprite.setRotation(180.f); else if(snake.direction=="DOWN")headSprite.setRotation(0.f); else if(snake.direction=="LEFT")headSprite.setRotation(90.f); else if(snake.direction=="RIGHT")headSprite.setRotation(270.f);
                headSprite.setColor(sf::Color::White);
                window.draw(headSprite);
            }
        }
    }

    // Mostrar mensaje de derrota si estoy muerto pero la partida *no* ha terminado
    if (!am_i_alive && g_client_state.load() == PLAYING && !g_is_game_over.load()) {
        drawTextCentered(window, "DERROTA", -50, 40, sf::Color::Red);
        drawTextCentered(window, "Presione ENTER para ir al menu principal", 50, 24);
    }
}


void drawText(sf::RenderWindow& window, const string& str, float x, float y, int size, sf::Color color) {
    sf::Text text; text.setFont(font); text.setString(str); text.setCharacterSize(size); text.setFillColor(color); text.setPosition(x,y); window.draw(text);
}

void drawTextCentered(sf::RenderWindow& window, const string& str, int y_offset, int size, sf::Color color) {
    sf::Text text; text.setFont(font); text.setString(str); text.setCharacterSize(size); text.setFillColor(color); sf::FloatRect r=text.getLocalBounds(); text.setOrigin(r.left+r.width/2.f,r.top+r.height/2.f); text.setPosition((float)(WINDOW_WIDTH/2.f), (float)(TOP_MARGIN+(WINDOW_HEIGHT-TOP_MARGIN)/2.f+y_offset)); window.draw(text);
}

void dibujarUI(sf::RenderWindow& window) {
    sf::RectangleShape topBar(sf::Vector2f((float)WINDOW_WIDTH, (float)TOP_MARGIN));
    topBar.setFillColor(sf::Color(40, 40, 40));
    topBar.setPosition(0.f, 0.f);
    window.draw(topBar);

    int my_id = g_my_player_id.load();
    bool is_game_over_state = g_is_game_over.load();
    float ui_y = TOP_MARGIN / 2.f;
    float icon_scale = 0.8f;
    float icon_width = (TILE_SIZE * icon_scale);
    float icon_spacing = 5.f;

    if (g_client_state.load() != MENU && g_client_state.load() != CONNECTING && my_id != -1) {

        int my_team_id = -1;
        int my_color_index = my_id % headFiles.size();
        bool am_i_alive = false;
        set<int> equipos_vivos_ids;

        {
            lock_guard<mutex> lock(state_mutex);
            for(const auto& snake : current_state.all_snakes) {
                if (snake.id == my_id) {
                    my_team_id = snake.equipo_id;
                    am_i_alive = snake.isAlive;
                }
                if (snake.isAlive) {
                    equipos_vivos_ids.insert(snake.equipo_id);
                }
            }
        }

        if (g_game_mode.load() == DUO && my_team_id != -1) {
            my_color_index = my_team_id % headFiles.size();
        }

        // 1. Dibujar Icono (izquierda)
        if(headSprites.count(my_color_index)) {
            sf::Sprite& h = headSprites[my_color_index];
            h.setScale(icon_scale,icon_scale);
            h.setPosition(15.f + icon_width/2.f, ui_y);
            h.setRotation(0);
            sf::Color c = sf::Color::White;
            if ((!am_i_alive && g_game_mode.load() == SOLO) ||
                (g_game_mode.load() == DUO && my_team_id != -1 && equipos_vivos_ids.find(my_team_id) == equipos_vivos_ids.end()) ||
                 is_game_over_state)
            {
                c.a = 150;
            }
            h.setColor(c);
            window.draw(h);
            h.setScale(1.f,1.f);
        }

        // 2. Dibujar Iconos de Equipos Vivos (centro)
        if (!equipos_vivos_ids.empty() && equipos_vivos_ids.size() > 1) {
            float total_list_width = equipos_vivos_ids.size() * (icon_width + icon_spacing) - icon_spacing;
            float current_x = (WINDOW_WIDTH / 2.f) - (total_list_width / 2.f);

            for (int team_id : equipos_vivos_ids) {
                int team_color_index = team_id % headFiles.size();
                if (headSprites.count(team_color_index)) {
                    sf::Sprite& h = headSprites[team_color_index];
                    h.setScale(icon_scale, icon_scale);
                    h.setPosition(current_x + icon_width/2.f, ui_y);
                    h.setRotation(0);
                    h.setColor(sf::Color::White);
                    window.draw(h);
                    h.setScale(1.f, 1.f);
                    current_x += icon_width + icon_spacing;
                }
            }
        }
    }
}


// --- FUNCIÓN TEXTO DE LOBBY---
void dibujarEstado(sf::RenderWindow& window) {
    ClientState state_copy_draw = g_client_state.load();
    string lobby_msg_copy_draw;
    {
        lock_guard<mutex> lock(state_mutex);
        lobby_msg_copy_draw = g_lobby_message;
    }

    switch (state_copy_draw) {
        case MENU:
            titleSprite.setPosition(WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f - 70.f);
            window.draw(titleSprite);
            drawTextCentered(window, "Seleccione 1 o 2:", 70, 24);
            drawTextCentered(window, "1. SOLO", 110, 24);
            drawTextCentered(window, "2. DUO", 150, 24);
            break;
        case CONNECTING:
            drawTextCentered(window, "Conectando...", 0, 30);
            break;
        case LOBBY: // Incluye COUNTDOWN visualmente
            drawGrid(window);
            drawTextCentered(window, lobby_msg_copy_draw, -30, 30);
            drawTextCentered(window, "Presione ENTER para volver al menu", 30, 24);
            break;
        case PLAYING:
            drawGrid(window);
            drawGame(window);
            break;
        case GAMEOVER: // La partida HA TERMINADO para todos
            drawGrid(window);
            drawGame(window);

            sf::Color msg_color = sf::Color::Red;
            if (lobby_msg_copy_draw == "VICTORIA" || lobby_msg_copy_draw == "VICTORIA DE EQUIPO") {
                msg_color = sf::Color::Green;
            }
            drawTextCentered(window, lobby_msg_copy_draw, -50, 40, msg_color);
            drawTextCentered(window, "Presione ENTER para ir al menu principal", 50, 24);
            break;
    }
}