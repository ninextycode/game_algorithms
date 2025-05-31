#include "tictactoe/TicTacToeNode.h"

// Constructor for initial state
TicTacToeNode::TicTacToeNode() 
    : is_terminal_state(false), utility_values({0, 0}) {
    // TicTacToeBoard default constructor already initializes an empty board
}

// Constructor for state after move using TicTacToeBoard
TicTacToeNode::TicTacToeNode(const TicTacToeBoard& board)
    : board(board) {

    calculateTerminalAndUtility();
}

// Copy constructor
TicTacToeNode::TicTacToeNode(const TicTacToeNode& other)
    : board(other.board), 
      is_terminal_state(other.is_terminal_state),
      utility_values(other.utility_values) {
}

// Move constructor
TicTacToeNode::TicTacToeNode(const TicTacToeNode&& other)
    : board(std::move(other.board)),
      is_terminal_state(other.is_terminal_state),
      utility_values(std::move(other.utility_values)) {
}


bool TicTacToeNode::isTerminal() const {
    return is_terminal_state;
}

std::vector<double> TicTacToeNode::getUtilitiesForTerminal() const {
    return utility_values;
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
    return board.getCurrentPlayer();
}

std::vector<int> TicTacToeNode::getAvailableActions() const {
    std::vector<int> actions;
    for (int i = 0; i < 9; ++i) {
        if (board.isEmpty(i)) {
            actions.push_back(i);
        }
    }
    return actions;
}

std::shared_ptr<GameNode> TicTacToeNode::nextGameNode(int action) const {
    // Create a copy of the current board
    TicTacToeBoard new_board = board.copy();
    
    // Apply the action to the new board
    new_board.makeMove(action);
    
    // Return new state with updated board and next player
    return std::make_shared<TicTacToeNode>(new_board);
}

std::string TicTacToeNode::getInfoSetKey() const {
    // In perfect information game like tic-tac-toe, the info set is just the board state
    return getBoardString();
}

std::string TicTacToeNode::getBoardString() const {
    return board.toString();
}

const TicTacToeBoard& TicTacToeNode::getBoard() const {
    return board;
}

array<int, 9> TicTacToeNode::getBoardArray() const {
    return board.getFlatBoard();
}

void TicTacToeNode::calculateTerminalAndUtility() {
    // Initialize utility for two players
    utility_values = {0.0, 0.0};
    
    // Determine if the game is terminal using board's isTerminal method
    int winner = board.getWinner();
    is_terminal_state = winner != -2;
    
    if (is_terminal_state) {    
        if (winner == -1) {
            // Draw - both players get 0 utility
            utility_values[0] = 0.0;
            utility_values[1] = 0.0;
        } else if (winner == 0) {
            // Player 0 wins
            utility_values[0] = 1.0;
            utility_values[1] = -1.0;
        } else if (winner == 1) {
            // Player 1 wins
            utility_values[0] = -1.0;
            utility_values[1] = 1.0;
        }
    }
}