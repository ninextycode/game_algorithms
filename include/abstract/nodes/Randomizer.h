/**
 * @brief Decision randomization wrapper system for game tree nodes
 * 
 * This header defines a comprehensive wrapper system that adds decision randomization
 * capabilities to existing game tree nodes. The system allows for probabilistic
 * replacement of strategic decisions with random action selection, which is useful
 * for creating diverse training data, implementing exploration strategies, or
 * simulating sub-optimal play.
 * 
 * The wrapper hierarchy consists of:
 * - WrapNode: Base wrapper providing common functionality
 * - RandomizerWrapNode: Entry point that decides between normal/random play
 * - NonRandomDecisionWrapNode: Preserves original decision-making behavior
 * - RandomDecisionWrapNode: Replaces decisions with uniform random selection
 * - ChanceWrapNode: Passes through original chance node behavior unchanged
 * 
 * The randomization process works by wrapping the original game tree with a layer
 * that probabilistically chooses between executing the original strategy or
 * selecting actions uniformly at random. This maintains the original game structure
 * while adding controllable randomness to decision points.
 * 
 */
#pragma once

#include <vector>
#include <memory>
#include <unordered_map>
#include "abstract/nodes/GameNode.h"

using namespace std;


class WrapNode {
public:
    /**
     * Constructor for the base wrapper node
     * @param wrapped_node The original game node to wrap
     * @param p_random_choice Probability of choosing random action over normal decision
     * @param max_random_actions Maximum number of random actions to consider (-1 for all)
     */
    WrapNode(
        shared_ptr<const GameNode> wrapped_node,
        double p_random_choice,
        int max_random_actions = -1
    );
    
    shared_ptr<const GameNode> applyActionWrapped(int action) const;
    
protected:
    shared_ptr<const GameNode> wrapped_node_;
    double p_random_choice_;
    int max_random_actions_;
};


/**
 * Chance node that decides whether a decision should be processed normally
 * or replaced with random action selection. This is the entry point for
 * decision randomization - it gives the choice between normal and random play.
 */
class RandomizerWrapNode : public ChanceNode, public WrapNode {
public:
    RandomizerWrapNode(
        shared_ptr<const GameNode> wrapped_node,
        double p_random_choice,
        int max_random_actions = -1
    );
    
    const vector<int>& getLegalActions() const override;
    shared_ptr<const GameNode> applyAction(int action) const override;
    virtual const vector<double>& getChanceProbabilities() const override;

    static constexpr int NONRANDOM_CHOICE = 0;
    static constexpr int RANDOM_CHOICE = 1;
private:
    static const inline vector<int> legal_actions = \
        {NONRANDOM_CHOICE, RANDOM_CHOICE};

    vector<double> action_probabilities_;
};

/**
 * Decision node wrapper that processes decisions normally without randomization.
 * This is used when the RandomizerWrapNode chooses non-random play.
 * It delegates all decision-making to the original wrapped node.
 */
class NonRandomDecisionWrapNode : public DecisionNode, public WrapNode {
public:
    NonRandomDecisionWrapNode(
        shared_ptr<const GameNode> wrapped_node,
        double p_random_choice,
        int max_random_actions = -1
    );

    int getCurrentPlayer() const override;
    string getInfoSetKeyString() const override;
    size_t getInfoSetKeyInt() const override;
    // Chance and Decision node function
    const vector<int>& getLegalActions() const override;
    shared_ptr<const GameNode> applyAction(int action) const override;
};

/**
 * Chance node wrapper that replaces decision-making with random action selection.
 * This is used when the RandomizerWrapNode chooses random play.
 * It randomly selects from available actions with uniform probability.
 */
class RandomDecisionWrapNode : public ChanceNode, public WrapNode {
public:
    RandomDecisionWrapNode(
        shared_ptr<const GameNode> wrapped_node,
        double p_random_choice,
        size_t max_random_actions = -1
    );

    const vector<double>& getChanceProbabilities() const override;
    const vector<int>& getLegalActions() const override;
    shared_ptr<const GameNode> applyAction(int action) const override;
private:
    vector<double> probabilities;
    vector<int> random_actions;
};

/**
 * Wrapper for chance nodes that preserves their original behavior.
 * Since chance nodes are already random, they don't need decision randomization.
 * This wrapper simply passes through the original chance node's functionality.
 */
class ChanceWrapNode : public ChanceNode, public WrapNode {
public:
    ChanceWrapNode(
        shared_ptr<const GameNode> wrapped_node,
        double p_random_choice,
        size_t max_random_actions = -1
    );

    const vector<double>& getChanceProbabilities() const override;
    const vector<int>& getLegalActions() const override;
    shared_ptr<const GameNode> applyAction(int action) const override;
private:
    vector<int> legal_actions_;
    vector<double> chance_probabilities_;
};