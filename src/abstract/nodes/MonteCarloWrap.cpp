#include "abstract/nodes/MonteCarloWrap.h"
#include <random>


MonteCarloWrap::MonteCarloWrap(shared_ptr<const GameNode> node, uint64_t seed)
    : non_chance_node_(node), seed_(seed) {
    
    if (seed == 0) {
        random_device rd;
        seed_ = rd();
    }

    mt19937_64 rng(seed);
    while (non_chance_node_->getType() == GameNode::Type::Chance) {
        auto legal_actions = non_chance_node_->getLegalActions();
        uniform_int_distribution<int> distribution(0, legal_actions.size() - 1);
        int random_index = distribution(rng);
        non_chance_node_ = non_chance_node_->applyAction(legal_actions[random_index]);
    }
}

GameNode::Type MonteCarloWrap::getType() const {
    return non_chance_node_->getType();
}

const vector<double>& MonteCarloWrap::getTerminalUtilities() const {
    return non_chance_node_->getTerminalUtilities();
}

const vector<double>& MonteCarloWrap::getChanceProbabilities() const {
    return non_chance_node_->getChanceProbabilities();
}

const vector<int>& MonteCarloWrap::getLegalActions() const {
    return non_chance_node_->getLegalActions();
}

shared_ptr<const GameNode> MonteCarloWrap::applyAction(int action) const {
    return make_shared<MonteCarloWrap>(non_chance_node_->applyAction(action), seed_);
}

int MonteCarloWrap::getCurrentPlayer() const {
    return non_chance_node_->getCurrentPlayer();
}

string MonteCarloWrap::getInfoSetKeyString() const {
    return non_chance_node_->getInfoSetKeyString();
}

size_t MonteCarloWrap::getInfoSetKeyInt() const {
    return non_chance_node_->getInfoSetKeyInt();
}