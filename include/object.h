#pragma once

#include <SDL.h>
#include <SDL_render.h>

class Object
{
public:
    void setRenderer(SDL_Renderer* r) noexcept { m_renderer = r; };

    [[nodiscard]] constexpr SDL_Rect getPosition() const { return m_position; }

    void setPosition(const int& x, const int& y) noexcept;

    void setSize(const int& w, const int& h) noexcept;

    //Set alpha modulation
    void setAlpha(Uint8 a) noexcept;

protected:
    SDL_Renderer* m_renderer = nullptr;

    //Texture pointer
    SDL_Texture* m_texture = nullptr;

    //x, y, w, h
    SDL_Rect m_position { 0, 0, 0, 0 };
};