//
// Created by mi on 13.11.23.
//

#include "NNe.h"
#include <cstdlib>
NNe::NNe(std::vector<size_t> layers) {
    this->layers = (float ***) malloc(sizeof (float **)*layers.size());
    for(size_t i = 0; i < layers.size()-1; i++){
        this->NNs+= layers[i] * layers[i+1];
    }
    size_t numbias;
    for(size_t i = 0; i < layers.size(); i++)
    {
        numbias+=layers[i];
    }
    this->NN = (float *) malloc(sizeof (float )*NNs);
    for(size_t i = 0; i < layers.size()-1; i++){
        this->layers[i] = (float **)malloc(sizeof (float *)*layers[i]);
        for(size_t j = 0; j< layers[i]; j++) {
            this->layers[i][j] = NN;
        }
    }
}
