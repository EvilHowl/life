#include "../include/game.h"

static bool m_isPlaying = false;
static constexpr Uint32 INTERVAL = 250;

static Uint32 callback(Uint32, void* ptr)
{
    Game* self = reinterpret_cast<Game*>(ptr);
    self->update();

    if (m_isPlaying)
        return INTERVAL;

    return 0;
}

bool Game::init()
{
    //Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0)
    {
        printf("SDL error: %s\n", SDL_GetError());
        return false;
    }

    //Initialize PNG subsystem
    int imgFlags = IMG_INIT_PNG;
    if (IMG_Init(imgFlags) != imgFlags)
    {
        printf("SDL_image error: %s\n", SDL_GetError());
        return false;
    }

    //Initialize TTF font subsystem
    if (TTF_Init() == -1)
    {
        printf("SDL_ttf error: %s\n", SDL_GetError());
        return false;
    }

    //Create window
    m_window = SDL_CreateWindow(
        m_config["title"].c_str(),
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        std::stoi(m_config["width"]),
        std::stoi(m_config["height"]),
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    if (m_window == nullptr)
    {
        printf("SDL error: %s\n", SDL_GetError());
        return false;
    }

    //Initialize renderer
    m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (m_renderer == nullptr)
    {
        printf("SDL error: %s\n", SDL_GetError());
        return false;
    }

    SDL_GetWindowSize(m_window, &m_screenWidth, &m_screenHeight);

    SDL_SetWindowMinimumSize(m_window, 640, 480);

    grid.alloc((m_screenHeight - m_hud_actual_height) / grid_cell_size, m_screenWidth / grid_cell_size);

    if (!loadResources())
        return false;

    updateHUDPosition();

    return true;
}

Game::~Game()
{
    IMG_Quit();
    SDL_Quit();
}

bool Game::Loop()
{
    SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);

    std::chrono::duration<int64_t, std::nano> framerate {};

    while (m_isRunning)
    {
        /* Clock */
        auto start = std::chrono::high_resolution_clock::now();

        /* Game logic */
        handleEvent();
        getPopulation();

        /* Draw background */
        SDL_SetRenderDrawColor(m_renderer, m_grid_background_color.r, m_grid_background_color.g, m_grid_background_color.b, m_grid_background_color.a);
        SDL_RenderClear(m_renderer);

        /* HUD: Draw status bar */
        SDL_SetRenderDrawColor(m_renderer, 92, 92, 92, 255);
        SDL_Rect hud = { 0, 0, m_screenWidth, m_hud_actual_height };
        SDL_RenderFillRect(m_renderer, &hud);

        /* HUD: Draw next generation button */
        m_advanceButton.render();

        /* HUD: Draw play or pause button */
        if (m_isPlaying)
            m_pauseButton.render();
        else
            m_playButton.render();

        /* HUD: Draw grid button */
        m_gridButton.render();

        /* HUD: Draw reset button */
        m_resetButton.render();

        /* HUD: Draw information text */
        std::stringstream ss;
        ss << "Generation: " << m_generation << " Population: " << m_population;

#ifdef DEBUG
        ss << " fps: " << 1e9 / framerate.count() << " cols: " << grid.cols() << " rows: " << grid.rows();
#endif

        m_info.setText(ss.str().c_str(), m_text_color, textType::Blended);

        /* Draw tiles */
        for (int i = 0; i < grid.rows(); ++i)
            for (int j = 0; j < grid.cols(); ++j)
            {
                if (grid(i, j))
                    SDL_SetRenderDrawColor(m_renderer, m_grid_cursor_color.r, m_grid_cursor_color.g, m_grid_cursor_color.b, m_grid_cursor_color.a);
                else
                    SDL_SetRenderDrawColor(m_renderer, m_grid_background_color.r, m_grid_background_color.g, m_grid_background_color.b, m_grid_background_color.a);

                SDL_Rect r = { j * grid_cell_size, m_hud_actual_height + i * grid_cell_size, grid_cell_size, grid_cell_size };
                SDL_RenderFillRect(m_renderer, &r);
            }

        /* Draw grid only if it's active */
        if (m_isLineActive)
        {
            SDL_SetRenderDrawColor(m_renderer, m_grid_line_color.r, m_grid_line_color.g, m_grid_line_color.b, m_grid_line_color.a);
            for (int i = 0; i <= grid.cols(); ++i)
                SDL_RenderDrawLine(m_renderer, grid_cell_size * i, m_hud_actual_height, grid_cell_size * i, grid.rows() * grid_cell_size + m_hud_actual_height);
            for (int i = 0; i <= grid.rows(); ++i)
                SDL_RenderDrawLine(m_renderer, 0, grid_cell_size * i + m_hud_actual_height, grid.cols() * grid_cell_size, grid_cell_size * i + m_hud_actual_height);
        }

        /* Draw mouse ghost cursor only if it's available and it's not positioned in the HUD */
        if (m_isMouseActive && m_isMouseHovering && grid_cursor.y >= m_hud_actual_height)
        {
            SDL_SetRenderDrawColor(m_renderer, m_grid_cursor_ghost_color.r,
                m_grid_cursor_ghost_color.g,
                m_grid_cursor_ghost_color.b,
                m_grid_cursor_ghost_color.a);
            SDL_RenderFillRect(m_renderer, &grid_cursor);
        }

        /* Render everything */
        m_info.render();
        SDL_RenderPresent(m_renderer);

        /* Clock */
        auto end = std::chrono::high_resolution_clock::now();
        framerate = end - start;
    }
    return true;
}

void Game::handleEvent()
{
    while (SDL_PollEvent(&m_event))
    {
        switch (m_event.type)
        {
        case SDL_KEYDOWN:
            switch (m_event.key.keysym.sym)
            {
            case SDLK_s:
#ifdef DEBUG
                raise(SIGINT);
#endif
                break;
            case SDLK_r:
                reset();
                break;
            case SDLK_t:
                update(true);
                break;
            case SDLK_q:
                if (m_isLineActive)
                    m_isLineActive = false;
                else
                    m_isLineActive = true;
                break;
            case SDLK_F11:
                SDL_SetWindowFullscreen(m_window, SDL_GetWindowFlags(m_window) >= SDL_WINDOW_FULLSCREEN_DESKTOP ? 0 : SDL_WINDOW_FULLSCREEN_DESKTOP);
                break;
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            //Click on tile
            if (m_event.motion.x <= (grid.cols() * grid_cell_size) && m_event.motion.y <= (grid.rows() * grid_cell_size + m_hud_actual_height) && m_event.motion.y > m_hud_actual_height)
                (grid(((m_event.motion.y - m_hud_actual_height) / grid_cell_size), (m_event.motion.x / grid_cell_size))) ? grid(((m_event.motion.y - m_hud_actual_height) / grid_cell_size), (m_event.motion.x / grid_cell_size)) = false : grid(((m_event.motion.y - m_hud_actual_height) / grid_cell_size), (m_event.motion.x / grid_cell_size)) = true;

            //Click on next generation button
            if (m_event.motion.x >= m_advanceButton.getPosition().x && m_event.motion.x <= (m_advanceButton.getPosition().x + m_advanceButton.getPosition().w) && m_event.motion.y <= m_hud_actual_height)
                update(true);

            //Click on play/pause button
            if (m_event.motion.x >= (m_playButton.getPosition().x) && m_event.motion.x <= (m_playButton.getPosition().x + m_playButton.getPosition().w) && m_event.motion.y <= m_hud_actual_height)
            {
                if (!m_isPlaying)
                {
                    m_isPlaying = true;
                    m_timer = SDL_AddTimer(INTERVAL, callback, this);
                }
                else
                {
                    m_isPlaying = false;
                    SDL_RemoveTimer(m_timer);
                }
            }

            //Click on grid button
            if (m_event.motion.x >= (m_gridButton.getPosition().x) && m_event.motion.x <= (m_gridButton.getPosition().x + m_gridButton.getPosition().w) && m_event.motion.y <= m_hud_actual_height)
            {
                if (m_isLineActive)
                    m_isLineActive = false;
                else
                    m_isLineActive = true;
            }

            //Click on reset button
            if (m_event.motion.x >= m_resetButton.getPosition().x && m_event.motion.x <= (m_resetButton.getPosition().x + m_resetButton.getPosition().w) && m_event.motion.y <= m_hud_actual_height)
                reset();

            break;
        case SDL_MOUSEMOTION:
            grid_cursor.x = (m_event.motion.x / grid_cell_size) * grid_cell_size;
            grid_cursor.y = (m_event.motion.y / grid_cell_size) * grid_cell_size;

            if (!m_isMouseActive)
                m_isMouseActive = true;
            break;
        case SDL_WINDOWEVENT:
            if (m_event.window.event == SDL_WINDOWEVENT_ENTER && !m_isMouseHovering)
                m_isMouseHovering = true;
            else if (m_event.window.event == SDL_WINDOWEVENT_LEAVE && m_isMouseHovering)
                m_isMouseHovering = false;

            switch (m_event.window.event)
            {
            case SDL_WINDOWEVENT_SIZE_CHANGED:
                m_screenWidth = SDL_GetWindowSurface(m_window)->w;
                m_screenHeight = SDL_GetWindowSurface(m_window)->h;

                updateHUDPosition();
                grid.alloc((m_screenHeight - m_hud_actual_height) / grid_cell_size, m_screenWidth / grid_cell_size);
                break;
            }
            break;
        case SDL_QUIT:
            m_isRunning = false;
            break;
        }
    }
}

bool Game::loadResources()
{
    m_info.setRenderer(m_renderer);
    if (!m_info.loadFromFont("font.ttf", 14))
        return false;

    m_advanceButton.setRenderer(m_renderer);
    if (!m_advanceButton.loadFromImage("advance.png"))
        return false;

    m_playButton.setRenderer(m_renderer);
    if (!m_playButton.loadFromImage("play.png"))
        return false;

    m_pauseButton.setRenderer(m_renderer);
    if (!m_pauseButton.loadFromImage("pause.png"))
        return false;

    m_gridButton.setRenderer(m_renderer);
    if (!m_gridButton.loadFromImage("grid.png"))
        return false;

    m_resetButton.setRenderer(m_renderer);
    if (!m_resetButton.loadFromImage("reset.png"))
        return false;

    return true;
}

void Game::update(bool once)
{
    if (!m_isPlaying && !once)
        return;

    Grid next;
    next.alloc(grid.rows(), grid.cols());

    for (int i = 0; i < grid.rows(); ++i)
        for (int j = 0; j < grid.cols(); ++j)
        {
            int nei = getNeighbour(i, j);
            if (grid(i, j) && (nei < 2 || nei > 3))
                next(i, j) = false;
            else if ((!grid(i, j) && nei == 3) || (grid(i, j) && (nei == 2 || nei == 3)))
                next(i, j) = true;
        }

    grid = std::move(next);

    m_generation++;
}

void Game::reset()
{
    for (int i = 0; i < grid.rows(); ++i)
        for (int j = 0; j < grid.cols(); ++j)
            grid(i, j) = false;

    m_generation = 0;
}

[[nodiscard]] int Game::getNeighbour(int i, int j) const noexcept
{
    int num = 0;

    if ((i - 1) >= 0 && grid(i - 1, j))
        num++;

    if ((i + 1) <= grid.rows() - 1 && grid(i + 1, j))
        num++;

    if ((j + 1) <= grid.cols() - 1 && grid(i, j + 1))
        num++;

    if ((j - 1) >= 0 && grid(i, j - 1))
        num++;

    if ((i + 1) <= grid.rows() - 1 && (j + 1) <= grid.cols() - 1 && grid(i + 1, j + 1))
        num++;

    if ((i - 1) >= 0 && (j - 1) >= 0 && grid(i - 1, j - 1))
        num++;

    if ((i + 1) <= grid.rows() - 1 && (j - 1) >= 0 && grid(i + 1, j - 1))
        num++;

    if ((i - 1) >= 0 && (j + 1) <= grid.cols() - 1 && grid(i - 1, j + 1))
        num++;

    return num;
}

void Game::getPopulation()
{
    int n = 0;

    for (int i = 0; i < grid.rows(); ++i)
        for (int j = 0; j < grid.cols(); ++j)
            if (grid(i, j))
                n++;

    m_population = n;
}

void Game::updateHUDPosition()
{
    m_info.setPosition(4, 2);
    m_advanceButton.setPosition(m_screenWidth - m_advanceButton.getPosition().w - 6, 4);
    m_playButton.setPosition((m_advanceButton.getPosition().x - m_playButton.getPosition().w - 6), 4);
    m_pauseButton.setPosition(m_playButton.getPosition().x, m_playButton.getPosition().y);
    m_gridButton.setPosition((m_playButton.getPosition().x - m_gridButton.getPosition().w - 6), 4);
    m_resetButton.setPosition((m_gridButton.getPosition().x - m_resetButton.getPosition().w - 6), 4);
}