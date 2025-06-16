#include "Utils.h"
#include <cxxabi.h>
#include <cstdlib>


string cpp_utils::demangle(const char* mangled) {
    int status = 0;
    char* dem = abi::__cxa_demangle(mangled, nullptr, nullptr, &status);
    string out = (status == 0 && dem) ? dem : mangled;
    free(dem);
    return out;
}


// Static helper function to normalize strategies
vector<double> strategy_utils::normalizeStrategy(const vector<double>& strategy) {
    vector<double> normalized = strategy;

    for (size_t i = 0; i < normalized.size(); i++) {
        normalized[i] = max(0.0, normalized[i]);
    }

    double sum = 0.0;
    for (size_t i = 0; i < normalized.size(); i++) {
        sum += normalized[i];
    }
    
    // If sum is positive, normalize the strategy
    if (sum > 0) {
        for (size_t i = 0; i < normalized.size(); i++) {
            normalized[i] /= sum;
        }
    } else {
        // Use uniform strategy if sum is zero
        double uniformProb = 1.0 / normalized.size();
        std::fill(normalized.begin(), normalized.end(), uniformProb);
    }
    
    return normalized;
}


vector<double> strategy_utils::epsilonSoftStrategy(
    double epsilon, const vector<double>& strategy
) {
    vector<double> softened_strategy = strategy;
    size_t n = strategy.size();
    
    // Reserve at least epsilon for all random actions
    double remaining_prob = 1.0 - epsilon;
    
    for (size_t i = 0; i < n; ++i) {
        // Each action gets epsilon / n + proportional share of remaining probability
        softened_strategy[i] = epsilon / n + (remaining_prob * strategy[i]);
    }
    
    return softened_strategy;
}
