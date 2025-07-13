/**
 * @brief A wrapper class that handles Monte Carlo traversal of game nodes.
 * 
 * This class wraps around GameNode instances and provides Monte Carlo behavior
 * for chance nodes. When the wrapped node is a chance node, it randomly traverses
 * through the chance nodes using the provided seed until it reaches the first
 * non-chance node, which becomes the final wrapped node.
 * 
 * The class maintains the interface of GameNode while internally handling
 * the stochastic traversal of chance nodes in the game tree.
 */

#pragma once

#include "abstract/nodes/GameNode.h"
#include <queue>

using namespace std;

class MonteCarloWrap : public GameNode {
public:
    MonteCarloWrap(
        shared_ptr<const GameNode> node,
        queue<int> preselected_chance_actions = {},
        uint64_t seed = 0
    );

    Type getType() const override;

    const vector<double>& getTerminalUtilities() const override;

    const vector<double>& getChanceProbabilities() const override;

    const vector<int>& getLegalActions() const override;
    shared_ptr<const GameNode> applyAction(int action) const override;

    int getCurrentPlayer() const override;

    string getInfoSetKeyString() const override;
    size_t getInfoSetKeyInt() const override;

    string toString() const override;
    string actionToString(int action) const override;

    uint64_t getSeed() const;

private:
    shared_ptr<const GameNode> wrapped_node_;
    uint64_t seed_;
    queue<int> preselected_chance_actions_;
    vector<int> chance_legal_action_;
    static inline const vector<double> chance_probability{1.0}; 
};
