/**
 * @brief A wrapper class that handles Monte Carlo traversal of game nodes.
 * 
 * This class wraps around GameNode instances and provides Monte Carlo behavior
 * for chance nodes. When the wrapped node is a chance node, it randomly samples
 * a single action using the correct chance probabilities.
 * 
 * The class maintains the interface of GameNode while internally handling
 * the stochastic traversal of chance nodes in the game tree.
 * 
 * Child nodes receive independent samplers via createNextSampler() to avoid
 * correlation between siblings in the game tree.
 * 
 * @tparam T The type of the wrapped node, must inherit from TypedGameNode<T>
 */

#pragma once

#include "abstract/nodes/game_node.h"
#include <deque>

using namespace std;

template<typename T>
requires is_base_of_v<TypedGameNode<T>, T>
class MonteCarloWrap : public TypedGameNode<MonteCarloWrap<T>> {
public:
    MonteCarloWrap(
        shared_ptr<const T> node,
        deque<int> preselected_chance_actions = {}
    );

    GameNode::Type getType() const override;

    const vector<double>& getTerminalUtilities() const override;

    const vector<double>& getChanceProbabilities() const override;

    const vector<int>& getLegalActions() const override;
    shared_ptr<const MonteCarloWrap<T>> applyActionTyped(int action) const override;

    int getCurrentPlayer() const override;

    string getInfoSetKeyString() const override;
    size_t getInfoSetKeyInt() const override;

    string toString() const override;
    string actionToString(int action) const override;

    /// @brief Get the underlying wrapped node
    shared_ptr<const T> getWrappedNode() const { return wrapped_node_; }

private:
    shared_ptr<const T> wrapped_node_;
    deque<int> preselected_chance_actions_;
    vector<int> chance_legal_action_;
    static inline const vector<double> chance_probability_{1.0}; 
};

#include "abstract/nodes/monte_carlo_wrap.hpp"