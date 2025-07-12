#pragma once

#include <vector>
#include <memory>
#include <stdexcept>
#include <string>
#include <functional>
#include <typeinfo>
#include <cxxabi.h>
#include <cstdlib>
#include "Utils.h"

using namespace std;

/**
 * @class GameNode
 * @brief Abstract base class representing a node in a game tree.
 * 
 * GameNode is the fundamental building block for representing game states in extensive-form games.
 * Each node has a specific type (Terminal, Chance, or Decision) and provides different functionality
 * based on its type.
 * 
 * @section node_types Node Types
 * - **Terminal**: Represents end states of the game with final utilities
 * - **Chance**: Represents random events with associated probabilities
 * - **Decision**: Represents player decision points with information sets
 * 
 * @section type_functions Type-Specific Functions
 * 
 * **All Node Types:**
 * - Type getType() const - Returns the node type
 * - string getTypeString() const - Returns string representation of node type
 * 
 * **Terminal Nodes:**
 * - const vector<double>& getTerminalUtilities() const - Returns final payoffs for all players
 * 
 * **Chance Nodes:**
 * - const vector<double>& getChanceProbabilities() const - Returns probability distribution over actions
 * 
 * **Decision and Chance Nodes:**
 * - const vector<int>& getLegalActions() const - Returns available actions
 * - shared_ptr<const GameNode> applyAction(int action) const - Returns child node after action
 * 
 * **Decision Nodes:**
 * - int getCurrentPlayer() const - Returns the player to move
 * - string getInfoSetKeyString() const - Returns information set identifier as string, 
 *   optional if int version is implemented
 * - size_t getInfoSetKeyInt() const - Returns information set identifier as integer value, 
 *   optional if string version is implemented, can be re-implemented for optimization
 * 
 * @note Calling inappropriate functions for a node type will throw logic_error exceptions.
 *       Derived classes must implement the appropriate virtual functions for their node type.
 */
class GameNode {
public:
    enum class Type { Terminal, Chance, Decision };

    virtual Type getType() const = 0;
    virtual ~GameNode() = default;

    string getTypeString() const;
    
    // toString and actionToString are logging helper functions
    virtual string toString() const;

    // Function for Terminal Nodes
    virtual const vector<double>& getTerminalUtilities() const;
    // Function for Chance Nodes
    virtual const vector<double>& getChanceProbabilities() const;
    // Functions for both Chance and Decision Nodes
    virtual const vector<int>& getLegalActions() const;
    virtual shared_ptr<const GameNode> applyAction(int action) const;
    virtual string actionToString(int action) const;
    // Functions for Decision Nodes
    virtual int getCurrentPlayer() const;
    virtual string getInfoSetKeyString() const;
    virtual size_t getInfoSetKeyInt() const;

    template <typename Key>
    Key getInfoSetKey() const = delete;

protected:
    // Throws an exception when a function is called on a node type that doesn't support it
    [[noreturn]]
    void throwWrongNodeTypeFnException(const string& funcName) const;
 
    // Throws an exception when a required function implementation is missing
    [[noreturn]]
    void throwMissingFnException(const string& funcName) const;
};

template<GameNode::Type T>
class TypedGameNode : public GameNode {
public:
    Type getType() const override final { return T; }
};

// TerminalNode
using TerminalNode = TypedGameNode<GameNode::Type::Terminal>;

// ChanceNode
using ChanceNode = TypedGameNode<GameNode::Type::Chance>;

// DecisionNode
using DecisionNode = TypedGameNode<GameNode::Type::Decision>;


// Template specializations must be defined outside the class
template <>
inline size_t GameNode::getInfoSetKey<size_t>() const { 
    return getInfoSetKeyInt(); 
}

template <>
inline string GameNode::getInfoSetKey<string>() const { 
    return getInfoSetKeyString(); 
}
