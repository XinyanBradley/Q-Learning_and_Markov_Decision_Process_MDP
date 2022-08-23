#include <iostream>
#include "learningstrategy_mdp.h"
using namespace std;

void learningstrategy_mdp :: train_model() {
    //call load_all_state to load transition matrix and state table
    env_ptr->load_all_state();
    
    //optimize Q value start
    for(int episode = 0; episode < max_iter; ++episode) {
        //cout << "episode " << episode << " start..." << endl;
        //iterate over all the states in state_table
        auto State_table_iter_begin = this->env_ptr->State_table_begin_iter();
        auto State_table_iter_end = this->env_ptr->State_table_end_iter();
        for(auto iter_s = State_table_iter_begin; iter_s != State_table_iter_end; ++iter_s) {
            state_base* this_state_ptr = iter_s->second;
            vector<action_base* > action_ptr_vec
                = this->env_ptr->LegalActions(this_state_ptr);
            //iterate over all possible actions at the current state.
            for(auto i_a = 0; i_a < action_ptr_vec.size(); ++i_a) {
                action_base* this_action_ptr = action_ptr_vec[i_a];
                double New_Q = 0.0;
                if(this->env_ptr->is_terminal(this_state_ptr)) {
                    //if this state is a terminal state
                    //its Q value is just the reward/penalty of getting this state.
                    //Note: IN this case, the only possible action would be "exit",
                    //so it's OK to put it inside the action loop
                    New_Q = this->env_ptr->GetReward(this_state_ptr, this_action_ptr, this_state_ptr);
                }
                else {
                    //if this state is not a terminal state
                    //calculate its Q value based on action costs & v values of following possible states
                    New_Q = calc_Q_value(this_state_ptr, this_action_ptr);
                }
                //update the updated_Q_value_table
                set_Q_value_in_updated_table(this_state_ptr, this_action_ptr, New_Q);
            }
            
            //delete the possible action list
            for(auto i_a = 0; i_a < action_ptr_vec.size(); ++i_a) {
                delete action_ptr_vec[i_a];
                action_ptr_vec[i_a] = 0;
            }
        }
        //update Q_value_table with Q values that are newly calculated in this iteration
        //and temporarily stored in update_Q_table
        updated_Q_table_to_Q_table();
    }
    
    //update V value for policy plot
    //Note: can also use load_V_table_n_policy_table_from_Q_table()
    // but it can be easier since state_table exists.
    auto State_table_iter_begin = this->env_ptr->State_table_begin_iter();
    auto State_table_iter_end = this->env_ptr->State_table_end_iter();
    for(auto iter_s = State_table_iter_begin;
        iter_s != State_table_iter_end; ++iter_s) {
        state_base* this_state_ptr = iter_s->second;
        action_base* optimal_action = get_optimal_action(this_state_ptr);
        policy_table[this_state_ptr->GetStringVal()] = optimal_action->GetStringVal();
        delete optimal_action;
        optimal_action = 0;
        V_value_table[this_state_ptr->GetStringVal()] = get_V_value(this_state_ptr);
    }
    
    env_ptr->plot_policy(policy_table);
    env_ptr->plot_V_values(V_value_table);
    
    return;
}
