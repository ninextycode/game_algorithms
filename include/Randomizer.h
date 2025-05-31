#pragma once

#include "GameNode.h"
#include <vector>
#include <memory>
#include <unordered_map>

using namespace std;

class RandomizedDecisionNode : public GameNode {
public:
    RandomizedDecisionNode(
        shared_ptr<GameNode> wrapped_node,
        double p_random_move,
        size_t max_random_actions
    );

    // GameNode interface implementation
    bool isTerminal() const override;
    vector<double> getUtilitiesForTerminal() const override;
    bool isChance() const override;
    vector<double> getChanceProbabilities() const override;
    int getCurrentPlayer() const override;
    vector<int> getAvailableActions() const override;
    shared_ptr<GameNode> nextGameNode(int action) const override;
    string getInfoSetKey() const override;

    // Helper method to get next decision node directly
    shared_ptr<GameNode> next_decision_game_node(int action) const;

private:
    shared_ptr<GameNode> wrapped_node_;
    double p_random_move_;
    size_t max_random_actions_;
};

class RandomizedChanceNode : public GameNode {
public:
    RandomizedChanceNode(
        shared_ptr<GameNode> parent_node,
        int parent_action,
        double p_random_move,
        size_t max_random_actions
    );

    // GameNode interface implementation
    bool isTerminal() const override;
    vector<double> getUtilitiesForTerminal() const override;
    bool isChance() const override;
    vector<double> getChanceProbabilities() const override;
    int getCurrentPlayer() const override;
    vector<int> getAvailableActions() const override;
    shared_ptr<GameNode> nextGameNode(int action) const override;
    string getInfoSetKey() const override;

private:
    double p_random_move_;
    size_t max_random_actions_;
    vector<int> available_actions_;
    vector<double> probabilities_;
    unordered_map<int, shared_ptr<GameNode>> nodes_after_actions_;
};

class Randomizer {
public:
    static shared_ptr<GameNode> create_randomized_game_tree(
        shared_ptr<GameNode> root_node,
        double p_random_move,
        size_t max_random_actions = 2
    );
};