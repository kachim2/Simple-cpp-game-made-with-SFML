//
// Created by mi on 10.11.23.
//

#ifndef CPP_GAME_GAME_H
#define CPP_GAME_GAME_H
#include <SFML/Graphics.hpp> // gra używa biblioteki SFML ze strony https://www.sfml-dev.org/
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Network.hpp>
#include <SFML/Audio.hpp>
#include <mutex>
#include <thread>
#include <fstream>
#include <iostream>
#include <vector>
#include "SHA256.h"
using namespace sf;
int run(int argc, char **argv);
class game;
class Obstacle
{

public:
    Sprite shape;
    Vector2f Size, x_y;
    inline static Texture white;
    Obstacle(Texture txt = white, Vector2f Start_Size = Vector2f(0, 0), Vector2f Start_x_y = Vector2f(0, 0)) ;
    void update();
    void draw(RenderWindow &rwindow);



};
class Platform
{
public:
    Obstacle end;
    Obstacle middlee;
    Vector2f Size;
    Vector2f Position;
    RectangleShape shape{Size};
    Texture endplat;
    Texture middle;
    Platform();
    void draw(RenderWindow &rwindow);
};
class game {
public:
    RenderWindow window{VideoMode({1280, 720}), "C++ Game"};
    game(int argc, char **argv);
    ~game();
    int run();
    int step(std::array<bool, 3> in, float dt);
    int render();
    static Texture white;
    bool graphics =1;
    Sprite player{player_txt1, IntRect({0, 0}, {50, 50})};
protected:
    sf::Sound loseS;
    bool dot = 0;

    bool erroro = false;
    Texture *killer_txt = new Texture[1];
    Texture middle;
    Texture player_idle;
    Texture treasure_txt;
    Texture player_txt1;
    Texture player_txt2;
    Texture jump_txt;

    Texture doors_txt;
    Vector2i mapsize = Vector2i(1280, 720);
    sf::View view;
    std::vector<RectangleShape> particles;

    std::vector<Sprite> otherplayers;
    Sprite ground;
    FloatRect next_position;
    Vector2f velocity;
    Vector2f Positionfornet;
    Text You_Won;
    Font font;
    int level = 1;
    int killernumber = 0;
    int platforms_number = 4;
    bool playertxt = 0;
    bool jump_able;
    bool stop = 0;
    uint8_t mside = 0;
    SHA256 msha;
    uint8_t *digestp;
    uint8_t mhash[32];
    std::mutex msm;
    std::mutex mhashm;
    std::mutex opu;
    std::mutex possm;
    std::mutex stopm;

    std::thread nett;
    sf::SoundBuffer musicbuffer;

    sf::Sound music;

    sf::SoundBuffer jump1buffer;

    sf::Sound jump1;

    sf::SoundBuffer losebuffer;
    Event event;
    std::string nextmapname = "maps/fmap.map";



    Clock timer;
    Clock timerp;
    Clock pftimer;

    float fall = 0;

    // map_init();


    // window.setFramerateLimit(20);

    float rtimer;


    bool Networking();
    int colide();
    void mapf_init(std::string &mapfilename);
    Obstacle *treasure = nullptr;
    Obstacle *doors = new Obstacle[1];
    Obstacle *killer = new Obstacle[1];
    Platform *platforms = new Platform[4];


};


#endif //CPP_GAME_GAME_H
