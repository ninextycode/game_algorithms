#include "abstract/nodes/Randomizer.h"
#include <cstdlib>
#include <algorithm>


WrapNode::WrapNode(
    shared_ptr<const GameNode> wrapped_node,
    double p_random_choice,
    int max_random_actions
) :
    wrapped_node_(wrapped_node),
    p_random_choice_(p_random_choice),
    max_random_actions_(max_random_actions)
{ }

shared_ptr<const GameNode> WrapNode::applyActionWrapped(int action) const {
    shared_ptr<const GameNode> next_node = wrapped_node_->applyAction(action);

    switch (next_node->getType()) {
    case GameNode::Type::Decision:
        return make_shared<RandomizerWrapNode>(
            next_node, p_random_choice_, max_random_actions_
        );
    case GameNode::Type::Chance:
        return make_shared<ChanceWrapNode>(
            next_node, p_random_choice_, max_random_actions_
        );
    case GameNode::Type::Terminal:
        return next_node;
    default:
        throw logic_error("Unexpected type");
    }
}



RandomizerWrapNode::RandomizerWrapNode(
    shared_ptr<const GameNode> wrapped_node,
    double p_random_choice,
    int max_random_actions
) :
    WrapNode(wrapped_node, p_random_choice, max_random_actions)
{
    for (int a: legal_actions) {
        double p = (a == RANDOM_CHOICE) ?
            p_random_choice_ : 1 - p_random_choice_;
        action_probabilities_.push_back(p);
    }
}

const vector<int>& RandomizerWrapNode::getLegalActions() const {
    return legal_actions;
}

shared_ptr<const GameNode> RandomizerWrapNode::applyAction(int action) const {
    switch (action) {
    case RANDOM_CHOICE:
        return make_shared<RandomDecisionWrapNode>(
            wrapped_node_, p_random_choice_, max_random_actions_
        );
    case NONRANDOM_CHOICE:
        return make_shared<NonRandomDecisionWrapNode>(
            wrapped_node_, p_random_choice_, max_random_actions_
        );
    default:
        throw logic_error("Illegal action " + to_string(action));
    }
}

const vector<double>& RandomizerWrapNode::getChanceProbabilities() const {
    return action_probabilities_;
}



NonRandomDecisionWrapNode::NonRandomDecisionWrapNode(
    shared_ptr<const GameNode> wrapped_node,
    double p_random_choice,
    int max_random_actions
) :
    WrapNode(wrapped_node, p_random_choice, max_random_actions)
{

}

int NonRandomDecisionWrapNode::getCurrentPlayer() const {
    return wrapped_node_->getCurrentPlayer();
}

string NonRandomDecisionWrapNode::getInfoSetKeyString() const {
    return wrapped_node_->getInfoSetKeyString();
}

size_t NonRandomDecisionWrapNode::getInfoSetKeyInt() const {
    return wrapped_node_->getInfoSetKeyInt();
}

const vector<int>& NonRandomDecisionWrapNode::getLegalActions() const {
    return wrapped_node_->getLegalActions();
}

shared_ptr<const GameNode> NonRandomDecisionWrapNode::applyAction(int action) const {
    return this->applyActionWrapped(action);
}


RandomDecisionWrapNode::RandomDecisionWrapNode(
    shared_ptr<const GameNode> wrapped_node,
    double p_random_choice,
    size_t max_random_actions
) : 
    WrapNode(wrapped_node, p_random_choice, max_random_actions) 
{
    vector<int> legal_actions = wrapped_node->getLegalActions();
    
    int n_random_actions = max_random_actions_;
    if (n_random_actions == -1 || n_random_actions > (int) legal_actions.size()) {
        n_random_actions = legal_actions.size();
    }
    
    while ((int) random_actions.size() < n_random_actions) {
        size_t rand_idx = rand() % legal_actions.size();
        random_actions.push_back(legal_actions[rand_idx]);
        legal_actions.erase(legal_actions.begin() + rand_idx);
    }

    probabilities = vector<double>(n_random_actions, 1.0 / n_random_actions);
}


const vector<double>& RandomDecisionWrapNode::getChanceProbabilities() const {
    return probabilities;
}

const vector<int>& RandomDecisionWrapNode::getLegalActions() const {
    return random_actions;
}

shared_ptr<const GameNode> RandomDecisionWrapNode::applyAction(int action) const {
    if (!ranges::contains(random_actions, action)) {
        throw logic_error("Action " + to_string(action) + " is not a legal action");
    }
    return this->applyActionWrapped(action);
}




ChanceWrapNode::ChanceWrapNode(
    shared_ptr<const GameNode> wrapped_node,
    double p_random_choice,
    size_t max_random_actions
) : 
    WrapNode(wrapped_node, p_random_choice, max_random_actions) 
{ 

}

const vector<double>& ChanceWrapNode::getChanceProbabilities() const {
    return wrapped_node_->getChanceProbabilities();
}

const vector<int>& ChanceWrapNode::getLegalActions() const {
    return wrapped_node_->getLegalActions();
}

shared_ptr<const GameNode> ChanceWrapNode::applyAction(int action) const {
    return this->applyActionWrapped(action);
}