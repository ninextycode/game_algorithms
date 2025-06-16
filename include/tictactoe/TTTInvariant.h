#pragma once

#include "tictactoe/TicTacToeNode.h"
#include "tictactoe/TicTacToeBoard.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_set>

using namespace std;

class TTTInvariant : public GameNode {
public:
    TTTInvariant();

    TTTInvariant(const TicTacToeNode& internal_ttt_node);
    
    Type getType() const override;

    const vector<double>& getTerminalUtilities() const override;    
    const vector<double>& getChanceProbabilities() const override;
    
    int getCurrentPlayer() const override;
    const vector<int>& getLegalActions() const override;
    shared_ptr<const GameNode> applyAction(int action) const override;
    
    string getInfoSetKeyString() const override;
    size_t getInfoSetKeyInt() const override;
private:
    TicTacToeNode internal_ttt_node_;
    vector<int> legal_invariant_actions_;
};