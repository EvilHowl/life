#pragma once

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <map>
#include <sstream>
#include <stdexcept>
#include <cstdio>
#include <string>

#ifdef DEBUG
#    include <csignal>
#endif

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_render.h>

#include "grid.h"
#include "text.h"
#include "texture.h"

class Game
{
public:
    Game() = default;

    //Initializes SDL and every subsystem and creates both window and renderer
    bool init();

    //Shuts down SDL
    ~Game();

    //Runs the game loop
    bool Loop();

    void update(bool once = false);

private:
    [[nodiscard]] bool loadResources();

    void handleEvent();

    void reset();

    void getPopulation();

    [[nodiscard]] int getNeighbour(int i, int j) const noexcept;

    void updateHUDPosition();

    int m_generation = 0;
    int m_population = 0;

    int m_screenWidth = 0;
    int m_screenHeight = 0;

    bool m_isRunning = true;

    bool m_isMouseActive = false;
    bool m_isMouseHovering = false;
    bool m_isLineActive = false;

    //Map that holds all the config
    std::map<std::string, std::string> m_config = {
        { "width", "1280" },
        { "height", "720" },
        { "title", "Conway's Game of Life" }
    };

    Grid grid;
    SDL_Rect grid_cursor = { 0, 0, grid_cell_size, grid_cell_size };

    SDL_Window* m_window = nullptr;
    SDL_Renderer* m_renderer = nullptr;

    SDL_Event m_event {};

    //Color palettes
    SDL_Color m_grid_background_color = { 22, 22, 22, 255 };
    SDL_Color m_grid_line_color = { 44, 44, 44, 255 };
    SDL_Color m_grid_cursor_ghost_color = { 255, 255, 255, 100 };
    SDL_Color m_grid_cursor_color = { 255, 255, 255, 255 };

    SDL_Color m_text_color = { 0, 0, 0, 255 };

    static constexpr int grid_cell_size = 12;

    //Height of the hud
    static constexpr int m_hud_actual_height = 24;

    Text m_info;

    Texture m_advanceButton;
    Texture m_playButton;
    Texture m_pauseButton;
    Texture m_gridButton;
    Texture m_resetButton;

    SDL_TimerID m_timer{};
};