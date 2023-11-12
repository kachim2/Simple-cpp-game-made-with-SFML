#include "game.h"

int main(int argc, char **argv){
    game the_game(argc, argv);
    int c = the_game.run();
    std::cerr<<  c;
    return c;
}