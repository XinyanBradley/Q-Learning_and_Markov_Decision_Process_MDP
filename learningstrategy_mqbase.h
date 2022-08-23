#ifndef _LEARNINGSTRATEGY_MQBASE_H
#define _LEARNINGSTRATEGY_MQBASE_H
#include <iostream>
#include <unordered_map>
#include "env_gridworld.h"
using namespace std;
/*
 learningstrategy_mqbase is the bsae class of the MDP agent "learningstrategy_mdp"
 and the Q-learning agent "learningstrategy_qlearning".
 functions defined and implemented in this class are those to be use by both the
 MDP and the q-learning classes except the pure virtual train_model(), which are
 to be overwriten.
 */

class learningstrategy_mqbase {
public:
    //initialize environment pointer, discount rate and max iteration number
    //from user input
    learningstrategy_mqbase(env_base* env_ptr_input,
                          const double& discount_input,
                          const int& max_iter_input)
                        : env_ptr(env_ptr_input), discount(discount_input),
                            max_iter(max_iter_input) {}
    virtual ~learningstrategy_mqbase();
    //calculate the Q value explicitly as
    //Q(s, a) = Sum T(s, a, s')[R(s, a, s') + discount * V(s')] over s'
    virtual double calc_Q_value(state_base* state_ptr, action_base* action_ptr);
    //update updated_Q_value_table such that
    //(*updated_Q_value_table[state_str])[action_str] = val
    virtual void set_Q_value_in_updated_table(state_base* state_ptr,
                                              action_base* action_ptr, const double& val);
    //get the Q(s, a) value from Q_value_table. i.e.
    //(*(this->Q_value_table)[state_str])[action_str]
    virtual double get_Q_value(state_base* state_ptr, action_base* action_ptr);
    //loop over all possible actions a at state s to find the maximum Q(s, a)
    //which is the V(s) value.
    virtual double get_V_value(state_base* state_ptr);
    //return the action a that gives the maximum Q(s, a) at state s.
    virtual action_base* get_optimal_action(state_base* state_ptr);
    //update Q_value_table with Q values newly calculated in the current iteration,
    //which are stored in updated_Q_value_table.
    //this function is only called at the end of each iteration.
    virtual void updated_Q_table_to_Q_table();
    //the following two functions are to update the policy_table and V_value_table
    //using the Q_value_table
    //called at the end of the entire training process
    //to prepare for the plot step.
    void load_V_table_n_policy_table_from_Q_table_helper(state_base* this_state_ptr);
    virtual void load_V_table_n_policy_table_from_Q_table();
    //find the optimal scenarios in the environment using the MDP agent.
    virtual void train_model() = 0;
protected:
    //Table of Q values calculated by the most recent completed iteration.
    //key is state_ptr->GetStringVal()
    //second key is action_ptr->GetStringVal()
    //(*Q_value_table[state_str])[action_str] = Q
    unordered_map<string, unordered_map<string, double>* > Q_value_table;
    //Q values calculated in the ongoing(current) iteration
    //(*updated_Q_value_table[state_str])[action_str] = Q
    //When the current iteration finished, Q_value_table will be updated with
    //Q values stored in updated_Q_value_table
    unordered_map<string, unordered_map<string, double>* > updated_Q_value_table;
    //table of the optimal action to take at each state
    //key is state->GetStringVal();
    //value is action->GetStringVal();
    unordered_map<string, string> policy_table;
    //table of V values
    //key is state->GetStringVal();
    //note that V value at state s is the max possible Q value at s.
    unordered_map<string, double> V_value_table;
    //pointer to the environment to be explored.
    env_base* env_ptr;
    //gamma
    double discount;
    //maximum episode
    int max_iter;
};

#endif
