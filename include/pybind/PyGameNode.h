#pragma once

#include "abstract/nodes/GameNode.h"
#include <pybind11/pybind11.h>

class PyGameNode : public GameNode {
public:
    // GameNode type interface
    Type getType() const override;
    
    // Terminal Node functions
    const vector<double>& getTerminalUtilities() const override;

    // Chance Node functions
    const vector<double>& getChanceProbabilities() const override;

    // Chance and Decision node functions
    const vector<int>& getLegalActions() const override;
    shared_ptr<const GameNode> applyAction(int action) const override;

    // Decision Node functions
    int getCurrentPlayer() const override;
    string getInfoSetKeyString() const override;
    size_t getInfoSetKeyInt() const override;
    
    virtual ~PyGameNode() = default;
};