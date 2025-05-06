// filepath: /home/maxim/Programming/poker_bot/game_model_c/extern/game_algorithms/src/pybind/PyGameNode.cpp

#include "pybind/PyGameNode.h"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/iostream.h>

namespace py = pybind11;

// Implementation of the trampoline class for PyGameNode

// TerminalNode methods
bool PyGameNode::isTerminal() const {
    PYBIND11_OVERRIDE_PURE(
        bool,                // Return type
        GameNode,            // Parent class
        isTerminal           // Function name
    );
}

vector<double> PyGameNode::getUtilitiesForTerminal() const {
    PYBIND11_OVERRIDE_PURE(
        vector<double>,      // Return type
        GameNode,            // Parent class
        getUtilitiesForTerminal  // Function name
    );
}

// ChanceNode methods
bool PyGameNode::isChance() const {
    PYBIND11_OVERRIDE_PURE(
        bool,                // Return type
        GameNode,            // Parent class
        isChance             // Function name
    );
}

vector<double> PyGameNode::getChanceProbabilities() const {
    PYBIND11_OVERRIDE_PURE(
        vector<double>,      // Return type
        GameNode,            // Parent class
        getChanceProbabilities  // Function name
    );
}

// DecisionNode methods
int PyGameNode::getCurrentPlayer() const {
    PYBIND11_OVERRIDE_PURE(
        int,                 // Return type
        GameNode,            // Parent class
        getCurrentPlayer     // Function name
    );
}

vector<int> PyGameNode::getAvailableActions() const {
    PYBIND11_OVERRIDE_PURE(
        vector<int>,         // Return type
        GameNode,            // Parent class
        getAvailableActions  // Function name
    );
}

shared_ptr<GameNode> PyGameNode::nextGameNode(int action) const {
    PYBIND11_OVERRIDE_PURE(
        shared_ptr<GameNode>,  // Return type
        GameNode,              // Parent class
        nextGameNode,          // Function name
        action                 // Arguments
    );
}

std::string PyGameNode::getInfoSetKey() const {
    PYBIND11_OVERRIDE_PURE(
        std::string,         // Return type
        GameNode,            // Parent class
        getInfoSetKey        // Function name
    );
}