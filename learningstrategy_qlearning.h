#ifndef _LEARNINGSTRATEGY_QLEARNING_H
#define _LEARNINGSTRATEGY_QLEARNING_H
#include <iostream>
#include <unordered_map>
#include "learningstrategy_mqbase.h"
using namespace std;
/*
 q-learning agent.
 */

class learningstrategy_ql : public learningstrategy_mqbase {
public:
    learningstrategy_ql(env_base* env_ptr_input,
                        const double& discount_input,
                        const int& max_iter_input,
                        const double& epsilon,
                        const double& LearningRate)
                    : learningstrategy_mqbase(env_ptr_input, discount_input, max_iter_input),
                        epsilon(epsilon), LearningRate(LearningRate) {}

    //The actual next state given current state and an action.
    //randomly sampled from all possible next states bsaed upon corresponding probs
    virtual state_base* actual_next_state(state_base* this_state_ptr, action_base* this_action_ptr);
    //An action randomly sampled from all the legal actions.
    action_base* get_random_action(state_base* state_ptr);
    //sample = R(s, a, s') + discount * max_a'{Q(s', a')}
    virtual double Q_sample_val(state_base* this_state_ptr,
                        action_base* this_action_ptr,
                        state_base* next_state_ptr);
    //Q(s, a) = (1 - alpha) * Q(s, a) + alpha * sample
    virtual double new_Q_val(state_base* this_state_ptr,
                      action_base* this_action_ptr,
                      const double& Q_sample);
    
    void update_LearningRate(const double& new_LR);
    
    virtual void train_model();
    
protected:
    double epsilon;         //explore or exploit
    double LearningRate;    //alpha
};

#endif
