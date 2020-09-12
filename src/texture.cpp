#include "../include/texture.h"

Texture::~Texture()
{
    //Free texture if it exists
    if (m_texture != nullptr)
    {
        SDL_DestroyTexture(m_texture);
        m_texture = nullptr;
        m_position = { 0, 0, 0, 0 };
    }
}

bool Texture::loadFromImage(const char* path)
{
    //Load image at specified path
    SDL_Surface* loadedSurface = IMG_Load(path);
    if (loadedSurface == nullptr)
    {
        printf("SDL error: %s\n", SDL_GetError());
        return false;
    }

    //Color key image
    SDL_SetColorKey(loadedSurface, true, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

    //Create texture from surface pixels
    m_texture = SDL_CreateTextureFromSurface(m_renderer, loadedSurface);
    if (m_texture == nullptr)
    {
        printf("SDL error: %s\n", SDL_GetError());
        return false;
    }

    //Get image dimensions
    m_position.w = loadedSurface->w;
    m_position.h = loadedSurface->h;

    //Get rid of old loaded surface
    SDL_FreeSurface(loadedSurface);

    return true;
}

void Texture::setColor(Uint8 r, Uint8 g, Uint8 b)
{
    SDL_SetTextureColorMod(m_texture, r, g, b);
}

void Texture::setBlendMode(SDL_BlendMode blend)
{
    SDL_SetTextureBlendMode(m_texture, blend);
}

void Texture::render()
{
    SDL_RenderCopy(m_renderer, m_texture, nullptr, &m_position);
}