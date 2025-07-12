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


using namespace std;

class MonteCarloWrap : public GameNode {
public:
    MonteCarloWrap(shared_ptr<const GameNode> node, uint64_t seed = 0);

    Type getType() const override;

    const vector<double>& getTerminalUtilities() const override;

    const vector<double>& getChanceProbabilities() const override;

    const vector<int>& getLegalActions() const override;
    shared_ptr<const GameNode> applyAction(int action) const override;

    int getCurrentPlayer() const override;

    string getInfoSetKeyString() const override;
    size_t getInfoSetKeyInt() const override;

private:
    shared_ptr<const GameNode> non_chance_node_;
    uint64_t seed_;
};
