#include "cfr/CFRPlus.h"
#include <iostream>
#include <Utils.h>

template<typename ISKey>
typename CFRPlus<ISKey>::Builder& CFRPlus<ISKey>::Builder::setRootNode(
    shared_ptr<const GameNode> root_node
) {
    root_node_ = root_node;
    return *this;
}

template<typename ISKey>
typename CFRPlus<ISKey>::Builder& CFRPlus<ISKey>::Builder::setInitialEvaluationRun(
    bool initial_evaluation_run
) {
    initial_evaluation_run_ = initial_evaluation_run;
    return *this;
}

template<typename ISKey>
typename CFRPlus<ISKey>::Builder& CFRPlus<ISKey>::Builder::setESoftRegsumStrategies(
    double e_soft_regsum_strategies
) {
    e_soft_regsum_strategies_ = e_soft_regsum_strategies;
    return *this;
}

template<typename ISKey>
typename CFRPlus<ISKey>::Builder& CFRPlus<ISKey>::Builder::setInitialState(
    const InfoSetMap<ISKey>& initial_state
) {
    initial_state_ = initial_state;
    return *this;
}

template<typename ISKey>
CFRPlus<ISKey> CFRPlus<ISKey>::Builder::buildCfr() {
    if (!root_node_) {
        throw std::invalid_argument("Root node cannot be null");
    }
    return CFRPlus<ISKey>(
        root_node_,
        initial_evaluation_run_,
        e_soft_regsum_strategies_,
        initial_state_
    );
}

template<typename ISKey>
CFRPlus<ISKey>::CFRPlus(
    shared_ptr<const GameNode> root_node,
    bool inital_evaluation_run,
    double e_soft_regsum_strategies,
    const InfoSetMap<ISKey>& initial_infosets
):
    root_node_(root_node),
    infosets_(initial_infosets),
    e_soft_regsum_strategies_(e_soft_regsum_strategies)
{
    if (infosets_.empty()) {
        initInfoStates();
    }

    if (inital_evaluation_run) {
        evaluateRegretSum();
    }
}

template<typename ISKey>
void CFRPlus<ISKey>::initInfoStates() {
    initInfoStatesRecursively(root_node_);
}

template<typename ISKey>
void CFRPlus<ISKey>::initInfoStatesRecursively(const shared_ptr<const GameNode> node) {
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

template<typename ISKey>
const InfoSetMap<ISKey>& CFRPlus<ISKey>::getStrategyInfoSets() {
    return infosets_;
}

template<typename ISKey>
double CFRPlus<ISKey>::evaluateAndUpdateRegretSum(
    bool accumulate_regsum,
    bool accumulate_strategy
) {
    return this->processNode(
        root_node_, 1, 1, 1, accumulate_regsum, accumulate_strategy
    );
}

template<typename ISKey>
double CFRPlus<ISKey>::evaluateRegretSum() {
    bool accumulate_regsum = false;
    bool accumulate_strategy = false;
    return this->processNode(
        root_node_, 1, 1, 1, accumulate_regsum, accumulate_strategy
    );
}

template<typename ISKey>
double CFRPlus<ISKey>::processNode(
    const shared_ptr<const GameNode> node,
    double p_past_actions_p0,
    double p_past_actions_p1,
    double p_past_chances,
    bool accumulate_regsum,
    bool accumulate_strategy
) {
    switch (node->getType()) {
    case GameNode::Type::Decision:
        return processDecisionNode(
            node,
            p_past_actions_p0,
            p_past_actions_p1,
            p_past_chances,
            accumulate_regsum,
            accumulate_strategy
        );
    
    case GameNode::Type::Chance:
        return processChanceNode(
            node,
            p_past_actions_p0,
            p_past_actions_p1,
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
    updates  strategy for inforset of a given node
*/
template<typename ISKey>
double CFRPlus<ISKey>::processDecisionNode(
    const shared_ptr<const GameNode> node,
    double p_past_actions_p0,
    double p_past_actions_p1,
    double p_past_chances,
    bool accumulate_regsum,
    bool accumulate_strategy
) {

    const vector<int>& available_actions = node->getLegalActions();
    int n_available_actions = available_actions.size();

    vector<double> action_utilities(n_available_actions, 0);
    
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

    for (int action_idx = 0; action_idx < n_available_actions; action_idx++) {
        double next_p_past_actions_p0 = p_past_actions_p0;
        double next_p_past_actions_p1 = p_past_actions_p1;

        if (node->getCurrentPlayer() == 0) {
            next_p_past_actions_p0 *= regretsum_strategy[action_idx];
        }
        if (node->getCurrentPlayer() == 1) {
            next_p_past_actions_p1 *= regretsum_strategy[action_idx];
        }

        shared_ptr<const GameNode> next_node = \
            node->applyAction(available_actions[action_idx]);
        
        action_utilities[action_idx] = \
            processNode(
                next_node,
                next_p_past_actions_p0,
                next_p_past_actions_p1,
                p_past_chances,
                accumulate_regsum,
                accumulate_strategy
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
        infoset.setInstantRegret(action_idx, new_regret);
    }

    if (accumulate_regsum || accumulate_strategy) {
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


template<typename ISKey>
double CFRPlus<ISKey>::processChanceNode(
    const shared_ptr<const GameNode> node,
    double p_past_actions_p0,
    double p_past_actions_p1,
    double p_past_chances,
    bool accumulate_regsum,
    bool accumulate_strategy
) {
    const vector<int>& available_actions = node->getLegalActions();
    int n_available_actions = available_actions.size();

    vector<double> action_utilities(n_available_actions, 0);
    vector<double> chance_probs = node->getChanceProbabilities();

    for (int action_idx = 0; action_idx < n_available_actions; action_idx++) {
        shared_ptr<const GameNode> next_node = \
            node->applyAction(available_actions[action_idx]);
        action_utilities[action_idx] = \
            processNode(
                next_node,
                p_past_actions_p0,
                p_past_actions_p1,
                p_past_chances * chance_probs[action_idx],
                accumulate_regsum,
                accumulate_strategy
            );
    }

    double chance_node_utility = 0;

    for (int action_idx = 0; action_idx < n_available_actions; action_idx++) {
        chance_node_utility += \
            chance_probs[action_idx] * action_utilities[action_idx];
    }
    return chance_node_utility;  
}


template<typename ISKey>
double CFRPlus<ISKey>::processTerminalNode(
    const shared_ptr<const GameNode> node
) {
    return node->getTerminalUtilities()[0];
}

// Explicit instantiation definitions - this generates the actual code
template class CFRPlus<string>;
template class CFRPlus<size_t>;
