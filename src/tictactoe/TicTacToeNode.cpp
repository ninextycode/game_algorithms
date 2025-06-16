#include "tictactoe/TicTacToeNode.h"
#include <cmath>

// Constructor for initial state
TicTacToeNode::TicTacToeNode() 
    : TicTacToeNode(TicTacToeBoard()) 
{ }

// Constructor for state after move using TicTacToeBoard
TicTacToeNode::TicTacToeNode(const TicTacToeBoard& board)
    : board(board) {
    calculateProperties();
}


void TicTacToeNode::calculateProperties() {
    int winner = board.getWinner();
    is_terminal_state = winner != -2;
    
    if (is_terminal_state) {    
        // calculate utilities for terminal node
        if (winner == -1) {
            // Draw - both players get 0 utility
            utility_values = {0.0, 0.0};
        } else if (winner == 0) {
            // Player 0 wins
            utility_values = {1.0, -1.0};
        } else if (winner == 1) {
            // Player 1 wins
            utility_values = {-1.0, 1.0};
        }
    } else {
        // calculate legal actions for non-terminal node
        legal_actions = board.getPossibleActions();
    }
}


GameNode::Type TicTacToeNode::getType() const {
    return is_terminal_state ? Type::Terminal : Type::Decision;
}

const vector<double>& TicTacToeNode::getTerminalUtilities() const {
    return utility_values;
}

int TicTacToeNode::getCurrentPlayer() const {
    return board.getCurrentPlayer();
}

const vector<int>& TicTacToeNode::getLegalActions() const {
    return legal_actions;
}

shared_ptr<const GameNode> TicTacToeNode::applyAction(int action) const {
    TicTacToeBoard new_board = board.copy();
    new_board.makeMove(action);
    return make_shared<TicTacToeNode>(new_board);
}

string TicTacToeNode::getInfoSetKeyString() const {
    return getBoardString();
}

string TicTacToeNode::getBoardString() const {
    return board.toString();
}

size_t TicTacToeNode::getInfoSetKeyInt() const {
    size_t key = 10e10;
    for (size_t i = 0; i < 9; i++) {
        key += pow(10, 9 - i) * (board.get(i) + 1);
    }
    return key;
}

const TicTacToeBoard& TicTacToeNode::getBoard() const {
    return board;
}