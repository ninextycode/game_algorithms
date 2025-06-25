#include "cfr_experimental/CFRE.h"
#include <iostream>
#include <Utils.h>

template<InfoSetKey ISKey>
CFRE<ISKey>::CFRE(
    shared_ptr<const GameNode> root_node,
    int n_players,
    bool inital_evaluation_run,
    double e_soft_regsum_strategies,
    const InfoSetMap<ISKey>& initial_infosets
):
    root_node_(root_node),
    infosets_(initial_infosets),
    e_soft_regsum_strategies_(e_soft_regsum_strategies),
    n_players_(n_players)
{
    if (infosets_.empty()) {
        initInfoStates();
    }

    if (inital_evaluation_run) {
        evaluateRegretSum();
    }
}

template<InfoSetKey ISKey>
void CFRE<ISKey>::initInfoStates() {
    initInfoStatesRecursively(root_node_);
}

template<InfoSetKey ISKey>
void CFRE<ISKey>::initInfoStatesRecursively(const shared_ptr<const GameNode> node) {
    if (node->getType() == GameNode::Type::Terminal) {
        return;
    }

    const vector<int>& actions = node->getLegalActions();

    if (node->getType() == GameNode::Type::Decision) {
        ISKey infoset_key = getInfoSetKey(node);
        infosets_.try_emplace(infoset_key, actions.size());
    }

    for (int action : actions) {
        initInfoStatesRecursively(node->applyAction(action));
    }
}

template<InfoSetKey ISKey>
const InfoSetMap<ISKey>& CFRE<ISKey>::getStrategyInfoSets() {
    return infosets_;
}

template<InfoSetKey ISKey>
vector<double> CFRE<ISKey>::evaluateAndUpdateRegretSum(
    bool accumulate_regsum,
    bool accumulate_strategy
) {
    vector<double> initial_probabilities(n_players_, 1.0);
    return this->processNode(
        root_node_, initial_probabilities, 1.0, accumulate_regsum, accumulate_strategy
    );
}

template<InfoSetKey ISKey>
vector<double> CFRE<ISKey>::evaluateRegretSum() {
    bool accumulate_regsum = false;
    bool accumulate_strategy = false;
    vector<double> initial_probabilities(n_players_, 1.0);
    return this->processNode(
        root_node_, initial_probabilities, 1.0, accumulate_regsum, accumulate_strategy
    );
}

template<InfoSetKey ISKey>
vector<double> CFRE<ISKey>::processNode(
    const shared_ptr<const GameNode> node,
    const vector<double>& p_past_actions,
    double p_past_chances,
    bool accumulate_regsum,
    bool accumulate_strategy
) {
    switch (node->getType()) {
    case GameNode::Type::Decision:
        return processDecisionNode(
            node,
            p_past_actions,
            p_past_chances,
            accumulate_regsum,
            accumulate_strategy
        );
    
    case GameNode::Type::Chance:
        return processChanceNode(
            node,
            p_past_actions,
            p_past_chances,
            accumulate_regsum,
            accumulate_strategy
        );
    
    case GameNode::Type::Terminal:
        return processTerminalNode(node);
        
    default:
        throw logic_error("Unexpected type");

    }
}

/* 
    updates strategy for infoset of a given node
*/
template<InfoSetKey ISKey>
vector<double> CFRE<ISKey>::processDecisionNode(
    const shared_ptr<const GameNode> node,
    const vector<double>& p_past_actions,
    double p_past_chances,
    bool accumulate_regsum,
    bool accumulate_strategy
) {

    const vector<int>& available_actions = node->getLegalActions();
    int n_available_actions = available_actions.size();

    vector<vector<double>> action_utilities(
        n_available_actions, vector<double>(n_players_, 0.0)
    );
    
    const ISKey& infoset_key = getInfoSetKey(node);

    InfoSet& infoset = infosets_.at(infoset_key);
    
    // e_soft strategy to add weight to "impossible" events - experimental
    vector<double> regretsum_strategy = infoset.getRegretSumStrategy();
    if (e_soft_regsum_strategies_ > 0) {
        regretsum_strategy = \
            strategy_utils::epsilonSoftStrategy(
                e_soft_regsum_strategies_, 
                regretsum_strategy
            );
    }

    int current_player = node->getCurrentPlayer();

    for (int action_idx = 0; action_idx < n_available_actions; action_idx++) {
        vector<double> next_p_past_actions = p_past_actions;
        
        // Update probability for the current player's action
        next_p_past_actions[current_player] *= regretsum_strategy[action_idx];

        shared_ptr<const GameNode> next_node = \
            node->applyAction(available_actions[action_idx]);
        
        action_utilities[action_idx] = \
            processNode(
                next_node,
                next_p_past_actions,
                p_past_chances,
                accumulate_regsum,
                accumulate_strategy
            );
    }

    // recursively weighting over regretsum_strategy
    // unfolds into final results being sums over utility(end)
    // scaled by p(node->end) probabilities
    // Calculate expected utility under current strategy 
    // 
    // Do it for all players - this function only needs current player's value, 
    // but other recursive calls require us to evaluate all players to be used in previous calls 
    vector<double> regretsum_strategy_utility(n_players_, 0.0);
    for (int action_idx = 0; action_idx < n_available_actions; action_idx++) {
        for (int player = 0; player < n_players_; player++) {
            regretsum_strategy_utility[player] += \
                regretsum_strategy[action_idx] * action_utilities[action_idx][player];
        }
    }

    // Calculate and set instant regrets for each action
    for (int action_idx = 0; action_idx < n_available_actions; action_idx++) {
        double new_regret = (
            action_utilities[action_idx][current_player]
            - regretsum_strategy_utility[current_player]
        );
        infoset.setInstantRegret(action_idx, new_regret);
    }

    if (accumulate_regsum || accumulate_strategy) {
        // Calculate regret weight - probability excluding current player's past actions
        double regret_weight = p_past_chances;
        for (int player = 0; player < n_players_; player++) {
            if (player != current_player) {
                regret_weight *= p_past_actions[player];
            }
        }
        
        // Strategy weight, conversely, is the current player's past action probability
        double cum_strategy_weight = p_past_actions[current_player];

        if (accumulate_regsum) {
            infoset.accumulateRegret(regret_weight);
        }
        if (accumulate_strategy) {
            // use non-soft strategy here to accumulate the correct final strategy
            // softness is used to achieve non-zero regrets for "impossible" events
            // but it should be excluded from the final result 
            infoset.accumulateStrategy(cum_strategy_weight);
        }
    }

    return regretsum_strategy_utility;
}


template<InfoSetKey ISKey>
vector<double> CFRE<ISKey>::processChanceNode(
    const shared_ptr<const GameNode> node,
    const vector<double>& p_past_actions,
    double p_past_chances,
    bool accumulate_regsum,
    bool accumulate_strategy
) {
    const vector<int>& available_actions = node->getLegalActions();
    int n_available_actions = available_actions.size();

    vector<vector<double>> action_utilities(
        n_available_actions, vector<double>(n_players_, 0.0)
    );
    vector<double> chance_probs = node->getChanceProbabilities();

    for (int action_idx = 0; action_idx < n_available_actions; action_idx++) {
        shared_ptr<const GameNode> next_node = \
            node->applyAction(available_actions[action_idx]);
        action_utilities[action_idx] = \
            processNode(
                next_node,
                p_past_actions,
                p_past_chances * chance_probs[action_idx],
                accumulate_regsum,
                accumulate_strategy
            );
    }

    vector<double> chance_node_utility(n_players_, 0.0);

    for (int action_idx = 0; action_idx < n_available_actions; action_idx++) {
        for (int player = 0; player < n_players_; player++) {
            chance_node_utility[player] += \
                chance_probs[action_idx] * action_utilities[action_idx][player];
        }
    }
    return chance_node_utility;  
}


template<InfoSetKey ISKey>
vector<double> CFRE<ISKey>::processTerminalNode(
    const shared_ptr<const GameNode> node
) {
    return node->getTerminalUtilities();
}

// Explicit instantiation definitions - this generates the actual code
template class CFRE<string>;
template class CFRE<size_t>;
