#pragma once

#include "rng/random_sampler.h"

template<typename T>
requires is_base_of_v<TypedGameNode<T>, T>
MonteCarloWrap<T>::MonteCarloWrap(
    shared_ptr<const T> node,
    deque<int> preselected_chance_actions
)
    : wrapped_node_(node), 
      preselected_chance_actions_(preselected_chance_actions),
      chance_legal_action_()
{
    if (wrapped_node_->getType() == GameNode::Type::Chance) {
        int chance_action = 0;
        if (!preselected_chance_actions_.empty()) {
            chance_action = preselected_chance_actions_.front();
            preselected_chance_actions_.pop_front();
        } else {
            // Sample using correct chance probabilities
            auto sampler = RandomSampler::createNextSampler();
            const auto& legal_actions = wrapped_node_->getLegalActions();
            const auto& probs = wrapped_node_->getChanceProbabilities();
            chance_action = sampler.choice(legal_actions, probs);
        }
        chance_legal_action_.push_back(chance_action);
    }
}

template<typename T>
requires is_base_of_v<TypedGameNode<T>, T>
GameNode::Type MonteCarloWrap<T>::getType() const {
    return wrapped_node_->getType();
}

template<typename T>
requires is_base_of_v<TypedGameNode<T>, T>
const vector<double>& MonteCarloWrap<T>::getTerminalUtilities() const {
    return wrapped_node_->getTerminalUtilities();
}

template<typename T>
requires is_base_of_v<TypedGameNode<T>, T>
const vector<double>& MonteCarloWrap<T>::getChanceProbabilities() const {
    if (wrapped_node_->getType() != GameNode::Type::Chance) {
        this->throwWrongNodeTypeFnException("getChanceProbabilities");
    }
    return chance_probability_;
}

template<typename T>
requires is_base_of_v<TypedGameNode<T>, T>
const vector<int>& MonteCarloWrap<T>::getLegalActions() const {
    if (wrapped_node_->getType() == GameNode::Type::Chance) {
        return chance_legal_action_;
    }
    return wrapped_node_->getLegalActions();
}

template<typename T>
requires is_base_of_v<TypedGameNode<T>, T>
shared_ptr<const MonteCarloWrap<T>> MonteCarloWrap<T>::applyActionTyped(int action) const {
    return make_shared<MonteCarloWrap<T>>(
        wrapped_node_->applyActionTyped(action),
        preselected_chance_actions_
    );
}

template<typename T>
requires is_base_of_v<TypedGameNode<T>, T>
int MonteCarloWrap<T>::getCurrentPlayer() const {
    return wrapped_node_->getCurrentPlayer();
}

template<typename T>
requires is_base_of_v<TypedGameNode<T>, T>
string MonteCarloWrap<T>::getInfoSetKeyString() const {
    return wrapped_node_->getInfoSetKeyString();
}

template<typename T>
requires is_base_of_v<TypedGameNode<T>, T>
size_t MonteCarloWrap<T>::getInfoSetKeyInt() const {
    return wrapped_node_->getInfoSetKeyInt();
}

template<typename T>
requires is_base_of_v<TypedGameNode<T>, T>
string MonteCarloWrap<T>::toString() const {
    return wrapped_node_->toString();    
}

template<typename T>
requires is_base_of_v<TypedGameNode<T>, T>
string MonteCarloWrap<T>::actionToString(int action) const {
    return wrapped_node_->actionToString(action);    
}

