#pragma once

#include "abstract/nodes/GameNode.h"
#include "tictactoe/TicTacToeBoard.h"
#include <string>
#include <vector>
#include <memory>
#include <array>

using namespace std;

class TicTacToeNode : public GameNode {
public:
    TicTacToeNode();
    TicTacToeNode(const TicTacToeBoard& board);

    Type getType() const override;
    
    const vector<double>& getTerminalUtilities() const override;
    
    int getCurrentPlayer() const override;
    const vector<int>& getLegalActions() const override;
    shared_ptr<const GameNode> applyAction(int action) const override;
    string getInfoSetKeyString() const override;
    size_t getInfoSetKeyInt() const override;
    
    string getBoardString() const;
    const TicTacToeBoard& getBoard() const;

protected:
    TicTacToeBoard board;
    vector<int> legal_actions;
    bool is_terminal_state;
    vector<double> utility_values;
    
    void calculateProperties();
};