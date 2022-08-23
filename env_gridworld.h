#ifndef _ENV_GRIDWORLD_H
#define _ENV_GRIDWORLD_H
#include <iostream>
#include "env_base.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <string>
using namespace std;
/*
 gridworld environment
 */

struct IntPair {
    int x;
    int y;
};

//actions in gridworld environment
//directions the agent can go
class action_grid : public action_base {
public:
    action_grid(const string& val) : dir(val) {}
    //return values in string format
    virtual string GetStringVal() const;
    virtual ~action_grid(){}

private:
    //possible values: 'N', 'S', 'W', 'E', "EXIT"
    string dir;
};

//states in gridworld environment
//coordinate in the gridworld
class state_grid : public state_base {
public:
    state_grid(const IntPair& pos_input);
    state_grid(const int& X, const int& Y);
    //return the position in IntPair format
    IntPair GetPairVal() const;
    //make a DEEP copy of this state
    virtual state_base* make_copy();
    //return the position in string format
    virtual string GetStringVal();
    virtual ~state_grid(){}
private:
    IntPair pos;
};

//template <class state_class, class action_class>
class env_gridworld : public env_base {
public:
    env_gridworld(vector<vector<string > > &problem_map_input,
                  vector<vector<double > > &rewards_map_input,
                  const double& prob);
    
    //see env_base for description
    virtual void load_all_state();
    //see env_base for description
    virtual void plot_policy(unordered_map<string, string> &policy_table);
    //see env_base for description
    virtual void plot_V_values(unordered_map<string, double> &V_value_table);
    //see env_base for description
    virtual void plot_Q_values(unordered_map<string, unordered_map<string, double>* > &Q_value_table);
    //find the legal actions can be taken using the loaded transition matrix
    //if transition matrix is not loaded, call calc_LegalActions to find legal actions explicitly
    virtual vector<action_base*> LegalActions(state_base* state_ptr);
    //find the legal actions can be taken at state_ptr explicitly
    vector<action_base*> calc_LegalActions(state_base* state_ptr);
    //return the value in rewards_map with coordinate of state_ptr.
    //If state_ptr is a terminal state, the reward/penalty of reaching state_ptr is given
    //If not, GetReward gives the costs of taking action Action_ptr (flat 0.01 in this demo)
    //new_state_ptr is a dummy variable for env_gridworld's GetReward
    virtual double GetReward(state_base* state_ptr, action_base* Action_ptr,
                      state_base* new_state_ptr) const;
    //return possible next states if taking action Action_ptr at state state_ptr
    //NextState_state finds these states from the transition matrix if it is loaded.
    //if TM is not loaded, it calls calc_NextState_state to find these states explicitly
    virtual vector<state_base*> NextState_state(state_base* state_ptr, action_base* Action_ptr);
    //finds possible next states explicitly
    vector<state_base*> calc_NextState_state(state_base* state_ptr, action_base* Action_ptr);
    //return probabilities corresponding to the possible next states found by the previous two functions.
    //NextState_prob finds these probabilities from the transition matrix if it is loaded.
    //if TM is not loaded, it calls calc_NextState_state to find them explicitly
    virtual vector<double> NextState_prob(state_base* state_ptr, action_base* Action_ptr);
    //calc_NextState_prob calculate the next state transition probabilities explicitly
    vector<double> calc_NextState_prob(state_base* state_ptr, action_base* Action_ptr);
    //return the starting position
    virtual state_base* starting_state();
    //if this position is a terminal position
    virtual bool is_terminal(state_base* state_ptr);
    //see env_base for description
    virtual unordered_map<string, state_base*>::iterator State_table_begin_iter();
    //see env_base for description
    virtual unordered_map<string, state_base*>::iterator State_table_end_iter();
    virtual ~env_gridworld(){}

protected:
    //map of the gridworld
    vector<vector<string > > problem_map;
    //reward value of each position
    vector<vector<double > > rewards_map;
    //probability of moving as planned.
    //prob of sliding left/right is assumed to be the same
    double prob_correct;
};

#endif
