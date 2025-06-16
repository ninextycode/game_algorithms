// filepath: /home/maxim/Programming/poker_bot/game_model_c/extern/game_algorithms/src/pybind/PyGameNode.cpp

#include "pybind/PyGameNode.h"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

// Implementation of the trampoline class for PyGameNode

GameNode::Type PyGameNode::getType() const {
    PYBIND11_OVERRIDE_PURE(
        GameNode::Type,      // Return type
        GameNode,            // Parent class
        getType              // Function name
    );
}

const vector<double>& PyGameNode::getTerminalUtilities() const {
    PYBIND11_OVERRIDE(
        const vector<double>&,  // Return type
        GameNode,               // Parent class
        getTerminalUtilities    // Function name
    );
}


const vector<double>& PyGameNode::getChanceProbabilities() const {
    PYBIND11_OVERRIDE(
        const vector<double>&,  // Return type
        GameNode,               // Parent class
        getChanceProbabilities  // Function name
    );
}

// DecisionNode methods
int PyGameNode::getCurrentPlayer() const {
    PYBIND11_OVERRIDE(
        int,                 // Return type
        GameNode,            // Parent class
        getCurrentPlayer     // Function name
    );
}

const vector<int>& PyGameNode::getLegalActions() const {
    PYBIND11_OVERRIDE(
        const vector<int>&,     // Return type
        GameNode,               // Parent class
        getLegalActions         // Function name
    );
}

shared_ptr<const GameNode> PyGameNode::applyAction(int action) const {
    PYBIND11_OVERRIDE(
        shared_ptr<const GameNode>,  // Return type
        GameNode,                    // Parent class
        applyAction,                 // Function name
        action                       // Arguments
    );
}

string PyGameNode::getInfoSetKeyString() const {
    PYBIND11_OVERRIDE(
        string,              // Return type
        GameNode,            // Parent class
        getInfoSetKeyString  // Function name
    );
}

size_t PyGameNode::getInfoSetKeyInt() const {
    PYBIND11_OVERRIDE(
        size_t,              // Return type
        GameNode,            // Parent class
        getInfoSetKeyInt     // Function name
    );
}