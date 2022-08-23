#include <iostream>
#include "env_base.h"
using namespace std;

env_base::~env_base(){
    if(this->StateLoaded == true) {
        //delete State_table
        for(auto iter_s = this->State_table.begin(); iter_s != this->State_table.end(); ++iter_s) {
            delete iter_s->second;
            iter_s->second = 0;
        }
    
        //delete transition matrix
        for(auto iter_s = this->TransitionMatrix.begin();
            iter_s != this->TransitionMatrix.end(); ++iter_s) {
            for(auto iter_a = iter_s->second->begin();
                iter_a != iter_s->second->end(); ++iter_a) {
                delete iter_a->second;
                iter_a->second = 0;
            }
            delete iter_s->second;
            iter_s->second = 0;
        }
    }
}
