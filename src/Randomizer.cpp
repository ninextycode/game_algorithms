#include "Randomizer.h"
#include <cstdlib>
#include <algorithm>

RandomizedDecisionNode::RandomizedDecisionNode(
    shared_ptr<GameNode> wrapped_node,
    double p_random_move,
    size_t max_random_actions
) :
    wrapped_node_(wrapped_node),
    p_random_move_(p_random_move),
    max_random_actions_(max_random_actions)
{
}

bool RandomizedDecisionNode::isTerminal() const {
    return wrapped_node_->isTerminal();
}

vector<double> RandomizedDecisionNode::getUtilitiesForTerminal() const {
    return wrapped_node_->getUtilitiesForTerminal();
}

bool RandomizedDecisionNode::isChance() const {
    return wrapped_node_->isChance();
}

vector<double> RandomizedDecisionNode::getChanceProbabilities() const {
    return wrapped_node_->getChanceProbabilities();
}

int RandomizedDecisionNode::getCurrentPlayer() const {
    return wrapped_node_->getCurrentPlayer();
}

vector<int> RandomizedDecisionNode::getAvailableActions() const {
    return wrapped_node_->getAvailableActions();
}

shared_ptr<GameNode> RandomizedDecisionNode::nextGameNode(int action) const {
    // If wrapped node is terminal or chance node, delegate directly
    if (wrapped_node_->isTerminal() || wrapped_node_->isChance()) {
        return wrapped_node_->nextGameNode(action);
    }
    
    // For decision nodes, insert a chance node
    return make_shared<RandomizedChanceNode>(
        wrapped_node_, action, p_random_move_, max_random_actions_
    );
}

shared_ptr<GameNode> RandomizedDecisionNode::next_decision_game_node(int action) const {
    shared_ptr<GameNode> next_node = wrapped_node_->nextGameNode(action);
    
    return make_shared<RandomizedDecisionNode>(
        next_node, p_random_move_, max_random_actions_
    );
}

string RandomizedDecisionNode::getInfoSetKey() const {
    return wrapped_node_->getInfoSetKey();
}

RandomizedChanceNode::RandomizedChanceNode(
    shared_ptr<GameNode> parent_node,
    int parent_action,
    double p_random_move,
    size_t max_random_actions
) :
    p_random_move_(p_random_move),
    max_random_actions_(max_random_actions),
    available_actions_(parent_node->getAvailableActions()),
    probabilities_(available_actions_.size(), 0),
    nodes_after_actions_()
{
    double p_single_random_choice = p_random_move_ / available_actions_.size();
    double p_nonrandom_choice = 1 - p_random_move_ + p_single_random_choice;

    // Create all possible next nodes and set probabilities
    for (size_t i = 0; i < available_actions_.size(); i++) {
        int action = available_actions_[i];
        
        if (action == parent_action) {
            probabilities_[i] = p_nonrandom_choice;
        } else {
            probabilities_[i] = p_single_random_choice;
        }

        shared_ptr<GameNode> next_node = parent_node->nextGameNode(action);
        nodes_after_actions_.emplace(
            action, 
            make_shared<RandomizedDecisionNode>(next_node, p_random_move_, max_random_actions_)
        );
    }

    // Remove random actions to control branching factor
    while (available_actions_.size() > max_random_actions_) {
        size_t rand_idx = rand() % available_actions_.size();
        int to_be_removed_action = available_actions_[rand_idx];
        
        // Don't remove the parent action
        if (to_be_removed_action == parent_action) {
            continue;
        }
        
        // Remove the action and its associated data
        nodes_after_actions_.erase(to_be_removed_action);
        available_actions_.erase(available_actions_.begin() + rand_idx);
        probabilities_.erase(probabilities_.begin() + rand_idx);
    }
    
    // Renormalize probabilities
    p_single_random_choice = p_random_move_ / available_actions_.size();
    p_nonrandom_choice = 1 - p_random_move_ + p_single_random_choice;
    
    for (size_t i = 0; i < available_actions_.size(); i++) {
        int action = available_actions_[i];
        if (action == parent_action) {
            probabilities_[i] = p_nonrandom_choice;
        } else {
            probabilities_[i] = p_single_random_choice;
        }
    }
}

bool RandomizedChanceNode::isTerminal() const {
    return false;
}

vector<double> RandomizedChanceNode::getUtilitiesForTerminal() const {
    return {};
}

bool RandomizedChanceNode::isChance() const {
    return true;
}

vector<double> RandomizedChanceNode::getChanceProbabilities() const {
    return probabilities_;
}

int RandomizedChanceNode::getCurrentPlayer() const {
    return GameNode::CHANCE_PLAYER;
}

vector<int> RandomizedChanceNode::getAvailableActions() const {
    return available_actions_;
}

shared_ptr<GameNode> RandomizedChanceNode::nextGameNode(int action) const {
    return nodes_after_actions_.at(action);
}

string RandomizedChanceNode::getInfoSetKey() const {
    return "";
}

shared_ptr<GameNode> Randomizer::create_randomized_game_tree(
    shared_ptr<GameNode> root_node,
    double p_random_move,
    size_t max_random_actions
) {
    return make_shared<RandomizedDecisionNode>(root_node, p_random_move, max_random_actions);
}