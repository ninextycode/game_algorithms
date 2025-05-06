#include "CFR.h"
#include <iostream>


CFR::CFR(shared_ptr<const GameNode> root_node):
    root_node(root_node)
{
    initInfoStates();

    evaluateAndUpdateFromNode(
        root_node, 1, 1, 1, true
    );
}

void CFR::initInfoStates() {
    initInfoStatesRecursively(root_node);
}

void CFR::initInfoStatesRecursively(const shared_ptr<const GameNode> node) {
    if (node->isTerminal()) {
        return;
    }

    const vector<int>& actions = node->getAvailableActions();

    if (!node->isChance()) {
        const string& infoset_key = node->getInfoSetKey();
        infosets.emplace(infoset_key, createInfoSet(actions.size()));
    }

    for (int action : actions) {
        initInfoStatesRecursively(node->nextGameNode(action));
    }
}


InfoSetMap CFR::getStrategyInfoSets() {
    InfoSetMap infosets_copy;
    for (const auto& [key, infoset] : infosets) {
        infosets_copy[key] = make_shared<InfoSet>(*infoset);
    }
    return infosets_copy;
}


double CFR::evaluateAndUpdateAll() {
    return this->evaluateAndUpdateFromNode(
        root_node, 1, 1, 1, false
    );
}

/* 
    updates  strategy for inforset of a given node
    returns utility at a given node for player 0
*/
double CFR::evaluateAndUpdateFromNode(
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

    shared_ptr<InfoSet> infoset = infosets.at(infoset_key);
    
    const vector<double> regretsum_strategy = \
        infoset->getRegretSumStrategy();
        
    // cout << "infoset_key " << endl << infoset_key << endl;
    // cout << "regretsum_strategy [";
    // for (size_t i = 0; i < regretsum_strategy.size(); ++i) {
    //     cout << regretsum_strategy[i];
    //     if (i < regretsum_strategy.size() - 1) cout << ", ";
    // }
    // cout << "]" << endl << endl;

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

        infoset->accumulateRegrets(regret_weight);
        infoset->accumulateStrategy(cum_strategy_weight);
    }

    return regretsum_strategy_utility;
}

double CFR::evaluateAndUpdateFromChanceNode(
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

shared_ptr<InfoSet> CFR::createInfoSet(int n_actions) {
    return make_shared<InfoSet>(
        n_actions
    );
}

shared_ptr<InfoSet> CFRPlus::createInfoSet(int n_actions) {
    return make_shared<InfoSetNonnegativeRegret>(
        n_actions
    );
}