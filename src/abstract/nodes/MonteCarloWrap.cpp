#include "abstract/nodes/MonteCarloWrap.h"
#include <random>


MonteCarloWrap::MonteCarloWrap(
    shared_ptr<const GameNode> node,
    queue<int> preselected_chance_actions,
    uint64_t seed
)
    : wrapped_node_(node), 
    seed_(seed),
    preselected_chance_actions_(preselected_chance_actions),
    chance_legal_action_()
{
    
    if (seed_ == 0) {
        random_device rd;
        seed_ = rd();
    }
    
    if (wrapped_node_->getType() == GameNode::Type::Chance) {
        int chance_action = 0;
        if (preselected_chance_actions_.size() > 0) {
            chance_action = preselected_chance_actions_.front();
            preselected_chance_actions_.pop();
        } else {
            // If no preselected actions, randomly select one
            mt19937_64 rng(seed_);
            auto legal_actions = wrapped_node_->getLegalActions();
            uniform_int_distribution<int> distribution(0, legal_actions.size() - 1);
            chance_action = legal_actions[distribution(rng)];
        }
        chance_legal_action_.push_back(chance_action);
    }
}

uint64_t MonteCarloWrap::getSeed() const {
    return seed_;
}

GameNode::Type MonteCarloWrap::getType() const {
    return wrapped_node_->getType();
}

const vector<double>& MonteCarloWrap::getTerminalUtilities() const {
    return wrapped_node_->getTerminalUtilities();
}

const vector<double>& MonteCarloWrap::getChanceProbabilities() const {
    if (getType() != Type::Chance) {
        throwWrongNodeTypeFnException("getChanceProbabilities");
    } else {
        return MonteCarloWrap::chance_probability;
    }
}

const vector<int>& MonteCarloWrap::getLegalActions() const {
    if (getType() == Type::Chance) {
        return chance_legal_action_;
    } else {
        return wrapped_node_->getLegalActions();
    }
}

shared_ptr<const GameNode> MonteCarloWrap::applyAction(int action) const {
    return make_shared<MonteCarloWrap>(
        wrapped_node_->applyAction(action),
        preselected_chance_actions_, seed_
    );
}

int MonteCarloWrap::getCurrentPlayer() const {
    return wrapped_node_->getCurrentPlayer();
}

string MonteCarloWrap::getInfoSetKeyString() const {
    return wrapped_node_->getInfoSetKeyString();
}

size_t MonteCarloWrap::getInfoSetKeyInt() const {
    return wrapped_node_->getInfoSetKeyInt();
}

string MonteCarloWrap::toString() const {
    return wrapped_node_->toString();    
}

string MonteCarloWrap::actionToString(int action) const {
    return wrapped_node_->actionToString(action);    
}
