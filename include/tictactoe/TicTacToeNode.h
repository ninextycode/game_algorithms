#pragma once

#include "GameNode.h"
#include "tictactoe/TicTacToeBoard.h"
#include <string>
#include <vector>
#include <memory>
#include <array>

using namespace std;

class TicTacToeNode : public GameNode {
public:
    // Constructor for initial state
    TicTacToeNode();
    
    // Constructor for state after move using TicTacToeBoard
    TicTacToeNode(const TicTacToeBoard& board);
    
    // Copy constructor
    TicTacToeNode(const TicTacToeNode& other);

    // Move constructor
    TicTacToeNode(const TicTacToeNode&& other);
    
    // GameNode interface implementation
    bool isTerminal() const override;
    vector<double> getUtilitiesForTerminal() const override;
    
    bool isChance() const override;
    vector<double> getChanceProbabilities() const override;
    
    int getCurrentPlayer() const override;
    vector<int> getAvailableActions() const override;
    shared_ptr<GameNode> nextGameNode(int action) const override;
    string getInfoSetKey() const override;
    
    // Additional methods
    string getBoardString() const;
    
    // New method to get board
    const TicTacToeBoard& getBoard() const;
    
    // Legacy method for backward compatibility
    array<int, 9> getBoardArray() const;

protected:
    // Board is now represented as a TicTacToeBoard object
    TicTacToeBoard board;
    
    // Cache terminal state and utility
    bool is_terminal_state;
    vector<double> utility_values;
    
    // Calculate if the game has ended and utility values
    void calculateTerminalAndUtility();
};