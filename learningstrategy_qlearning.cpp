#include <iostream>
#include "learningstrategy_qlearning.h"
#include <stdlib.h>
using namespace std;

//sample the actual next state from all possible next states
state_base* learningstrategy_ql::actual_next_state(state_base* this_state_ptr,
                                                   action_base* this_action_ptr) {
    //get the possible next states and corresponding transition probabilities
    //at the current state and action
    vector<state_base*> next_states = env_ptr->NextState_state(this_state_ptr, this_action_ptr);
    vector<double> next_probs = env_ptr->NextState_prob(this_state_ptr, this_action_ptr);
    
    if(next_states.size() == 0) {
        cout << "no available actual next state" << endl;
        return new state_grid(0, 0);
    }
    
    if(next_states.size() != next_probs.size()) {
        cout << "num of available next state does not match num of prob entries" << endl;
        return new state_grid(0, 0);
    }
    
    //sample the actual next state
    double rand_num = ((double) rand() / (RAND_MAX));
    int actual_i = 0;
    double cum_prob = next_probs[actual_i];
    
    while(cum_prob < rand_num) {
        actual_i++;
        cum_prob += next_probs[actual_i];
    }
    
    for(auto i = 0; i < next_states.size(); ++i) {
        if(i == actual_i) {
            continue;
        }
        delete next_states[i];
        next_states[i] = 0;
    }
    
    return next_states[actual_i];
}

//for exploration.  Randomly choose an action
action_base* learningstrategy_ql::get_random_action(state_base* state_ptr) {
    vector<action_base* > action_ptr_vec = env_ptr->LegalActions(state_ptr);
    if(action_ptr_vec.size() == 0) {
        cout << "state " << state_ptr->GetStringVal() << " has no legal actions." << endl;
        return NULL;
    }
    
    double rand_num = ((double) rand() / (RAND_MAX));

    int actual_i = 0;
    
    while((double) (actual_i + 1) / action_ptr_vec.size() < rand_num) {
        actual_i++;
    }
    
    for(auto i = 0; i < action_ptr_vec.size(); ++i) {
        if(i == actual_i) {
            continue;
        }
        delete action_ptr_vec[i];
        action_ptr_vec[i] = 0;
    }
    
    return action_ptr_vec[actual_i];
    
}

//sample = R(s, a, s') + discount * max_a'{Q(s', a')}
double learningstrategy_ql::Q_sample_val(state_base* this_state_ptr,
                                         action_base* this_action_ptr,
                                         state_base* next_state_ptr) {
    return env_ptr->GetReward(this_state_ptr, this_action_ptr, next_state_ptr)
        + discount * get_V_value(next_state_ptr);
}

//Q(s, a) = (1 - alpha) * Q(s, a) + alpha * sample
double learningstrategy_ql::new_Q_val(state_base* this_state_ptr,
                                      action_base* this_action_ptr,
                                      const double& Q_sample) {
    return (1.0 - LearningRate) * get_Q_value(this_state_ptr, this_action_ptr)
        + LearningRate * Q_sample;
}

void learningstrategy_ql::update_LearningRate(const double& new_LR) {
    LearningRate = new_LR;
    return;
}

void learningstrategy_ql::train_model() {
    for(int episode = 0; episode < max_iter; ++episode) {
        //cout << "episode " << episode << " start..." << endl;
        state_base* this_state_ptr = env_ptr->starting_state();
        while(!env_ptr->is_terminal(this_state_ptr)) {
            action_base* this_action_ptr = NULL;
            //explore or exploit
            double ee_rand = ((double) rand() / (RAND_MAX));
            if(ee_rand < epsilon) {
                //explore
                this_action_ptr = get_random_action(this_state_ptr);
            }
            else {
                //exploit
                this_action_ptr = get_optimal_action(this_state_ptr);
            }
            state_base* next_state_ptr = actual_next_state(this_state_ptr, this_action_ptr);
            
            //update Q value
            double Sample = Q_sample_val(this_state_ptr, this_action_ptr, next_state_ptr);
            double New_Q = new_Q_val(this_state_ptr, this_action_ptr, Sample);
            set_Q_value_in_updated_table(this_state_ptr, this_action_ptr, New_Q);
            
            delete this_action_ptr;
            this_action_ptr = 0;
            
            //increment state
            state_base* victim_ptr = this_state_ptr;
            this_state_ptr = next_state_ptr;
            delete victim_ptr;
            victim_ptr = 0;
        }

        if(env_ptr->is_terminal(this_state_ptr)) {
            vector<action_base* > action_ptr_vec = env_ptr->LegalActions(this_state_ptr);
            if(action_ptr_vec.size() == 0) {
                cout << "state " << this_state_ptr->GetStringVal() << " has no legal action.  V=default" << endl;
                continue;
            }
            
            //action_ptr_vec.size() should be only one
            //the only legal action at terminal is "exit"
            for(int i = 0; i < action_ptr_vec.size(); ++i) {
                set_Q_value_in_updated_table(this_state_ptr, action_ptr_vec[i],
                    env_ptr->GetReward(this_state_ptr, action_ptr_vec[i], this_state_ptr));
                delete action_ptr_vec[i];
                action_ptr_vec[i] = 0;
            }
        }
        //cout << "final state: " << this_state_ptr->GetStringVal() << endl;
        delete this_state_ptr;
        this_state_ptr = 0;
        
        updated_Q_table_to_Q_table();
        
        //decrease epsilon and learningrate over iterations
        epsilon = epsilon - 1.0 / max_iter;
        LearningRate = LearningRate - 1.0 / max_iter;
        
    }
    
    load_V_table_n_policy_table_from_Q_table();
    
    env_ptr->plot_policy(policy_table);
    env_ptr->plot_V_values(V_value_table);
    
    return;
}
