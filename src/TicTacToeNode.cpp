#include "TicTacToeNode.h"
#include <sstream>

// Constructor for initial state
TicTacToeNode::TicTacToeNode() : currentPlayer(0), calculatedTerminal(false) {
    // Initialize empty board (-1 means empty)
    board.fill(-1);
}

// Constructor for state after move
TicTacToeNode::TicTacToeNode(std::array<int, 9> board, int currentPlayer)
    : board(board), currentPlayer(currentPlayer), calculatedTerminal(false) {
}

bool TicTacToeNode::isTerminal() const {
    if (!calculatedTerminal) {
        calculateTerminalAndUtility();
    }
    return isTerminalState;
}

std::vector<double> TicTacToeNode::getUtilitiesForTerminal() const {
    if (!calculatedTerminal) {
        calculateTerminalAndUtility();
    }
    return utilityValues;
}

bool TicTacToeNode::isChance() const {
    // Tic-tac-toe has no chance nodes
    return false;
}

std::vector<double> TicTacToeNode::getChanceProbabilities() const {
    // Tic-tac-toe has no chance nodes, return empty vector
    return std::vector<double>();
}

int TicTacToeNode::getCurrentPlayer() const {
    return currentPlayer;
}

std::vector<int> TicTacToeNode::getAvailableActions() const {
    std::vector<int> actions;
    for (int i = 0; i < 9; ++i) {
        if (board[i] == -1) {  // Empty cell
            actions.push_back(i);
        }
    }
    return actions;
}

std::shared_ptr<GameNode> TicTacToeNode::nextGameNode(int action) const {
    // Create a new board with the action applied
    std::array<int, 9> newBoard = board;
    newBoard[action] = currentPlayer;
    
    // Return new state with updated board and next player
    return std::make_shared<TicTacToeNode>(newBoard, 1 - currentPlayer);
}

std::string TicTacToeNode::getInfoSetKey() const {
    // In perfect information game like tic-tac-toe, the info set is just the board state
    return getBoardString();
}

std::string TicTacToeNode::getBoardString() const {
    std::ostringstream oss;
    for (int i = 0; i < 9; ++i) {
        if (i > 0 && i % 3 == 0) {
            oss << "\n";
        }
        
        if (board[i] == -1) {
            oss << ".";
        } else if (board[i] == 0) {
            oss << "x";
        } else {
            oss << "o";
        }
        
        if (i % 3 < 2) {
            oss << " ";
        }
    }
    return oss.str();
}

void TicTacToeNode::calculateTerminalAndUtility() const {
    // Initialize utility for two players
    utilityValues = {0.0, 0.0};
    
    // Check for win
    static const int winPatterns[8][3] = {
        // Rows
        {0, 1, 2}, {3, 4, 5}, {6, 7, 8},
        // Columns
        {0, 3, 6}, {1, 4, 7}, {2, 5, 8},
        // Diagonals
        {0, 4, 8}, {2, 4, 6}
    };
    
    for (int i = 0; i < 8; ++i) {
        int a = winPatterns[i][0];
        int b = winPatterns[i][1];
        int c = winPatterns[i][2];
        
        if (board[a] != -1 && board[a] == board[b] && board[a] == board[c]) {
            // We have a winner
            if (board[a] == 0) {
                // Player 0 wins
                utilityValues[0] = 1.0;
                utilityValues[1] = -1.0;
            } else {
                // Player 1 wins
                utilityValues[0] = -1.0;
                utilityValues[1] = 1.0;
            }
            isTerminalState = true;
            calculatedTerminal = true;
            return;
        }
    }
    
    // Check for draw
    bool isDraw = true;
    for (int i = 0; i < 9; ++i) {
        if (board[i] == -1) {
            isDraw = false;
            break;
        }
    }
    
    if (isDraw) {
        // Draw (both players get 0)
        utilityValues[0] = 0.0;
        utilityValues[1] = 0.0;
        isTerminalState = true;
    } else {
        isTerminalState = false;
    }
    
    calculatedTerminal = true;
}