#ifndef _LEARNINGSTRATEGY_MDP_H
#define _LEARNINGSTRATEGY_MDP_H
#include <iostream>
#include "learningstrategy_mqbase.h"
using namespace std;

/*
 MDP learning agent.
 */

class learningstrategy_mdp : public learningstrategy_mqbase {
public:
    learningstrategy_mdp(env_base* env_ptr_input,
                        const double& discount_input,
                        const int& max_iter_input)
                    : learningstrategy_mqbase(env_ptr_input, discount_input, max_iter_input) {}
    virtual void train_model();
};

#endif
