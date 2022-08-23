#ifndef _ENV_BASE_H
#define _ENV_BASE_H
#include <iostream>
#include <vector>
#include <unordered_map>
#include <vector>
using namespace std;

//base class of actions
class action_base {
public:
    virtual string GetStringVal() const = 0;
    virtual ~action_base(){};
};

//base class of states
class state_base {
public:
    virtual state_base* make_copy() = 0;
    //write state values as a string
    //for example, x1y2 for state_grid
    //used as q_value, v_value's hash table's key
    virtual string GetStringVal() = 0;
    virtual ~state_base(){};
};

class env_base {
public:
    //load the transition matrix and state table
    //must be called if MDP learning agent is used.
    virtual void load_all_state() = 0;
    //visualize the policy map
    virtual void plot_policy(unordered_map<string, string > &policy_table) = 0;
    //visualize the v value map
    virtual void plot_V_values(unordered_map<string, double > &V_value_table) = 0;
    //print the Q value table
    virtual void plot_Q_values(unordered_map<string, unordered_map<string, double>* > &Q_value_table) = 0;
    //return a vector of legal actions can be taken at state state_ptr
    //please manually delete the action pointers in the vector after use
    virtual vector<action_base*> LegalActions(state_base* state_ptr) = 0;
    //the reward/cost value of taking action Action_ptr at state state_ptr
    //and moving to state new_state_ptr
    virtual double GetReward(state_base* state_ptr, action_base* Action_ptr,
                      state_base* new_state_ptr) const = 0;
    //return possible next states if taking action Action_ptr at state state_ptr
    virtual vector<state_base*> NextState_state(state_base* state_ptr,
                                                action_base* Action_ptr) = 0;
    //probabilities corresponding to the possible next states given by function NextState_state
    virtual vector<double> NextState_prob(state_base* state_ptr,
                                          action_base* Action_ptr) = 0;
    //return the starting state
    virtual state_base* starting_state() = 0;
    //whether the state state_ptr is a terminal state
    virtual bool is_terminal(state_base* state_ptr) = 0;
    //return State_table's begin iterator to be used by the MDP agent
    virtual unordered_map<string, state_base*>::iterator State_table_begin_iter() = 0;
    //return State_table's end iterator to be used by the MDP agent
    virtual unordered_map<string, state_base*>::iterator State_table_end_iter() = 0;
    //destructor: delete transition matrix and state table if they are loaded
    virtual ~env_base();
protected:
    //if State_table and TransitionMatrix have been loaded
    bool StateLoaded;
    //A hash table of all the states. State_table[state_str] = state_ptr
    unordered_map<string, state_base*> State_table;
    //transition probability table
    //(*(*(this->TransitionMatrix[this_state_str]))[action_str])[next_state_str] = prob1
    //the probability to move to state next_state_str if action action_str is
    //taken at this_state_str is prob1
    unordered_map<string, unordered_map<string, unordered_map<string, double>* >* > TransitionMatrix;
};
#endif
