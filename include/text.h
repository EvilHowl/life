#pragma once

#include <cstdio>

#include <SDL_ttf.h>

#include "object.h"

enum class textType
{
    Solid,
    Blended
};

class Text : public Object
{
public:
    //Deallocate used memory
    ~Text();

    //Load font from specified path
    bool loadFromFont(const char* path, const int& size);

    //Set text
    void setText(const char* text, const SDL_Color& color = { 0, 0, 0, 255 }, textType type = textType::Blended);

    //Render texture
    void render();

private:
    //Loaded font pointer
    TTF_Font* m_font = nullptr;
};