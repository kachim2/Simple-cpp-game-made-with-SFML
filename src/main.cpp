#include <omp.h>
#include <random>
#include "AIgame.h"
#include "NNe.h"
#include "SFML/Graphics.hpp"

//int main(int argc, char **argv){
//    AIgame the_game(argc, argv, 144, 72);
//    int frame = 0;
//    auto start = std::chrono::system_clock::now();
//    while (true){
//        the_game.step(std::array<bool, 3>{0,0,0});
//        the_game.renderpx();
//        sf::Uint8* pixels = new sf::Uint8[144*72*3];
//
//        FILE *fp = fopen((std::to_string(frame) + "c.ppm").c_str(), "wb"); /* b - binary mode */
//        fprintf(fp, "P6\n%d %d\n255\n", 144, 72);
//        for(auto x : the_game.pixels){
//            static unsigned char color[3];
//            x*=255;
//
//            color[0] = x;  /* red */
//            color[1] = x;  /* green */
//            color[2] = x;  /* blue */
//
//            fwrite(color, 1, 3, fp);
//        }
//        fclose(fp);
//        frame++;
//        auto now  = std::chrono::system_clock::now();
//        std::cerr << (double)frame/(double)std::chrono::duration_cast<std::chrono::seconds>(now - start).count() << '\n';
//    }
//
//    return 0;
//}
//int main(){
//    NN::NNe nn(std::vector<size_t>{2, 10, 2});
//    nn.Randomise();
//    std::vector<float> r = nn.run({1.0, 0.0});
//    for(auto x: r){
//        std::cout<<x <<' ';
//    }
//}


int main(int argc, char **argv){
    int num = 9;
    std::random_device rd;  // a seed source for the random number engine
    std::mt19937 gen(rd()); // mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> distrib(0, num);
    std::vector<std::unique_ptr<AIgame>> games(num);
    std::vector<NN::NNe> NNs(num, std::vector<size_t >{72*36+4, 72*36, 72*18, 72, 72, 3});
    std::vector<float> scores(num);
#pragma omp parallel for
    for (auto &i : NNs){

        i.Randomise();
    }
    std::vector<NN::NNe> NNs2(num, std::vector<size_t >{72*36+4, 72*36, 72*18, 72, 72, 3});
    size_t generation = 0;

    while (true){
        std::fill(scores.begin(), scores.end(), 0.0f);
        generation++;
        games.clear();
        games.resize(num);
        for (auto &i : games){
            i = std::make_unique<AIgame>(argc, argv, 72, 36);
        }

        #pragma omp parallel for
        for(size_t game= 0; game<num; game++){
            for(size_t frame = 0; frame<450; frame++){

                games[game]->renderpx();
                if (game == 0){
                    FILE *fp = fopen(("S"+std::to_string(generation) +"E"+ std::to_string(frame) + "c.ppm").c_str(), "wb"); /* b - binary mode */
                    fprintf(fp, "P6\n%d %d\n255\n", 72, 36);
                    for(auto x : games[game]->pixels){
                        static unsigned char color[3];
                        x*=255;

                        color[0] = x;  /* red */
                        color[1] = x;  /* green */
                        color[2] = x;  /* blue */

                        fwrite(color, 1, 3, fp);
                    }
                    fclose(fp);
                }
                std::vector<float> imin = games[game]->pixels;
                imin.push_back((float)games[game]->player.getPosition().x/1280.0f);
                imin.push_back((float)games[game]->player.getPosition().y/720.0f);
                imin.push_back((float)900/1280.0f);
                imin.push_back((float)games[game]->player.getPosition().y/720.0f);
                std::vector<float> vec = NNs[game].run(imin);

                std::array<bool, 3> ar;
                for(int i = 0; i < 3; i++){

                    if(vec[i] > 0.1f) {
                        ar[i] = 1;
                        //std::cout << vec[i];
                    }
                    else
                        ar[i] = 0;
                }
                games[game]->step(ar, 0.033);
                //std::cout << frame;
                //std::cout << "Current thread number: " << omp_get_thread_num() << std::endl;
            }
            std::cerr << game << std::endl;
            scores[game]+=std::abs((float)games[game]->player.getPosition().x/1280.0f-900/1280.0f);
            scores[game]+=std::abs((float)games[game]->player.getPosition().y/720.0f-(float)games[game]->player.getPosition().y/720.0f);
            scores[game] = 1/std::min(0.01f, scores[game]);
        }

        float total_probability = 0;
        for (int i = 0; i <num ; ++i) {
            total_probability += scores[i];
        }
        for(size_t game= 0; game<num; game++){
            float p = (rand() / static_cast<float>(RAND_MAX)) * total_probability;
            float p2 = (rand() / static_cast<float>(RAND_MAX)) * total_probability;
            size_t cur1 = 0, cur2 = 0;
            while ( (p -= scores[cur1]) > 0)
                ++cur1;
            while ( (p2 -= scores[cur2]) > 0)
                ++cur2;
            NNs2[game] = NN::NNe(NNs[cur1], NNs[cur2], 0.001f);
        }
        for(size_t i = 0; i < num; i++){

            std::swap(NNs[i], NNs2[i]);
            //NNs[i] = std::move(NNs2[i]);
        }
        //std::swap(NNs, NNs2)
        //sleep(sf::seconds(1000));
        std::cerr << "safgsgfsdfgs";
    }
    AIgame the_game(argc, argv, 144, 72);
    int frame = 0;
    auto start = std::chrono::system_clock::now();
    while (true){
        the_game.step(std::array<bool, 3>{0,0,0}, 0.033);
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
