#pragma once

#include <cstdio>

#include <SDL_image.h>

#include "object.h"

class Texture : public Object
{
public:
    //Deallocate used memory
    ~Texture();

    //Load image from specified path
    bool loadFromImage(const char* path);

    //Set color modulation
    void setColor(Uint8 r, Uint8 g, Uint8 b);

    //Set blending
    void setBlendMode(SDL_BlendMode blend);

    //Render texture
    void render();
};