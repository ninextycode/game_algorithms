#pragma once

#include <vector>
#include <string>
#include <memory>

using namespace std;


class GameNode {
public:
    static constexpr int CHANCE_PLAYER = -1;  // TODO

    // TerminalNode
    virtual bool isTerminal() const = 0;
    virtual vector<double> getUtilitiesForTerminal() const = 0;

    // ChanceNode
    virtual bool isChance() const = 0;
    virtual vector<double> getChanceProbabilities() const = 0;

    // DecisionNode
    virtual int getCurrentPlayer() const = 0;
    virtual vector<int> getAvailableActions() const = 0;
    virtual shared_ptr<GameNode> nextGameNode(int action) const = 0;
    virtual std::string getInfoSetKey() const = 0;
    
    virtual ~GameNode() {}
};