//
// Created by mi on 13.11.23.
//

#ifndef CPP_GAME_NNE_H
#define CPP_GAME_NNE_H
#include <vector>
#include <cmath>
#include "cista/containers/vector.h"
#include "cista/reflection/to_tuple.h"
namespace NN {
    namespace data = cista::raw;
    struct node{
        float bias;
        data::vector<float> weights;
    };
    struct layer{
        u_int64_t dsize;
        data::vector<node> nodes;
    };
    struct NNd {
        data::vector<layer> layers;
    };

    class NNe {
    public:
        NNd net;
        NNe(std::vector<size_t> layers);
        NNe(NNe &n1, NNe &n2, float random_chance);
        NNe(std::string filename);
        void Randomise();
        std::vector<float> run(std::vector<float> input);
        float (*activation)(float ) = &std::tanh;
        ~NNe();

    };



}
#endif //CPP_GAME_NNE_H