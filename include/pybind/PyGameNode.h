#pragma once

#include "GameNode.h"

class PyGameNode : public GameNode {
    // using GameNode::GameNode;

    // TerminalNode
    bool isTerminal() const override;
    vector<double> getUtilitiesForTerminal() const override;

    // ChanceNode
    bool isChance() const override;
    vector<double> getChanceProbabilities() const override;

    // DecisionNode
    int getCurrentPlayer() const override;
    vector<int> getAvailableActions() const override;
    shared_ptr<GameNode> nextGameNode(int action) const override;
    std::string getInfoSetKey() const override;
    
    ~PyGameNode() {}
};