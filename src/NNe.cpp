//
// Created by mi on 13.11.23.
//

#include "NNe.h"
#include <cstdlib>
#include <random>
#include <ctime>
namespace NN {
    NNe::NNe(NNe &n1, NNe &n2, float random_chance){
        std::random_device rd;
        std::mt19937 rng(rd());
        std::uniform_int_distribution<> distrib(0, 1);
        std::uniform_int_distribution<> rn(0, (int)(1.0f/random_chance));

        this->net.layers.resize(n1.net.layers.size());
        for(size_t i = 0; i < this->net.layers.size(); i++){
            this->net.layers[i].dsize = n1.net.layers[i].dsize;
        }
        for(size_t i = 1; i < this->net.layers.size(); i++){
            std::uniform_real_distribution d{-(1.0f/ sqrt(this->net.layers[i-1].dsize)), (1.0f/ sqrt(this->net.layers[i-1].dsize))};
            this->net.layers[i].nodes.resize(n1.net.layers[i].nodes.size());
            size_t  nn = 0;
            for(node &x : this->net.layers[i].nodes){
                if(distrib(rng)) {
                    x = n1.net.layers[i].nodes[nn];
                }else{
                    x = n2.net.layers[i].nodes[nn];
                }
                for(size_t k = 0; k < x.weights.size(); k+= rn(rng)){
                    x.weights[k] = d(rng);
                }
                if(!rn(rng)){
                    x.bias = std::min<float>(d(rng), 1.0f);
                }

                nn++;
            }
        }
    }

    NNe::NNe(std::vector<size_t> layers) {
        this->net.layers.resize(layers.size());
        for(size_t i = 0; i < layers.size(); i++){
            this->net.layers[i].dsize = layers[i];
        }
        for(size_t i = 1; i < layers.size(); i++){
            this->net.layers[i].nodes.resize(layers[i]);
            for(node &x : this->net.layers[i].nodes){
                x.weights.resize(this->net.layers[i-1].dsize);
            }

        }

    }
    void NNe::Randomise() {
        std::random_device rd{};
        std::mt19937 gen{rd()};

        for(size_t i = 1; i < this->net.layers.size(); i++){
            std::uniform_real_distribution d{-(1.0f/ sqrt(this->net.layers[i-1].dsize)), (1.0f/ sqrt(this->net.layers[i-1].dsize))};
            for(node &x : this->net.layers[i].nodes){
                for (float &y : x.weights){
                    y = std::min<float>(d(gen), 1.0f);
                }
                x.bias = std::min<float>(d(gen), 1.0f);
            }
        }

    }
    std::vector<float> NNe::run(std::vector<float> input){
        assert(input.size()==this->net.layers[0].dsize);
        std::vector<float> pdata = input;
        std::vector<float> cdata;
        for(size_t i = 1; i < this->net.layers.size(); i++){
            cdata.clear();
            cdata.resize(this->net.layers[i].dsize);
            for(size_t xx = 0; xx < this->net.layers[i].nodes.size(); xx++){
                node& x = this->net.layers[i].nodes[xx];
                float val = 0.0f;
                for (size_t j = 0; j < pdata.size(); j++){
                    val+=pdata[j]*x.weights[j];
                }
                val =(*this->activation)(val);
                val+=x.bias;
                cdata[xx] = val;
            }
            pdata.clear();
            pdata = cdata;
        }
        return cdata;
    }
    NNe::~NNe() {

    }
}