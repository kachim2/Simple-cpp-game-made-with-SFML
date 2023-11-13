//
// Created by mi on 13.11.23.
//

#ifndef CPP_GAME_NNE_H
#define CPP_GAME_NNE_H
#include <vector>

class NNe {
public:
    NNe(std::vector<size_t> layers);
    ~NNe();
    float *NN;
    size_t NNs;
    float *NNdata;
    size_t NNdatas;
    float ***layers;
    float **biases;
    float (*activation)(float );
};


#endif //CPP_GAME_NNE_H
