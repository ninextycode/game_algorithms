#pragma once
#include "abstract/nodes/game_node.h"
#include "abstract/regret_data/regret_data.h"
#include "abstract/regret_data/regret_data_map.h"
#include <unordered_map>
#include <memory>
#include <vector>

using namespace std;

template<RegretDataKey RKey = string>
class CFRE {
public:
    CFRE(
        shared_ptr<const GameNode> root_node,
        int n_players,
        bool initial_evaluation_run = true,
        double e_soft_regsum_strategies = 0,
        const RegretDataMap<RKey>& initial_state = RegretDataMap<RKey>()
    );

    // returns game utilities at the root node for all players
    // if playing regretsum-based strategy
    // accumulates regrets in infosets
    // (!) Note: evaluation is using regretsum strategies, not cumulative strategy
    vector<double> evaluateAndUpdateRegretSum(
        bool accumulate_regsum = true,
        bool accumulate_strategy = true
    );

    // does not accumulate regrets in infosets
    // (!) Note: evaluation is using regretsum strategies, not cumulative strategy
    vector<double> evaluateRegretSum();
    
    const RegretDataMap<RKey>& getStrategyInfoSets();
    
    vector<double> processNode(
        const shared_ptr<const GameNode> node,
        const vector<double>& p_past_actions,
        double p_past_chances = 1.0,
        bool accumulate_regsum = false,
        bool accumulate_strategy = false
    );
    
private:    
    vector<double> processDecisionNode(
        const shared_ptr<const GameNode> node,
        const vector<double>& p_past_actions,
        double p_past_chances,
        bool accumulate_regsum,
        bool accumulate_strategy
    );

    vector<double> processChanceNode(
        const shared_ptr<const GameNode> node,
        const vector<double>& p_past_actions,
        double p_past_chances,
        bool accumulate_regsum,
        bool accumulate_strategy
    );

    RKey getInfoSetKey(shared_ptr<const GameNode> node) {
        return node->getInfoSetKey<RKey>();
    }

    // returns utilities for all players
    vector<double> processTerminalNode(
        const shared_ptr<const GameNode> node
    );

    void initInfoStates();
    void initInfoStatesRecursively(const shared_ptr<const GameNode> node);
    
    const shared_ptr<const GameNode> root_node_;
    RegretDataMap<RKey> infosets_;
    double e_soft_regsum_strategies_;
    int n_players_;
};

// Explicit instantiation declarations
extern template class CFRE<string>;
extern template class CFRE<size_t>;

// Type aliases for convenience
using CFREString = CFRE<string>;
using CFREInt = CFRE<size_t>;