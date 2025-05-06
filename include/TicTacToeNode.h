#ifndef TICTACTOE_H
#define TICTACTOE_H

#include "GameNode.h"
#include <array>
#include <string>
#include <vector>
#include <memory>

class TicTacToeNode : public GameNode {
public:
    // Constructor for initial state
    TicTacToeNode();
    
    // Constructor for state after move
    TicTacToeNode(std::array<int, 9> board, int currentPlayer);
    
    // GameNode interface implementation
    bool isTerminal() const override;
    std::vector<double> getUtilitiesForTerminal() const override;
    
    bool isChance() const override;
    std::vector<double> getChanceProbabilities() const override;
    
    int getCurrentPlayer() const override;
    std::vector<int> getAvailableActions() const override;
    std::shared_ptr<GameNode> nextGameNode(int action) const override;
    std::string getInfoSetKey() const override;
    
    // Additional methods
    std::string getBoardString() const;

private:
    // Board is represented as array of 9 elements (0-8 positions)
    // -1: empty, 0: first player (cross), 1: second player (circle)
    std::array<int, 9> board;
    int currentPlayer; // 0 or 1
    
    // Cache terminal state and utility
    mutable bool calculatedTerminal;
    mutable bool isTerminalState;
    mutable std::vector<double> utilityValues;
    
    // Calculate if the game has ended and utility values
    void calculateTerminalAndUtility() const;
};

#endif // TICTACTOE_H