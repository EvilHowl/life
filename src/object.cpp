#include "../include/object.h"

void Object::setPosition(const int& x, const int& y) noexcept
{
    m_position.x = x;
    m_position.y = y;
}

void Object::setSize(const int& w, const int& h) noexcept
{
    m_position.w = w;
    m_position.h = h;
}

void Object::setAlpha(Uint8 a) noexcept
{
    SDL_SetTextureAlphaMod(m_texture, a);
}