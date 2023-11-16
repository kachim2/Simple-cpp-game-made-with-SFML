//
// Created by mi on 12.11.23.
//

#ifndef CPP_GAME_AIGAME_H
#define CPP_GAME_AIGAME_H
#include "game.h"

class AIgame: public game{
public:
    int renderpx();
    int xv;
    int yv;
    std::vector<float> pixels;

    AIgame(int argc, char **argv, int xv, int yv);
};


#endif //CPP_GAME_AIGAME_H
