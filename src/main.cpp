#include "AIgame.h"
#include "SFML/Graphics.hpp"
int main(int argc, char **argv){
    AIgame the_game(argc, argv, 144, 72);
    int frame = 0;
    auto start = std::chrono::system_clock::now();
    while (true){
        the_game.step();
        the_game.renderpx();
        sf::Uint8* pixels = new sf::Uint8[144*72*3];

        FILE *fp = fopen((std::to_string(frame) + "c.ppm").c_str(), "wb"); /* b - binary mode */
        fprintf(fp, "P6\n%d %d\n255\n", 144, 72);
        for(auto x : the_game.pixels){
            static unsigned char color[3];
            x*=255;

            color[0] = x;  /* red */
            color[1] = x;  /* green */
            color[2] = x;  /* blue */

            fwrite(color, 1, 3, fp);
        }
        fclose(fp);
        frame++;
        auto now  = std::chrono::system_clock::now();
        std::cerr << (double)frame/(double)std::chrono::duration_cast<std::chrono::seconds>(now - start).count() << '\n';
    }

    return 0;
}
