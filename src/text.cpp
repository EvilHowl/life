#include "../include/text.h"

Text::~Text()
{
    //Free m_texture if it exists
    if (m_texture != nullptr)
    {
        SDL_DestroyTexture(m_texture);
        m_texture = nullptr;

        TTF_CloseFont(m_font);
        m_font = nullptr;

        m_position = { 0, 0, 0, 0 };
    }
}

bool Text::loadFromFont(const char* path, const int& size)
{
    m_font = TTF_OpenFont(path, size);
    if (m_font == nullptr)
    {
        printf("SDL error: %s\n", SDL_GetError());
        return false;
    }

    return true;
}

void Text::setText(const char* text, const SDL_Color& color, textType type)
{
    TTF_SizeUTF8(m_font, text, &m_position.w, &m_position.h);
    SDL_Surface* surface = nullptr;

    switch (type)
    {
    case textType::Solid:
        surface = TTF_RenderUTF8_Solid(m_font, text, color);
        break;
    case textType::Blended:
        surface = TTF_RenderUTF8_Blended(m_font, text, color);
        break;
    }

    SDL_DestroyTexture(m_texture);

    m_texture = SDL_CreateTextureFromSurface(m_renderer, surface);

    //Destroy the surface once we've converted it into a texture
    SDL_FreeSurface(surface);
}

void Text::render()
{
    SDL_RenderCopy(m_renderer, m_texture, nullptr, &m_position);
}