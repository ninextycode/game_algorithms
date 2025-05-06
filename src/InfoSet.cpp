#include "InfoSet.h"
#include <fstream>


void InfoSet::saveInfoSetMapStrategy(
    const InfoSetMap& infoset_map, 
    const string& filename
) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }

    file << "{" << std::endl;
    
    bool firstEntry = true;
    for (const auto& entry : infoset_map) {
        if (!firstEntry) {
            file << "," << std::endl;
        }
        firstEntry = false;

        const auto& key = entry.first;
        const auto& infoset = entry.second;
        
        // Escape the key for JSON
        std::string escaped_key = key;
        size_t pos = 0;

        // Replace newlines with escaped newlines
        pos = 0;
        while ((pos = escaped_key.find("\n", pos)) != std::string::npos) {
            escaped_key.replace(pos, 1, "\\n");
            pos += 2;
        }
        
        file << "  \"" << escaped_key << "\": [";
        
        const auto& strategy = infoset->getCumulativeStrategy();
        for (size_t i = 0; i < strategy.size(); ++i) {
            if (i > 0) {
                file << ", ";
            }
            file << strategy[i];
        }
        
        file << "]";
    }
    
    file << std::endl << "}" << std::endl;
    file.close();
}

InfoSet::InfoSet(int n_actions) 
    : instant_regrets(n_actions, 0.0),
      regret_sum(n_actions, 0.0), 
      regret_sum_strategy_updated(false),
      regret_sum_strategy(n_actions, 0.0),
      cumulative_strategy_raw(n_actions, 0.0) {
}

InfoSet::InfoSet(const InfoSet& other)
    : instant_regrets(other.instant_regrets),
      regret_sum(other.regret_sum),
      regret_sum_strategy_updated(other.regret_sum_strategy_updated),
      regret_sum_strategy(other.regret_sum_strategy),
      cumulative_strategy_raw(other.cumulative_strategy_raw) {
}

void InfoSet::setInstantRegret(int action_idx, double regret) {
    instant_regrets[action_idx] = regret;
}

void InfoSet::accumulateRegrets(double weight) {
    for (size_t action_idx = 0; action_idx < instant_regrets.size(); action_idx++) {
        this->regret_sum[action_idx] += weight * instant_regrets[action_idx];
    }
    regret_sum_strategy_updated = false;
}

void InfoSet::accumulateStrategy(double weight) {
    const vector<double>& strategy = getRegretSumStrategy();
    for (size_t i = 0; i < strategy.size(); i++) {
        cumulative_strategy_raw[i] += weight * strategy[i];
    }
}

const vector<double>& InfoSet::getInstantRegrets() const {
    return instant_regrets;
}

const vector<double>& InfoSet::getRegretSum() const {
    return regret_sum;
}

// Static helper function to normalize strategies
vector<double> InfoSet::normalizeStrategy(const vector<double>& strategy) {
    vector<double> normalized = strategy;

    // First pass: keep only positive values
    vector<double> positive_regrets(normalized.size(), 0.0);
    for (size_t i = 0; i < normalized.size(); i++) {
        normalized[i] = normalized[i] > 0 ? normalized[i] : 0;
    }

    double sum = 0.0;
    for (size_t i = 0; i < strategy.size(); i++) {
        sum += strategy[i];
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
vector<double> InfoSet::epsilonSoftStrategy(
    double epsilon, const vector<double>& strategy
) {
    vector<double> softened_strategy = strategy;
    size_t n = strategy.size();
    
    // Reserve at least epsilon for all random actions
    double remaining_prob = 1.0 - (n * epsilon);
    
    for (size_t i = 0; i < n; ++i) {
        // Each action gets epsilon + proportional share of remaining probability
        softened_strategy[i] = epsilon + (remaining_prob * strategy[i]);
    }
    
    return softened_strategy;
}

const vector<double>& InfoSet::getRegretSumStrategy() {
    if (regret_sum_strategy_updated) {
        return regret_sum_strategy;
    }

    // Normalize the strategy
    regret_sum_strategy = normalizeStrategy(regret_sum);
    // regret_sum_strategy = epsilonSoftStrategy(0.05, regret_sum_strategy);
    regret_sum_strategy_updated = true;

    return regret_sum_strategy;
}

vector<double> InfoSet::getCumulativeStrategy() {
    // Create a copy, normalize it, and update the original
    vector<double> cumulative_strategy_normalized =\
        normalizeStrategy(cumulative_strategy_raw);
    return cumulative_strategy_normalized;
}


void InfoSetNonnegativeRegret::accumulateRegrets(double weight) {
    // Call superclass method first
    InfoSet::accumulateRegrets(weight);

    // Ensure all regret sums are non-negative
    for (size_t i = 0; i < regret_sum.size(); i++) {
        regret_sum[i] = max(0.0, regret_sum[i]);
    }
}
