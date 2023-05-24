#ifndef NN
#define NN
#include <mutex>
#include <thread>
#include <fstream>
#include <iostream>
#include <vector>
#include <cmath>
#include <SFML/Graphics.hpp> // gra używa biblioteki SFML ze strony https://www.sfml-dev.org/
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Network.hpp>
#include <SFML/Audio.hpp>
using namespace sf;
#define NumOfRays 64
#define raylength 500.0f
namespace mai
{
    std::ofstream lout;
    float S(float x)
    {
        return 0.5 * (x / (1 + abs(x)) + 1);
    }
    int spos = 600;

    const int kmp = 10000; // kill multiplier
    struct ainode
    {
        std::vector<float> weights;
        float bias;
    };
    struct ailayer
    {
        std::vector<ainode> nodes;
    };
    struct ainetwork
    {
        std::vector<ailayer> layers;
        int itskills = 0;
    };
    const int numofaipergen = 64;
    int changepart = 100;
    std::array<ainetwork, numofaipergen> topais;
    ainetwork aicurrent;
    std::vector<std::vector<float>> aid;
    std::vector<int> ailayers;
    float trainspeed = 500000;
    void RandomiseAi(ainetwork &aicurr)

    {
        srand((unsigned)time(NULL));
        // aicurr = topais[rand() % numofaipergen];
        for (int i = 1; i < ailayers.size(); i++)
        {

            // for (int j = 0; j < layer.size(); j++) {
            //	layer[j] = rand() % 1000;
            // }
            for (int j = 0; j < ailayers[i]; j++)
            {
                for (int k = 0; k < ailayers[i - 1]; k += (rand() % changepart))
                {
                    aicurr.layers[i].nodes[j].weights[k] += ((float)((rand() % 100000) - 50000) / trainspeed);
                    if (aicurr.layers[i].nodes[j].weights[k] > 1)
                    {
                        aicurr.layers[i].nodes[j].weights[k] = 1;
                    }
                    else if (aicurr.layers[i].nodes[j].weights[k] < -1)
                    {
                        aicurr.layers[i].nodes[j].weights[k] = -1;
                    }
                }
                aicurr.layers[i].nodes[j].bias += ((float)((rand() % 100000) - 50000) / trainspeed);
                // lout << ai[i][j][0];
            }
        }
    }
    float PerpDot(const sf::Vector2f &a, const sf::Vector2f &b) { return (a.y * b.x) - (a.x * b.y); }
    float LineCollision(const sf::Vector2f &A1, const sf::Vector2f &A2, const sf::Vector2f &B1, const sf::Vector2f &B2)
    {
        float dist;
        sf::Vector2f out;
        sf::Vector2f a(A2 - A1);
        sf::Vector2f b(B2 - B1);
        sf::Vector2f c(B2 - A2);

        float f = PerpDot(a, b);

        // lines are parallel
        if (!f)
        {

            return raylength;
        }

        float aa = PerpDot(a, c);
        float bb = PerpDot(b, c);

        // lout << bb << std::endl;

        if (f < 0)
        {
            if (aa > 0)
                return raylength;
            if (bb > 0)
                return raylength;
            if (aa < f)
                return raylength;
            if (bb < f)
                return raylength;
        }
        else
        {
            if (aa < 0)
                return raylength;
            if (bb < 0)
                return raylength;
            if (aa > f)
                return raylength;
            if (bb > f)
                return raylength;
        }

        out = b * (1.0f - (aa / f)) + B1;
        // assigns the point of intersection
        // lout << ' ' << out.x << ' ' << out.y << ' ' << A1.x << ' ' << A1.y << '\n';
        dist = sqrt((out.x - A1.x) * (out.x - A1.x) + (out.y - A1.y) * (out.y - A1.y));
        return dist;
    }
    float SpriteLineCollision(Sprite s, const sf::Vector2f &A1, const sf::Vector2f &A2)
    {
        float o = raylength;
        sf::Vector2f out;

        o = std::min(o, LineCollision(A1, A2, {s.getPosition().x, s.getPosition().y}, {s.getPosition().x, s.getPosition().y + s.getGlobalBounds().height}));
        o = std::min(o, LineCollision(A1, A2, {s.getPosition().x, s.getPosition().y}, {s.getPosition().x + s.getGlobalBounds().width, s.getPosition().y}));
        o = std::min(o, LineCollision(A1, A2, {s.getPosition().x, s.getPosition().y + s.getGlobalBounds().height}, {s.getPosition().x, s.getPosition().y + s.getGlobalBounds().height}));
        o = std::min(o, LineCollision(A1, A2, {s.getPosition().x + s.getGlobalBounds().width, s.getPosition().y}, {s.getPosition().x + s.getGlobalBounds().width, s.getPosition().y}));
        return o;
    }
    float RSLineCollision(RectangleShape s, const sf::Vector2f &A1, const sf::Vector2f &A2)
    {
        float o = raylength;
        sf::Vector2f out;

        o = std::min(o, LineCollision(A1, A2, {s.getPosition().x, s.getPosition().y}, {s.getPosition().x, s.getPosition().y + s.getGlobalBounds().height}));
        o = std::min(o, LineCollision(A1, A2, {s.getPosition().x, s.getPosition().y}, {s.getPosition().x + s.getGlobalBounds().width, s.getPosition().y}));
        o = std::min(o, LineCollision(A1, A2, {s.getPosition().x, s.getPosition().y + s.getGlobalBounds().height}, {s.getPosition().x, s.getPosition().y + s.getGlobalBounds().height}));
        o = std::min(o, LineCollision(A1, A2, {s.getPosition().x + s.getGlobalBounds().width, s.getPosition().y}, {s.getPosition().x + s.getGlobalBounds().width, s.getPosition().y}));
        return o;
    }

    std::array<float, NumOfRays> castraysp(Sprite &player, Sprite &other)
    {
        std::array<float, NumOfRays> out;
        for (int i = 0; i < NumOfRays; i++)
        {
            float angle = i / (float)NumOfRays;
            float x, y;
            x = player.getPosition().x + (cos(angle * M_PI * 2) * raylength);
            y = player.getPosition().y + (sin(angle * M_PI * 2) * raylength);
            float dist = SpriteLineCollision(other, player.getPosition(), {x, y});
            // lout << dist << '\n';

            out[i] = dist;
        }
        return out;
    }
    std::array<float, NumOfRays> castraysr(Sprite &player, RectangleShape &other)
    {
        std::array<float, NumOfRays> out;
        for (int i = 0; i < NumOfRays; i++)
        {
            float angle = i / (float)NumOfRays;
            float x, y;
            x = player.getPosition().x + (cos(angle * M_PI * 2) * raylength);
            y = player.getPosition().y + (sin(angle * M_PI * 2) * raylength);
            float dist = RSLineCollision(other, player.getPosition(), {x, y});
            // lout << dist << '\n';

            out[i] = dist;
        }
        return out;
    }
    std::array<float, NumOfRays> castrayso(Sprite player, std::vector<Sprite> otherplayers)
    {
        std::array<float, NumOfRays> out;
        for (int j = 0; j < NumOfRays; j++)
        {
            out[j] = raylength;
        }
        for (int i = 0; i < otherplayers.size(); i++)
        {
            std::array<float, NumOfRays> t = castraysp(player, otherplayers[i]);
            for (int j = 0; j < NumOfRays; j++)
            {
                out[j] = std::min(out[j], t[j]);
            }
        }

        return out;
    }
    std::array<float, NumOfRays> castraysk(Sprite &player, std::vector<Sprite *> killers)
    {
        std::array<float, NumOfRays> out;
        for (int j = 0; j < NumOfRays; j++)
        {
            out[j] = raylength;
        }
        for (int i = 0; i < killers.size(); i++)
        {
            std::array<float, NumOfRays> t = castraysp(player, *(killers[i]));
            for (int j = 0; j < NumOfRays; j++)
            {
                out[j] = std::min(out[j], t[j]);
            }
        }
        return out;
    }
    std::array<float, NumOfRays> castraysob(Sprite player, std::vector<RectangleShape *> obs)
    {
        std::array<float, NumOfRays> out;
        for (int j = 0; j < NumOfRays; j++)
        {
            out[j] = raylength;
        }
        for (int i = 0; i < obs.size(); i++)
        {
            std::array<float, NumOfRays> t = castraysr(player, *(obs[i]));
            for (int j = 0; j < NumOfRays; j++)
            {
                out[j] = std::min(out[j], t[j]);
            }
        }
        return out;
    }
    std::array<bool, 3> runstep(ainetwork &currai, Sprite &player, std::vector<Sprite> otherplayers, const std::vector<Sprite *> &killers, const std::vector<RectangleShape *> &obs, sf::RenderWindow &win)
    {
        std::ios_base::sync_with_stdio(0);
        lout.tie(0);
        std::array<bool, 3> out{0, 0, 0};
        std::array<float, NumOfRays> outt{0, 0, 0};
        std::array<float, NumOfRays * 3> FinalView;
        std::array<float, NumOfRays> obsv = castraysob(player, obs);
        std::fill(outt.begin(), outt.end(), raylength);
        for (int i = 0; i < NumOfRays; i++)
        {
            FinalView[i] = obsv[i];
            outt[i] = std::min(outt[i], obsv[i]);
        }
        std::array<float, NumOfRays> kiv = castraysk(player, killers);
        for (int i = NumOfRays; i < NumOfRays * 2; i++)
        {
            FinalView[i] = kiv[i - NumOfRays];
            outt[i - NumOfRays] = std::min(kiv[i - NumOfRays], outt[i - NumOfRays]);
        }
        std::array<float, NumOfRays> opv = castrayso(player, otherplayers);
        for (int i = NumOfRays * 2; i < NumOfRays * 3; i++)
        {
            FinalView[i] = opv[i - NumOfRays * 2];
            outt[i - NumOfRays * 2] = std::min(outt[i - NumOfRays * 2], opv[i - NumOfRays * 2]);
        }
        for (int i = 0; i < NumOfRays * 3; i++)
        {
            aid[0][i] = FinalView[i];
        }
        for (int i = 0; i < NumOfRays; i++)
        {
            float angle = i / (float)NumOfRays;
            float x, y;
            x = player.getPosition().x + (cos(angle * M_PI * 2) * outt[i]);
            y = player.getPosition().y + (sin(angle * M_PI * 2) * outt[i]);

            sf::Vertex line[] =
                {

                    sf::Vertex(sf::Vector2f(player.getPosition().x, player.getPosition().y), sf::Color(255 - outt[i] / raylength * 255, outt[i] / raylength * 255, 0)),
                    sf::Vertex(sf::Vector2f(x, y), sf::Color(255 - outt[i] / raylength * 255, outt[i] / raylength * 255, 0))};
            // win.draw(line, 2, sf::Lines);
        }
        // win.display();

        for (int k = 1; k < ailayers.size(); k++)
        {
            for (int j = 0; j < ailayers[k]; j++)
            {
                float outcome = 0;
                // lout << ailayers[k] << '\n';
                for (int i = 0; i < ailayers[k - 1]; i++)
                {
                    // lout <<aid[k - 1][i];
                    // lout << currai.layers[k].nodes[j].weights[i];
                    outcome += currai.layers[k].nodes[j].weights[i] * aid[k - 1][i];
                }
                // lout << currai.layers.size();
                // lout << currai.layers[k].nodes.size();
                outcome -= currai.layers[k].nodes[j].bias;
                outcome = S(outcome);
                aid[k][j] = outcome;
            }
        }
        for (int i = 0; i < ailayers[ailayers.size() - 1]; i++)
        {

            lout << aid[ailayers.size() - 1][i] << ' ';
        }

        if (aid[ailayers.size() - 1][0] - aid[ailayers.size() - 1][2] > 0.1f)
            out[0] = 1;
        else if (aid[ailayers.size() - 1][2] - aid[ailayers.size() - 1][0] > 0.1f)
            out[2] = 1;
        if (aid[ailayers.size() - 1][1] > 0.4f)
        {
            out[1] = 1;
        }
        lout << '\n';
        return out;
    }
    void BuildAi(std::vector<int> mailayers)
    {
        lout.open("ailogs.txt");
        ailayers = mailayers;
        srand((unsigned)time(NULL));
        for (int i = 0; i < ailayers.size(); i++)
        {

            // for (int j = 0; j < layer.size(); j++) {
            //	layer[j] = rand() % 1000;
            // }
            aid.push_back(std::vector<float>(ailayers[i]));
        }
        // topais[d].push_back(std::vector<std::vector<float>>());
        for (int d = 0; d < numofaipergen; d++)
        {
            topais[d].layers.push_back(ailayer());
            for (int i = 1; i < ailayers.size(); i++)
            {

                // for (int j = 0; j < layer.size(); j++) {
                //	layer[j] = rand() % 1000;
                // }
                ailayer currentlayer;
                for (int j = 0; j < ailayers[i]; j++)
                {
                    ainode tonode;
                    for (int k = 0; k < ailayers[i - 1]; k++)
                    {
                        tonode.weights.push_back(((float)(rand() % 1200) / 1000.0f) - 0.5f);
                    }
                    tonode.bias = (float)(rand() % 1000) / 1000;
                    currentlayer.nodes.push_back(tonode);
                    // lout << ai[i][j][0];
                }
                topais[d].layers.push_back(currentlayer);
                lout << topais[d].layers.size();
            }
        }
    }
    void genpop()
    {
        ainetwork tops[32];
        int jjj[32] = {-1, -1, -1, -1, -1};
        for (int i = 0; i < 32; i++)
        {

            int maxim = 0;
            for (int i = 0; i < topais.size(); i++)
            {
                bool is_valid = true;
                for (int d : jjj)
                {
                    if (i == d)
                        is_valid = false;
                }
                if (!is_valid)
                    continue;
                if (topais[i].itskills >= topais[maxim].itskills)
                    maxim = i;
            }
            jjj[i] = maxim;
            tops[i] = topais[maxim];
        }
        lout;
        for (int d = 0; d < numofaipergen; d++)
        {
            for (int i = 1; i < ailayers.size(); i++)
            {

                // for (int j = 0; j < layer.size(); j++) {
                //	layer[j] = rand() % 1000;
                // }
                for (int j = 0; j < ailayers[i]; j++)
                {
                    for (int k = 0; k < ailayers[i - 1]; k++)
                    {
                        topais[d].layers[i].nodes[j].weights[k] = tops[rand() % 5].layers[i].nodes[j].weights[k];
                    }
                    topais[d].layers[i].nodes[j].bias = tops[rand() % 5].layers[i].nodes[j].bias;
                    // lout << ai[i][j][0];
                }
            }
            RandomiseAi(topais[d]);
        }
    }
    void deinitialize()
    {
        lout.close();
    }
}

#endif