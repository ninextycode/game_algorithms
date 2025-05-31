#include "CFRPlus.h"
#include "InfoSet.h"
#include "tictactoe/TicTacToeNode.h"
#include "tictactoe/TTTInvariant.h"
#include "Randomizer.h"
#include "memory"
#include <iostream>
#include <string>
#include <chrono>

using namespace std;

struct RegretMetric {
    int n_regrets;
    double max_instant_regret;
    double sum_positive_instant_regrets;
};

RegretMetric calculateMetric(const InfoSetMap& infoset_map) {
    RegretMetric metric = {0, 0.0, 0.0};

    for (const auto& pair : infoset_map) {
        const auto& infoset = pair.second;
        const vector<double>& regrets = infoset->getInstantRegret();
        
        for (const double& regret : regrets) {
            metric.n_regrets++;
            
            if (regret > metric.max_instant_regret) {
                metric.max_instant_regret = regret;
            }
            
            if (regret > 0) {
                metric.sum_positive_instant_regrets += regret;
            }
        }
    }

    return metric;
}


int main(int argc, char **argv) {
    int n_iterations = 5; // Default value
    
    // Parse command line arguments if provided
    if (argc > 1) {
        try {
            n_iterations = std::stoi(argv[1]);
        } catch (const std::exception& e) {
            std::cerr << "Error parsing iterations parameter: " << e.what() << std::endl;
            std::cerr << "Using default value of " << n_iterations << std::endl;
        }
    }
    
    auto t_start = chrono::high_resolution_clock::now();
    
    // shared_ptr<RandomTTTDecisionNode> root_state = make_shared<RandomTTTDecisionNode>(
    //     0.10
    // );
    
    shared_ptr<TTTInvariant> root_state = make_shared<TTTInvariant>();

    // shared_ptr<GameNode> root_state = make_shared<RandomizedDecisionNode>(
    //     make_shared<TTTInvariant>(), 0.2, 2
    // );
    string folder = "training_output_hard";
    string strategy_filename_prefix = folder + "/" + "tic_tac_toe_cpp_strategy";
    string regretsum_filename_prefix = folder + "/" + "tic_tac_toe_cpp_regretsum";
    string instant_regret_filename_prefix = folder + "/" + "tic_tac_toe_cpp_instant_regret";
    
    // Derive final filenames
    string strategy_filename = strategy_filename_prefix + ".json";
    string regretsum_filename = regretsum_filename_prefix + ".json";

    InfoSetMap initial_state;
    bool loaded_state = false;
    
    try {
        initial_state = InfoSet::loadInfoSetMap(regretsum_filename, strategy_filename);
        cout << "Loaded from " << regretsum_filename << endl;
        loaded_state = true;
    } catch (const std::exception& e) {
        cout << "Could not load regretsum file: " << e.what() << endl;
        cout << "Starting with empty initial state" << endl;
    }

    bool initial_evaluation_run = false;
    double e_soft = 0.0; // hard

    auto cfr_start = chrono::high_resolution_clock::now();
    cout << "CFR constructor started" << endl;
    CFRPlus cfr = loaded_state ? 
        CFRPlus(root_state, initial_state, initial_evaluation_run, e_soft) :
        CFRPlus(root_state, initial_evaluation_run, e_soft);
    auto cfr_end = chrono::high_resolution_clock::now();
    double cfr_time = chrono::duration_cast<chrono::milliseconds>(cfr_end - cfr_start).count() / 1000.0;
    
    vector<RegretMetric> metrics;
    InfoSetMap infoset_map = cfr.getStrategyInfoSets();
    RegretMetric init_metric = calculateMetric(infoset_map);
    metrics.push_back(init_metric);

    cout << "Metrics (# regrets = " << metrics[0].n_regrets << "):" << endl;
    cout << "step, time, max_instant_regret, sum_positive_instant_regrets" << endl;

    cout << "Step 0" << ", "
        << cfr_time << "s, "
        << init_metric.max_instant_regret << ", "
        << init_metric.sum_positive_instant_regrets
        << endl; 

    for (int i = 0; i < n_iterations; i++) {
        auto t0 = chrono::high_resolution_clock::now();
        cfr.evaluateAndUpdateAll();
        auto t1 = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(t1 - t0).count() / 1000.0;
        
        infoset_map = std::move(cfr.getStrategyInfoSets());
        RegretMetric step_metric = calculateMetric(infoset_map);
        metrics.push_back(step_metric);

        cout << "Step " << i + 1 << ", "
            << duration << "s, "
            << step_metric.max_instant_regret << ", "
            << step_metric.sum_positive_instant_regrets
            << endl; 
            
        // Save strategy and regretsum after each iteration using prefixes
        string step_strategy_filename = strategy_filename_prefix + "_step_" + to_string(i+1) + ".json";
        string step_regretsum_filename = regretsum_filename_prefix + "_step_" + to_string(i+1) + ".json";
        string step_instant_regret_filename = instant_regret_filename_prefix + "_step_" + to_string(i+1) + ".json";
        
        InfoSet::saveInfoSetMapStrategy(infoset_map, step_strategy_filename);
        cout << "Saved strategy as " << step_strategy_filename << endl;
        InfoSet::saveInfoSetMapRegretSum(infoset_map, step_regretsum_filename);
        cout << "Saved regretsum as " << step_regretsum_filename << endl;
        InfoSet::saveInfoSetMapInstantRegret(infoset_map, step_instant_regret_filename);
        cout << "Saved instant regret as " << step_instant_regret_filename << endl;
    }

    auto t_end = chrono::high_resolution_clock::now();
    auto total_time = chrono::duration_cast<chrono::milliseconds>(t_end - t_start).count() / 1000.0;
    
    cout << n_iterations << " iterations completed" << endl;
    cout << "Total time: " << total_time << "s" << endl;

    // Also save final versions with the original filenames using prefixes
    InfoSet::saveInfoSetMapStrategy(infoset_map, strategy_filename);
    cout << "Saved final strategy as " << strategy_filename << endl;
    InfoSet::saveInfoSetMapRegretSum(infoset_map, regretsum_filename);
    cout << "Saved final regretsum as " << regretsum_filename << endl;
    return 0;
}