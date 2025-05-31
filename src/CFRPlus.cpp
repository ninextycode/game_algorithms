#include "CFRPlus.h"
#include <iostream>


CFRPlus::CFRPlus(
    shared_ptr<const GameNode> root_node,
    const InfoSetMap& initial_infosets, 
    bool inital_evaluation_run,
    double e_soft_regsum_strategies
):
    root_node_(root_node),
    infosets_(initial_infosets),
    e_soft_regsum_strategies_(e_soft_regsum_strategies)
{
    if (inital_evaluation_run) {
        evaluateAndUpdateFromNode(
            root_node, 1, 1, 1, true
        );
    }
}

CFRPlus::CFRPlus(
    shared_ptr<const GameNode> root_node, 
    bool inital_evaluation_run,
    double e_soft_regsum_strategies
):
    root_node_(root_node),
    infosets_(),
    e_soft_regsum_strategies_(e_soft_regsum_strategies)
{
    initInfoStates();

    if (inital_evaluation_run) {
        evaluateAndUpdateFromNode(
            root_node, 1, 1, 1, true
        );
    }
}

void CFRPlus::initInfoStates() {
    initInfoStatesRecursively(root_node_);
}

void CFRPlus::initInfoStatesRecursively(const shared_ptr<const GameNode> node) {
    if (node->isTerminal()) {
        return;
    }

    const vector<int>& actions = node->getAvailableActions();

    if (!node->isChance()) {
        const string& infoset_key = node->getInfoSetKey();
        infosets_.emplace(infoset_key, createInfoSet(actions.size()));
    }

    for (int action : actions) {
        initInfoStatesRecursively(node->nextGameNode(action));
    }
}


InfoSetMap CFRPlus::getStrategyInfoSets() {
    InfoSetMap infosets_copy;
    for (const auto& [key, infoset] : infosets_) {
        infosets_copy[key] = make_shared<InfoSet>(*infoset);
    }
    return infosets_copy;
}


double CFRPlus::evaluateAndUpdateAll() {
    return this->evaluateAndUpdateFromNode(
        root_node_, 1, 1, 1, false
    );
}

/* 
    updates  strategy for inforset of a given node
    returns utility at a given node for player 0
*/
double CFRPlus::evaluateAndUpdateFromNode(
    const shared_ptr<const GameNode> node,
    double p_past_actions_p0,
    double p_past_actions_p1,
    double p_past_chances,
    bool no_accumulate
) {
    if (node->isTerminal()) {
        return node->getUtilitiesForTerminal()[0];
    }

    if (node->isChance()) {
        return evaluateAndUpdateFromChanceNode(
            node,
            p_past_actions_p0,
            p_past_actions_p1,
            p_past_chances,
            no_accumulate
        );
    }

    const vector<int>& available_actions = \
        node->getAvailableActions();
    int n_available_actions = \
        node->getAvailableActions().size();
    vector<double> action_utilities(n_available_actions, 0);
    
    const string& infoset_key = node->getInfoSetKey();

    shared_ptr<InfoSet> infoset = infosets_.at(infoset_key);
    
    // e_soft strategy to add weight to "impossible" events - experimental
    const vector<double> regretsum_strategy = \
        infoset->getRegretSumStrategy(e_soft_regsum_strategies_);

    for (int action_idx = 0; action_idx < n_available_actions; action_idx++) {
        double next_p_past_actions_p0 = p_past_actions_p0;
        double next_p_past_actions_p1 = p_past_actions_p1;

        if (node->getCurrentPlayer() == 0) {
            next_p_past_actions_p0 *= regretsum_strategy[action_idx];
        }
        if (node->getCurrentPlayer() == 1) {
            next_p_past_actions_p1 *= regretsum_strategy[action_idx];
        }

        shared_ptr<GameNode> next_node = \
            node->nextGameNode(available_actions[action_idx]);
        
        action_utilities[action_idx] = \
            evaluateAndUpdateFromNode(
                next_node,
                next_p_past_actions_p0,
                next_p_past_actions_p1,
                p_past_chances,
                no_accumulate
            );
    }

    // recursively weighting over regretsum_strategy
    // unfolds into final results being sums over utility(end)
    // scaled by p(node->end) probabilities
    double regretsum_strategy_utility = 0;
    for (int action_idx = 0; action_idx < n_available_actions; action_idx++) {
        regretsum_strategy_utility += \
            regretsum_strategy[action_idx] * action_utilities[action_idx];
    }

    for (int action_idx = 0; action_idx < n_available_actions; action_idx++) {
        double new_regret = (
            action_utilities[action_idx] - regretsum_strategy_utility
        );
        if (node->getCurrentPlayer() == 1) {
            // default returned value is for player 0
            // invert value for player 1
            new_regret = -new_regret;
        }
        infoset->setInstantRegret(action_idx, new_regret);
    }

    if (!no_accumulate) {
        double regret_weight = 0;
        double cum_strategy_weight = 0;
        // regret weight takes probability excluding current player's past  actions
        // cum strategy weight, conversely, takes current player's actions probabilities
        if (node->getCurrentPlayer() == 0) {
            regret_weight = p_past_chances * p_past_actions_p1;
            cum_strategy_weight = p_past_actions_p0;
        }
        if (node->getCurrentPlayer() == 1) {
            regret_weight = p_past_chances * p_past_actions_p0;
            cum_strategy_weight = p_past_actions_p1;
        }

        infoset->accumulateRegret(regret_weight);
        // use non-soft strategy here to accumulate the correct final strategy
        // softness is used to achieve non-zero regrets for "impossible" events
        // but it should be excluded from the final result 
        infoset->accumulateStrategy(cum_strategy_weight, 0);
    }

    return regretsum_strategy_utility;
}

double CFRPlus::evaluateAndUpdateFromChanceNode(
    const shared_ptr<const GameNode> node,
    double p_past_actions_p0,
    double p_past_actions_p1,
    double p_past_chances,
    bool no_accumulate
) {
    const vector<int>& available_actions = \
        node->getAvailableActions();
    int n_available_actions = \
        node->getAvailableActions().size();

    vector<double> action_utilities(n_available_actions, 0);
    vector<double> chance_probs = node->getChanceProbabilities();

    for (int action_idx = 0; action_idx < n_available_actions; action_idx++) {
        shared_ptr<GameNode> next_node = \
            node->nextGameNode(available_actions[action_idx]);
        action_utilities[action_idx] = \
            evaluateAndUpdateFromNode(
                next_node,
                p_past_actions_p0,
                p_past_actions_p1,
                p_past_chances * chance_probs[action_idx],
                no_accumulate
            );
    }

    double chance_node_utility = 0;

    for (int action_idx = 0; action_idx < n_available_actions; action_idx++) {
        chance_node_utility += \
            chance_probs[action_idx] * action_utilities[action_idx];
    }
    return chance_node_utility;   
}

shared_ptr<InfoSet> CFRPlus::createInfoSet(int n_actions) {
    return make_shared<InfoSet>(
        n_actions
    );
}