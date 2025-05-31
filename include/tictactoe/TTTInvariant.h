#pragma once

#include "GameNode.h"
#include "tictactoe/TicTacToeNode.h"
#include "tictactoe/TicTacToeBoard.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_set>

using namespace std;

class TTTInvariant : public GameNode {
public:
    // Constructor for initial node
    TTTInvariant();

    // Constructor that wraps a TicTacToeNode
    TTTInvariant(const TicTacToeNode& internal_ttt_node);
    
    // GameNode interface implementation
    bool isTerminal() const override;
    vector<double> getUtilitiesForTerminal() const override;
    
    bool isChance() const override;
    vector<double> getChanceProbabilities() const override;
    
    int getCurrentPlayer() const override;
    vector<int> getAvailableActions() const override;
    shared_ptr<GameNode> nextGameNode(int action) const override;
    string getInfoSetKey() const override;
    
private:
    TicTacToeNode internal_ttt_node;
};