#pragma once
#include "abstract/nodes/GameNode.h"
#include "abstract/infoset/InfoSet.h"
#include "abstract/infoset/InfoSetMap.h"
#include <unordered_map>
#include <memory>
#include <vector>

using namespace std;

template<typename ISKey = string>
class CFRPlus {
public:
    class Builder {
    public:
        Builder& setRootNode(shared_ptr<const GameNode> root_node);
        Builder& setInitialEvaluationRun(bool initial_evaluation_run);
        Builder& setESoftRegsumStrategies(double e_soft_regsum_strategies);
        Builder& setInitialState(const InfoSetMap<ISKey>& initial_state);
        CFRPlus buildCfr();
        
    private:
        shared_ptr<const GameNode> root_node_;
        bool initial_evaluation_run_ = true;
        double e_soft_regsum_strategies_ = 0;
        InfoSetMap<ISKey> initial_state_ = InfoSetMap<ISKey>();
    };

    CFRPlus(
        shared_ptr<const GameNode> root_node,
        bool initial_evaluation_run = true,
        double e_soft_regsum_strategies = 0,
        const InfoSetMap<ISKey>& initial_state = InfoSetMap<ISKey>()
    );
    

    // returns game utility at the root node for player 0 
    // if playing regretsum-based strategy
    // accumulates regrets in infosets
    // (!) Note: evaluation is node using regretsum strategies, not cumulative strategy
    double evaluateAndUpdateRegretSum(
        bool accumulate_regsum = true,
        bool accumulate_strategy = true
    );

    // does not accumulate regrets in infosets
    // (!) Note: evaluation is node using regretsum strategies, not cumulative strategy
    double evaluateRegretSum();
    
    const InfoSetMap<ISKey>& getStrategyInfoSets();
    
    // run with accumulate flags to control regret and strategy accumulation
    // and get node value for player 0
    double processNode(
        const shared_ptr<const GameNode> node,
        double p_past_actions_p0 = 1,
        double p_past_actions_p1 = 1,
        double p_past_chances = 1,
        bool accumulate_regsum = false,
        bool accumulate_strategy = false
    );
    
private:    
    double processDecisionNode(
        const shared_ptr<const GameNode> node,
        double p_past_actions_p0,
        double p_past_actions_p1,
        double p_past_chances,
        bool accumulate_regsum,
        bool accumulate_strategy
    );

    double processChanceNode(
        const shared_ptr<const GameNode> node,
        double p_past_actions_p0,
        double p_past_actions_p1,
        double p_past_chances,
        bool accumulate_regsum,
        bool accumulate_strategy
    );

    ISKey getInfoSetKey(shared_ptr<const GameNode> node) {
        return node->getInfoSetKey<ISKey>();
    }

    // simply returns utility for player 0 
    // player 1 is assumed to have the same utility with a different sign
    double processTerminalNode(
        const shared_ptr<const GameNode> node
    );

    void initInfoStates();
    void initInfoStatesRecursively(const shared_ptr<const GameNode> node);
    
    const shared_ptr<const GameNode> root_node_;
    InfoSetMap<ISKey> infosets_;
    double e_soft_regsum_strategies_;
};

// Explicit instantiation declarations
extern template class CFRPlus<string>;
extern template class CFRPlus<size_t>;

// Type aliases for convenience
using CFRPlusString = CFRPlus<string>;
using CFRPlusInt = CFRPlus<size_t>;