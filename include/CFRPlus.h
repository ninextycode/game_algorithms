#pragma once

#include <vector>
#include <unordered_map>
#include <GameNode.h>
#include <InfoSet.h>
#include <memory>

using namespace std;


class CFRPlus {
public:
    CFRPlus(
        shared_ptr<const GameNode> root_node,
        const InfoSetMap& initial_state,
        bool initial_evaluation_run,
        double e_soft_regsum_strategies=0
    );

    CFRPlus(
        shared_ptr<const GameNode> root_node,
        bool initial_evaluation_run,
        double e_soft_regsum_strategies=0
    );
    
    // returns game utility at the root node for player 0 
    // if playing regretsum-based strategy
    double evaluateAndUpdateAll();
    
    InfoSetMap getStrategyInfoSets();

protected:
    virtual shared_ptr<InfoSet> createInfoSet(int n_actions);

private:
    // run with no_accumulate to initialise instantaneous regrets only
    double evaluateAndUpdateFromNode(
        const shared_ptr<const GameNode> node,
        double p_past_actions_p0,
        double p_past_actions_p1,
        double p_past_chances,
        bool no_accumulate
    );

    double evaluateAndUpdateFromChanceNode(
        const shared_ptr<const GameNode> node,
        double p_past_actions_p0,
        double p_past_actions_p1,
        double p_past_chances,
        bool no_accumulate
    );

    void initInfoStates();
    void initInfoStatesRecursively(const shared_ptr<const GameNode> node);
    
    const shared_ptr<const GameNode> root_node_;
    InfoSetMap infosets_;
    double e_soft_regsum_strategies_;
};