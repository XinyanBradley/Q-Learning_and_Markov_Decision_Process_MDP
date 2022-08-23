#include <iostream>
#include <unordered_map>
#include <iomanip>
//#include "env_gridworld.h"
#include "learningstrategy_mdp.h"
#include "learningstrategy_qlearning.h"
using namespace std;

int main(int argc, char const *argv[]) {
    vector<vector<string> > Map
    {
        {"0", "0", "0", "1"},
        {"0", "x", "0", "-1"},
        {"0", "0", "0", "0"},
    };
    
    double cost = -0.01;
    vector<vector<double> > rewards_map
    {
        {cost, cost, cost,    1},
        {cost,    0, cost,   -1},
        {cost, cost, cost, cost},
    };
    
    cout << setprecision(3);
    int max_iter;
    cout << "max iter:";
    cin >> max_iter;

    env_gridworld* environment = new env_gridworld(Map, rewards_map, 0.8);
    
    /*
    learningstrategy_mdp* MDP_ptr = new learningstrategy_mdp(environment,
                                                             0.95,   //discount
                                                             max_iter); //max iteration
    MDP_ptr->train_model();
    delete MDP_ptr;
    */
    
    
    learningstrategy_ql* QLearning_ptr
    = new learningstrategy_ql(environment,
                                0.95,   //discount
                                max_iter,    //max_iteration
                                1.0,    //epsilon
                                1.0);   //learningrate
    QLearning_ptr->train_model();
    delete QLearning_ptr;
     
    
    delete environment;
    
    return 0;
}
