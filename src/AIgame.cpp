//
// Created by mi on 12.11.23.
//

#include <queue>
#include "AIgame.h"
AIgame::AIgame(int argc, char **argv, int xv, int yv) : game{argc, argv}, xv{xv}, yv{yv}{
    pixels = std::vector<float>(xv*yv);
    window.close();
    this->graphics = 0;
}

int AIgame::renderpx() {
    //for(auto & x: pixels){
    //    x = 0;
    //}
    //std::fill(pixels.begin(), pixels.end(), 0.0f);
    pixels.assign(pixels.size(), 0);
    std::queue<std::pair<IntRect, float>> draws;
    // drawing all


    draws.emplace(ground.getGlobalBounds(), 1.0f);
    if (!dot)
        draws.emplace(doors[0].shape.getGlobalBounds(), 1.0f);
    for (int i = 0; i < platforms_number; i++)
    {

        draws.emplace(Rect(platforms[i].Position, platforms[i].Size), 1.0f);
    }
    opu.lock();
    for (int i = 0; i < otherplayers.size(); i++)
    {
        draws.emplace(otherplayers[i].getGlobalBounds(), 0.7f);
    }
    opu.unlock();
    for (int i = 0; i < killernumber; i++)
    {
       draws.emplace( killer[i].shape.getGlobalBounds(), 1.0f);
    }

    msm.lock();





    // std::cout << particles.size();
    player.move(velocity);
    possm.lock();
    Positionfornet = player.getPosition();
    possm.unlock();

    if (!(fall < 0.1f && fall > -0.1f))
    {
        player.setTexture(jump_txt);

        mside = 3;
    }
    if (velocity.x < 0)
    {

        mside += 4;
    }
    msm.unlock();
    velocity = Vector2f(0, 0);
    int xb = 0, yb = 0;
    int pxppxx = 1280/xv;
    int pxppxy = 720/yv;
    // WORKING ON VIEWS
    xb = player.getPosition().x-xv*pxppxx/2;
    yb = player.getPosition().y-yv*pxppxy/2;
    xb/=pxppxx;
    yb/=pxppxy;


    next_position = player.getGlobalBounds();
    if (dot)
        draws.emplace(treasure[0].shape.getGlobalBounds(), 1.0f);
    draws.emplace(player.getGlobalBounds(), 0.5f);
    std::pair<IntRect, float> s;
    while(!draws.empty()){
        s = draws.front();
        draws.pop();

        for(int x = s.first.getPosition().x; x< s.first.getPosition().x + s.first.getSize().x; x+= pxppxx){
            if(x/pxppxx -xb < xv and x/pxppxx -xb > 0)
            for (int y = s.first.getPosition().y; y< s.first.getPosition().y + s.first.getSize().y; y+= pxppxx){
                if(y/pxppxy -yb < yv and y/pxppxy -yb > 0)
                pixels[(y/pxppxy-yb)*xv+(x/pxppxx-xb)] = s.second;
            }
        }

    }
    return 0;
}