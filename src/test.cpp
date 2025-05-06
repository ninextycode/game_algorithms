#include "CFR.h"
#include "InfoSet.h"
#include "TicTacToeNode.h"
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
        const vector<double>& regrets = infoset->getInstantRegrets();
        
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
    int n_iterations = 6;
    
    auto t_start = chrono::high_resolution_clock::now();
    
    shared_ptr<TicTacToeNode> root_state = make_shared<TicTacToeNode>();
    CFR cfr(root_state);

    
    vector<RegretMetric> metrics;
    InfoSetMap infoset_map = cfr.getStrategyInfoSets();
    RegretMetric init_metric = calculateMetric(infoset_map);
    metrics.push_back(init_metric);


    cout << "Metrics (# regrets = " << metrics[0].n_regrets << "):" << endl;
    cout << "step, time, max_instant_regret, sum_positive_instant_regrets" << endl;

    cout << "Step 0" << ", "
        << 0 << "s, "
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
    }

    auto t_end = chrono::high_resolution_clock::now();
    auto total_time = chrono::duration_cast<chrono::milliseconds>(t_end - t_start).count() / 1000.0;
    
    cout << n_iterations << " iterations completed" << endl;
    cout << "Total time: " << total_time << "s" << endl;

    string filename = "tic_tac_toe_cpp_strategy.json";
    InfoSet::saveInfoSetMapStrategy(infoset_map, filename);
    cout << "Saved as " << filename << endl;
    return 0;
}