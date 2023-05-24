#ifndef BC
#define BC
#include <mutex>
#include <thread>
#include <fstream>
#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp> // gra używa biblioteki SFML ze strony https://www.sfml-dev.org/
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Network.hpp>
#include <SFML/Audio.hpp>
using namespace sf;
class Obstacle
{

public:
    Sprite shape;
    Vector2f Size, x_y;
    Obstacle(Texture txt = white, Vector2f Start_Size = Vector2f(0, 0), Vector2f Start_x_y = Vector2f(0, 0))
    {
        white.setRepeated(true);
        Size = Start_Size;
        x_y = Start_x_y;
        shape.setTexture(txt);

        update();
    }
    void update()
    {

        shape.setPosition(x_y);
        shape.setScale({1, 1});
        shape.setTextureRect(IntRect({0, 0}, {(int)Size.x, (int)Size.y}));
    }
    void draw()
    {
        window.draw(shape);
    }
};
#endif