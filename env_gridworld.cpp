#include <iostream>
#include "env_gridworld.h"
#include <string>
#include <unordered_map>
#include <vector>
using namespace std;

string action_grid::GetStringVal() const {
    return this->dir;
}

state_grid::state_grid(const IntPair& pos_input){
    this->pos = pos_input;
}

state_grid::state_grid(const int& X, const int& Y){
    this->pos.x = X;
    this->pos.y = Y;
}

state_base* state_grid::make_copy() {
    state_base* new_state_ptr = new state_grid(this->pos.x, this->pos.y);
    return new_state_ptr;
}

string state_grid::GetStringVal() {
    return "x" + to_string(pos.x) + "y" + to_string(pos.y);
}

IntPair state_grid::GetPairVal() const {
    return this->pos;
}

env_gridworld::env_gridworld(vector<vector<string > > &problem_map_input,
                             vector<vector<double > > &rewards_map_input,
                             const double& prob) {
    this->problem_map = problem_map_input;
    this->rewards_map = rewards_map_input;
    this->prob_correct = prob;
    load_all_state();
}

void env_gridworld::load_all_state() {
    if(this->StateLoaded == true) {
        return;
    }
    //loop over all the coordinates in the gridworld map
    for(int j = 0; j < this->problem_map.size(); ++j) {
        for(int i = 0; i < this->problem_map[0].size(); ++i) {
            //"X" means it is a block
            if((this->problem_map[j][i] != "X") && (this->problem_map[j][i] != "x")) {
                state_base* this_state_ptr = new state_grid(i, j);
                string this_state_str = this_state_ptr->GetStringVal();
                //State_table
                this->State_table[this_state_str] = this_state_ptr;
                //TransitionMatrix
                this->TransitionMatrix[this_state_str]
                    = new unordered_map<string, unordered_map<string, double>* >;
                if(this->is_terminal(this_state_ptr)) {
                    //terminal state has only one legal action: "EXIT".
                    action_base* action_exit_ptr = new action_grid("EXIT");
                    string action_exit_str = action_exit_ptr->GetStringVal();
                    delete action_exit_ptr;
                    (*(this->TransitionMatrix[this_state_str]))[action_exit_str]
                        = new unordered_map<string, double>;
                    (*(*(this->TransitionMatrix[this_state_str]))[action_exit_str])[this_state_str] = 1.0;
                }
                else {
                    vector<action_base* > action_ptr_vec = LegalActions(this_state_ptr);
                    //loop over possible actions at the current state
                    for(auto i_action = 0; i_action < action_ptr_vec.size(); ++i_action) {
                        action_base* this_action_ptr = action_ptr_vec[i_action];
                        string this_action_str = this_action_ptr->GetStringVal();
                        //transition matrix hash table initialization
                        (*(this->TransitionMatrix[this_state_str]))[this_action_str]
                            = new unordered_map<string, double>;
                        
                        vector<state_base*> next_states = NextState_state(this_state_ptr, this_action_ptr);
                        vector<double> next_probs = NextState_prob(this_state_ptr, this_action_ptr);
                        //loop over all possible next states given current state and action
                        for(auto i_NextState = 0; i_NextState < next_states.size(); ++i_NextState) {
                            string next_state_str = next_states[i_NextState]->GetStringVal();
                            double next_prob = next_probs[i_NextState];
                            if((*(this->TransitionMatrix[this_state_str]))[this_action_str]->find(next_state_str)
                               == (*(this->TransitionMatrix[this_state_str]))[this_action_str]->end()) {
                                //the key (current state, action, next state) is firstly seen
                                (*(*(this->TransitionMatrix[this_state_str]))[this_action_str])[next_state_str] = next_prob;
                            }
                            else {
                                //the key (current state, action, next state) is not firstly seen
                                //an example of triggering this scenarion:
                                //Consider a position where its west and east sides are both blocked.
                                //if action "N" is taken, the agent has 10% chance to slide to west and 10% to east.
                                //However, since  west and east are blocked, either scenarions will stay stationary.
                                //Thus (current state, "N", current state) will appear twice and will result in a 20% prob.
                                (*(*(this->TransitionMatrix[this_state_str]))[this_action_str])[next_state_str] += next_prob;
                            }
                            delete next_states[i_NextState];
                            next_states[i_NextState] = 0;
                        }
                        delete this_action_ptr;
                        this_action_ptr = 0;
                    }
                }
            }
        }
    }
    
    this->StateLoaded = true;
    return;
}


//plot the policy map
void env_gridworld::plot_policy(unordered_map<string, string> &policy_table) {
    for(int j = 0; j < this->problem_map.size(); ++j) {
        for(int i = 0; i < this->problem_map[0].size(); ++i) {
            string key_code = "x" + to_string(i) + "y" + to_string(j);
            if(policy_table.find(key_code) != policy_table.end()) {
                cout << policy_table[key_code] << "\t";
            }
            else {
                cout << problem_map[j][i] << "\t";
            }
        }
        cout << endl;
    }
    return;
}

//plot the v value map
void env_gridworld::plot_V_values(unordered_map<string, double> &V_value_table) {
    for(int j = 0; j < this->problem_map.size(); ++j) {
        for(int i = 0; i < this->problem_map[0].size(); ++i) {
            string key_code = "x" + to_string(i) + "y" + to_string(j);
            if(V_value_table.find(key_code) != V_value_table.end()) {
                cout << V_value_table[key_code] << "\t";
            }
            else {
                cout << problem_map[j][i] << "\t";
            }
        }
        cout << endl;
    }
    return;
}

void env_gridworld::plot_Q_values(unordered_map<string, unordered_map<string, double>* > &Q_value_table){
    //not implemented yet
    return;
}

//see env_gridworld.h for description
vector<action_base*> env_gridworld::calc_LegalActions(state_base* state_ptr){
    vector<action_base*> vec;
    if(env_gridworld::is_terminal(state_ptr)) {
        vec.push_back(new action_grid("EXIT"));
        return vec;
    }
    vec.push_back(new action_grid("N"));
    vec.push_back(new action_grid("E"));
    vec.push_back(new action_grid("S"));
    vec.push_back(new action_grid("W"));
    return vec;
}

//see env_gridworld.h for description
vector<action_base*> env_gridworld::LegalActions(state_base* state_ptr){
    if(this->StateLoaded == false) {
        return calc_LegalActions(state_ptr);
    }
    
    vector<action_base*> result;
    string state_str = state_ptr->GetStringVal();
    if(this->TransitionMatrix.find(state_str) == this->TransitionMatrix.end()) {
        cout << "LegalActions: requested state not in transition matrix" << endl;
        return result;
    }
    for(auto iter_a = this->TransitionMatrix[state_str]->begin();
        iter_a != this->TransitionMatrix[state_str]->end(); ++iter_a) {
        result.push_back(new action_grid(iter_a->first));
    }
    return result;
}

//see env_gridworld.h for description
double env_gridworld::GetReward(state_base* state_ptr,
                                 action_base* Action_ptr,
                                 state_base* new_state_ptr) const {
    state_grid* State_Grid_ptr = dynamic_cast<state_grid*>(state_ptr);
    if(State_Grid_ptr == NULL) {
        cout << "wrong state pointer passed in.  state_grid* expected" << endl;
        return 0.0;
    }
    int StateY = State_Grid_ptr->GetPairVal().y;
    int StateX = State_Grid_ptr->GetPairVal().x;
    return rewards_map[StateY][StateX];
}

//see env_gridworld.h for description
vector<state_base*> env_gridworld::calc_NextState_state(state_base* state_ptr,
                                                  action_base* Action_ptr) {
    //double slide_prob = 0.5 - (this->prob_correct) / 2;    
    vector<state_base* > vec;
    
    state_grid* State_Grid_ptr = dynamic_cast<state_grid*>(state_ptr);
    if(State_Grid_ptr == NULL) {
        cout << "wrong state pointer passed in.  state_grid* expected" << endl;
        return vec;
    }
    int StateY = State_Grid_ptr->GetPairVal().y;
    int StateX = State_Grid_ptr->GetPairVal().x;
    
    action_grid* Action_Grid_ptr = dynamic_cast<action_grid*>(Action_ptr);
    if(Action_Grid_ptr == NULL) {
        cout << "wrong action pointer passed in.  action_grid* expected" << endl;
        return vec;
    }
    
    if(Action_Grid_ptr->GetStringVal() == "N") {
        //0.8 north
        if(StateY == 0) {
            vec.push_back(new state_grid(StateX, StateY));
        }
        else if (this->problem_map[StateY - 1][StateX] == "X"
                 || this->problem_map[StateY - 1][StateX] == "x") {
            vec.push_back(new state_grid(StateX, StateY));
        }
        else {
            vec.push_back(new state_grid(StateX, StateY - 1));
        }
        
        //0.1 west
        if(StateX == 0) {
            vec.push_back(new state_grid(StateX, StateY));
        }
        else if (this->problem_map[StateY][StateX - 1] == "X"
                 || this->problem_map[StateY][StateX - 1] == "x") {
            vec.push_back(new state_grid(StateX, StateY));
        }
        else {
            vec.push_back(new state_grid(StateX - 1, StateY));
        }
        
        //0.1 east
        if(StateX == this->problem_map[0].size() - 1) {
            vec.push_back(new state_grid(StateX, StateY));
        }
        else if (this->problem_map[StateY][StateX + 1] == "X"
                 || this->problem_map[StateY][StateX + 1] == "x") {
            vec.push_back(new state_grid(StateX, StateY));
        }
        else {
            vec.push_back(new state_grid(StateX + 1, StateY));
        }
    }
    else if(Action_Grid_ptr->GetStringVal() == "E") {
        //0.8 East
        if(StateX == this->problem_map[0].size() - 1) {
            vec.push_back(new state_grid(StateX, StateY));
        }
        else if (this->problem_map[StateY][StateX + 1] == "X"
                 || this->problem_map[StateY][StateX + 1] == "x") {
            vec.push_back(new state_grid(StateX, StateY));
        }
        else {
            vec.push_back(new state_grid(StateX + 1, StateY));
        }
        
        //0.1 north
        if(StateY == 0) {
            vec.push_back(new state_grid(StateX, StateY));
        }
        else if (this->problem_map[StateY - 1][StateX] == "X"
                 || this->problem_map[StateY - 1][StateX] == "x") {
            vec.push_back(new state_grid(StateX, StateY));
        }
        else {
            vec.push_back(new state_grid(StateX, StateY - 1));
        }
        
        //0.1 south
        if(StateY == this->problem_map.size() - 1) {
            vec.push_back(new state_grid(StateX, StateY));
        }
        else if (this->problem_map[StateY + 1][StateX] == "X"
                 || this->problem_map[StateY + 1][StateX] == "x") {
            vec.push_back(new state_grid(StateX, StateY));
        }
        else {
            vec.push_back(new state_grid(StateX, StateY + 1));
        }
    }
    else if(Action_Grid_ptr->GetStringVal() == "S") {
        //0.8 south
        if(StateY == this->problem_map.size() - 1) {
            vec.push_back(new state_grid(StateX, StateY));
        }
        else if (this->problem_map[StateY + 1][StateX] == "X"
                 || this->problem_map[StateY + 1][StateX] == "x") {
            vec.push_back(new state_grid(StateX, StateY));
        }
        else {
            vec.push_back(new state_grid(StateX, StateY + 1));
        }
        
        //0.1 west
        if(StateX == 0) {
            vec.push_back(new state_grid(StateX, StateY));
        }
        else if (this->problem_map[StateY][StateX - 1] == "X"
                 || this->problem_map[StateY][StateX - 1] == "x") {
            vec.push_back(new state_grid(StateX, StateY));
        }
        else {
            vec.push_back(new state_grid(StateX - 1, StateY));
        }
        
        //0.1 east
        if(StateX == this->problem_map[0].size() - 1) {
            vec.push_back(new state_grid(StateX, StateY));
        }
        else if (this->problem_map[StateY][StateX + 1] == "X"
                 || this->problem_map[StateY][StateX + 1] == "x") {
            vec.push_back(new state_grid(StateX, StateY));
        }
        else {
            vec.push_back(new state_grid(StateX + 1, StateY));
        }
    }
    else if(Action_Grid_ptr->GetStringVal() == "W") {
        //0.8 west
        if(StateX == 0) {
            vec.push_back(new state_grid(StateX, StateY));
        }
        else if (this->problem_map[StateY][StateX - 1] == "X"
                 || this->problem_map[StateY][StateX - 1] == "x") {
            vec.push_back(new state_grid(StateX, StateY));
        }
        else {
            vec.push_back(new state_grid(StateX - 1, StateY));
        }
        
        //0.1 north
        if(StateY == 0) {
            vec.push_back(new state_grid(StateX, StateY));
        }
        else if (this->problem_map[StateY - 1][StateX] == "X"
                 || this->problem_map[StateY - 1][StateX] == "x") {
            vec.push_back(new state_grid(StateX, StateY));
        }
        else {
            vec.push_back(new state_grid(StateX, StateY - 1));
        }
        
        //0.1 south
        if(StateY == this->problem_map.size() - 1) {
            vec.push_back(new state_grid(StateX, StateY));
        }
        else if (this->problem_map[StateY + 1][StateX] == "X"
                 || this->problem_map[StateY + 1][StateX] == "x") {
            vec.push_back(new state_grid(StateX, StateY));
        }
        else {
            vec.push_back(new state_grid(StateX, StateY + 1));
        }
    }
    else {
        cout << "illegal Act encountered.  Should be NESW" << endl;
    }
    
    return vec;
}

//see env_gridworld.h for description
vector<state_base*> env_gridworld::NextState_state(state_base* state_ptr,
                                         action_base* Action_ptr) {
    if(this->StateLoaded == false) {
        return calc_NextState_state(state_ptr, Action_ptr);
    }
    
    vector<state_base*> result;
    
    string this_state_str = state_ptr->GetStringVal();
    string this_action_str = Action_ptr->GetStringVal();
    
    if(this->TransitionMatrix.find(this_state_str) == this->TransitionMatrix.end()) {
        cout << "NextState_state: State not in transitionmatrix" << endl;
        return result;
    }
    if((this->TransitionMatrix[this_state_str])->find(this_action_str)
       == (this->TransitionMatrix[this_state_str])->end()) {
        cout << "NextState_state: Action not in transitionmatrix[state]" << endl;
        return result;
    }

    for(auto iter_TM = (*(this->TransitionMatrix[this_state_str]))[this_action_str]->begin();
        iter_TM != (*(this->TransitionMatrix[this_state_str]))[this_action_str]->end(); ++iter_TM) {
        result.push_back(this->State_table[iter_TM->first]->make_copy());
    }
    
    return result;
}

//see env_gridworld.h for description
vector<double> env_gridworld::calc_NextState_prob(state_base* state_ptr, action_base* Action_ptr) {
    vector<double> vec{this->prob_correct,
                    0.5 - (this->prob_correct) / 2,
                    0.5 - (this->prob_correct) / 2};
    return vec;
}

//see env_gridworld.h for description
vector<double> env_gridworld::NextState_prob(state_base* state_ptr, action_base* Action_ptr) {
    if(this->StateLoaded == false) {
        return calc_NextState_prob(state_ptr, Action_ptr);
    }
    vector<double> result;
    
    string this_state_str = state_ptr->GetStringVal();
    string this_action_str = Action_ptr->GetStringVal();
    
    if(this->TransitionMatrix.find(this_state_str) == this->TransitionMatrix.end()) {
        cout << "NextState_prob: State not in transitionmatrix" << endl;
        return result;
    }
    if((this->TransitionMatrix[this_state_str])->find(this_action_str)
       == (this->TransitionMatrix[this_state_str])->end()) {
        cout << "NextState_prob: Action not in transitionmatrix[state]" << endl;
        return result;
    }

    for(auto iter_TM = (*(this->TransitionMatrix[this_state_str]))[this_action_str]->begin();
        iter_TM != (*(this->TransitionMatrix[this_state_str]))[this_action_str]->end(); ++iter_TM) {
        result.push_back(iter_TM->second);
    }
    return result;
}

//see env_gridworld.h for description
state_base* env_gridworld::starting_state() {
    return new state_grid(0, 2);
}

bool env_gridworld::is_terminal(state_base* state_ptr){
    state_grid* State_Grid_ptr = dynamic_cast<state_grid*>(state_ptr);
    if(State_Grid_ptr == NULL) {
        cout << "wrong state pointer passed in.  state_grid* expected" << endl;
        return true;
    }
    if(problem_map[State_Grid_ptr->GetPairVal().y][State_Grid_ptr->GetPairVal().x] == "0") {
        return false;
    }
    return true;
}

unordered_map<string, state_base*>::iterator env_gridworld::State_table_begin_iter() {
    return this->State_table.begin();
}
unordered_map<string, state_base*>::iterator env_gridworld::State_table_end_iter() {
    return this->State_table.end();
}
