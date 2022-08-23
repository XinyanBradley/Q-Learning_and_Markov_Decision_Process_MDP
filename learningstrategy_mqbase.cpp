#include <iostream>
#include "learningstrategy_mqbase.h"
using namespace std;

//destructor. delete the Q_value_table.
learningstrategy_mqbase::~learningstrategy_mqbase() {
    for(auto iter = this->Q_value_table.begin(); iter != this->Q_value_table.end(); ++iter) {
        delete iter->second;
        iter->second = 0;
    }
}

//calculate Q(s, a) = Sum T(s, a, s')[R(s, a, s') + discount * V(s')] over s'
double learningstrategy_mqbase::calc_Q_value(state_base* state_ptr, action_base* action_ptr) {
    vector<state_base*> next_states = this->env_ptr->NextState_state(state_ptr, action_ptr);
    vector<double> next_probs = this->env_ptr->NextState_prob(state_ptr, action_ptr);
    double Q_val = 0.0;
    for(auto i_NextState = 0; i_NextState < next_states.size(); ++i_NextState) {
        state_base* next_state_ptr = next_states[i_NextState];
        double next_prob = next_probs[i_NextState];
        Q_val += next_prob * (env_ptr->GetReward(state_ptr, action_ptr, next_state_ptr)
                              + this->discount * get_V_value(next_state_ptr));
        delete next_state_ptr;
    }
    return Q_val;
}

//update (*updated_Q_value_table[state_str])[action_str] as val
void learningstrategy_mqbase::set_Q_value_in_updated_table(state_base* state_ptr, action_base* action_ptr,
                                        const double& val) {
    string state_str = state_ptr->GetStringVal();
    string action_str = action_ptr->GetStringVal();
    
    if(this->updated_Q_value_table.find(state_str) == this->updated_Q_value_table.end()) {
        unordered_map<string, double>* temp_ptr = new unordered_map<string, double>;
        this->updated_Q_value_table[state_str] = temp_ptr;
    }
    (*(this->updated_Q_value_table)[state_str])[action_str] = val;
    
    return;
}

//get the Q(s, a) value from Q_value_table.
//also initialize Q(s, a) to default value (0 here) if it is not set before.
double learningstrategy_mqbase::get_Q_value(state_base* state_ptr, action_base* action_ptr) {
    //unordered_map<string, unordered_map<string, double>* > Q_value_table;
    string state_str = state_ptr->GetStringVal();
    string action_str = action_ptr->GetStringVal();
    if(this->Q_value_table.find(state_str) != this->Q_value_table.end()) {
        unordered_map<string, double>* Q_s_table_ptr = this->Q_value_table[state_str];
        if(Q_s_table_ptr->find(action_str) != Q_s_table_ptr->end()) {
            return (*Q_s_table_ptr)[action_str];
        }
    }
    else {
        unordered_map<string, double>* Q_s_table_ptr = new unordered_map<string, double>;
        this->Q_value_table[state_str] = Q_s_table_ptr;
    }
    (*(this->Q_value_table)[state_str])[action_str] = 0;
    return 0;
}

//iterate over updated_Q_value_table to
//move Q values in updated_Q_value_table to Q_value_table
//and reset the updated_Q_value_table to empty.
void learningstrategy_mqbase::updated_Q_table_to_Q_table() {
    for(auto iter_s = this->updated_Q_value_table.begin();
        iter_s != this->updated_Q_value_table.end(); ++iter_s) {
        string state_str = iter_s->first;
        unordered_map<string, double>* updated_Q_s_table_ptr = iter_s->second;
        for(auto iter_a = updated_Q_s_table_ptr->begin(); iter_a != updated_Q_s_table_ptr->end(); ++iter_a) {
            string action_str = iter_a->first;
            if(this->Q_value_table.find(state_str) == this->Q_value_table.end()) {
                unordered_map<string, double>* Q_s_table_ptr = new unordered_map<string, double>;
                this->Q_value_table[state_str] = Q_s_table_ptr;
            }
            (*(this->Q_value_table)[state_str])[action_str] = iter_a->second;
        }
    }
    
    for(auto iter = this->updated_Q_value_table.begin(); iter != this->updated_Q_value_table.end(); ++iter) {
        delete iter->second;
        iter->second = NULL;
    }
    this->updated_Q_value_table.clear();
    return;
}

//loop over all possible actions a at state s to find the maximum Q(s, a)
//which is the V(s) value.
double learningstrategy_mqbase::get_V_value(state_base* state_ptr) {
    vector<action_base* > action_ptr_vec = env_ptr->LegalActions(state_ptr);
    if(action_ptr_vec.size() == 0) {
        cout << "state " << state_ptr->GetStringVal() << " has no legal action.  V=default" << endl;
        return 0;
    }
    action_base* action_ptr = action_ptr_vec[0];
    double best_Q = get_Q_value(state_ptr, action_ptr);
    for(auto i = 1; i < action_ptr_vec.size(); ++i) {
        action_ptr = action_ptr_vec[i];
        double this_Q = get_Q_value(state_ptr, action_ptr);
        if(this_Q > best_Q) {
            best_Q = this_Q;
        }
    }
    
    for(auto i = 0; i < action_ptr_vec.size(); ++i) {
        delete action_ptr_vec[i];
        action_ptr_vec[i] = 0;
    }
    
    return best_Q;
}

//loop over all possible actions a at state s to find the a
//that gives the maximum Q(s, a)
action_base* learningstrategy_mqbase::get_optimal_action(state_base* state_ptr) {
    
    vector<action_base* > action_ptr_vec = env_ptr->LegalActions(state_ptr);
    if(action_ptr_vec.size() == 0) {
        cout << "state " << state_ptr->GetStringVal() << " has no legal actions." << endl;
        return NULL;
    }
    action_base* action_ptr = action_ptr_vec[0];
    action_base* best_action_ptr = action_ptr_vec[0];
    double best_Q = get_Q_value(state_ptr, action_ptr);
    for(auto i = 1; i < action_ptr_vec.size(); ++i) {
        action_ptr = action_ptr_vec[i];
        double this_Q = get_Q_value(state_ptr, action_ptr);
        if(this_Q > best_Q) {
            best_Q = this_Q;
            best_action_ptr = action_ptr;
        }
    }
    
    
    //delete action pointers in action_ptr_vec
    for(auto i = 0; i < action_ptr_vec.size(); ++i) {
        if(best_action_ptr->GetStringVal() != (action_ptr_vec[i])->GetStringVal()) {
            delete action_ptr_vec[i];
            action_ptr_vec[i] = 0;
        }
    }
    
    return best_action_ptr;
}


//recursively traverse over all the states and actions to udpate the
//policy table and V table from the values in Q_value_table
void learningstrategy_mqbase::load_V_table_n_policy_table_from_Q_table_helper(state_base* this_state_ptr) {
    //update the policy table and V table
    action_base* optimal_action = get_optimal_action(this_state_ptr);
    policy_table[this_state_ptr->GetStringVal()] = optimal_action->GetStringVal();
    delete optimal_action;
    optimal_action = 0;
    V_value_table[this_state_ptr->GetStringVal()] = get_V_value(this_state_ptr);
    
    //terminate the recursion if terminal state is reached
    if(env_ptr->is_terminal(this_state_ptr)) {
        return;
    }
    
    //loop over all possible actions and potential next states
    vector<action_base* > action_ptr_vec = env_ptr->LegalActions(this_state_ptr);
    for(int i = 0; i < action_ptr_vec.size(); ++i) {
        action_base* this_action_ptr = action_ptr_vec[i];
        vector<state_base*> state_vec = env_ptr->NextState_state(this_state_ptr, this_action_ptr);
        for(auto j = 0; j < state_vec.size(); ++j) {
            state_base* next_state_ptr = state_vec[j];
            //recursively search the next state
            if(V_value_table.find(next_state_ptr->GetStringVal()) == V_value_table.end()) {
                load_V_table_n_policy_table_from_Q_table_helper(next_state_ptr);
            }
            delete next_state_ptr;
            next_state_ptr = 0;
        }
        
        delete this_action_ptr;
        this_action_ptr = 0;
    }
    
    //delete state_ptr;
    //state_ptr = 0;
    return;
    
}

//update the policy_table and V_value_table using the Q_value_table
//called at the end of the entire training process to prepare for the plot step.
void learningstrategy_mqbase::load_V_table_n_policy_table_from_Q_table() {
    V_value_table.clear();
    policy_table.clear();
    state_base* state_ptr = env_ptr->starting_state();
    load_V_table_n_policy_table_from_Q_table_helper(state_ptr);
    delete state_ptr;
    state_ptr = 0;
    return;
}
