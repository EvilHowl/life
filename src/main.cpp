#include "../include/game.h"

int main(int, char**)
{
    Game game;

    if (!game.init())
        return -1;

    game.Loop();

    return 0;
}