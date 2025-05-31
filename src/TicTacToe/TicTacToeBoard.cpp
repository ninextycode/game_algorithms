#include "tictactoe/TicTacToeBoard.h"
#include <sstream>
#include <unordered_set>
#include <algorithm>


TicTacToeBoard::TicTacToeBoard() : current_player(0) {
    // Initialize empty board (-1 means empty)
    for (auto& row : board) {
        row.fill(-1);
    }
}


TicTacToeBoard::TicTacToeBoard(const array<array<int, 3>, 3>& board) 
    : board(board) {
    current_player = determineCurrentPlayer();
}

TicTacToeBoard::TicTacToeBoard(const array<int, 9>& flatBoard) {
    // Convert 1D array to 2D
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            board[i][j] = flatBoard[i * 3 + j];
        }
    }
    current_player = determineCurrentPlayer();
}

int TicTacToeBoard::determineCurrentPlayer() const {
    // Count pieces to determine current player
    int count_x = 0, count_o = 0;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] == 0) count_x++;
            else if (board[i][j] == 1) count_o++;
        }
    }
    // If x's (0s) count is equal or less than o's (1s), it's x's turn (player 0)
    return (count_x <= count_o) ? 0 : 1;
}

TicTacToeBoard TicTacToeBoard::copy() const {
    TicTacToeBoard result(board);
    return result;
}

void TicTacToeBoard::verticalFlip() {
    // Swap row 0 and row 2
    swap(board[0], board[2]);
}

void TicTacToeBoard::horizontalFlip() {
    // Swap column 0 and column 2 for each row
    for (int i = 0; i < 3; i++) {
        swap(board[i][0], board[i][2]);
    }
}

void TicTacToeBoard::rotateClockwise() {
    array<array<int, 3>, 3> newBoard;
    // Perform 90° clockwise rotation: new[j][2-i] = old[i][j]
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            newBoard[j][2-i] = board[i][j];
        }
    }
    board = newBoard;
}

void TicTacToeBoard::rotateCounterclockwise() {
    array<array<int, 3>, 3> newBoard;
    // Perform 90° counterclockwise rotation: new[2-j][i] = old[i][j]
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            newBoard[2-j][i] = board[i][j];
        }
    }
    board = newBoard;
}

int TicTacToeBoard::get(int row, int col) const {
    return board[row][col];
}

int TicTacToeBoard::get(int flat_index) const {
    int row = flat_index / 3;
    int col = flat_index % 3;
    return board[row][col];
}

const array<array<int, 3>, 3>& TicTacToeBoard::getBoard() const {
    return board;
}

array<int, 9> TicTacToeBoard::getFlatBoard() const {
    array<int, 9> flatBoard;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            flatBoard[i * 3 + j] = board[i][j];
        }
    }
    return flatBoard;
}

string TicTacToeBoard::toString() const {
    std::ostringstream oss;
    for (int i = 0; i < 3; i++) {
        if (i > 0) {
            oss << "\n";
        }
        for (int j = 0; j < 3; j++) {
            if (board[i][j] == -1) {
                oss << ".";
            } else if (board[i][j] == 0) {
                oss << "x";
            } else {
                oss << "o";
            }
            
            if (j < 2) {
                oss << " ";
            }
        }
    }
    return oss.str();
}

bool TicTacToeBoard::isEmpty(int row, int col) const {
    return board[row][col] == -1;
}

bool TicTacToeBoard::isEmpty(int flat_index) const {
    return get(flat_index) == -1;
}

int TicTacToeBoard::getCurrentPlayer() const {
    return current_player;
}

void TicTacToeBoard::makeMove(int row, int col) {
    board[row][col] = current_player;
    current_player = 1 - current_player; // Switch player
}

void TicTacToeBoard::makeMove(int flat_index) {
    int row = flat_index / 3;
    int col = flat_index % 3;
    this->makeMove(row, col);
}

bool TicTacToeBoard::operator<(const TicTacToeBoard& other) const {
    // Compare each position in row-major order
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] < other.board[i][j]) {
                return true;
            } else if (board[i][j] > other.board[i][j]) {
                return false;
            }
            // If equal, continue to next position
        }
    }
    // All positions are equal
    return false;
}

bool TicTacToeBoard::operator==(const TicTacToeBoard& other) const {
    if (current_player != other.current_player) {
        return false;
    }
    
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] != other.board[i][j]) {
                return false;
            }
        }
    }
    return true;
}

TicTacToeBoard TicTacToeBoard::getNormalForm() const {
    // Create all 8 possible invariants
    vector<TicTacToeBoard> invariants;
    
    // Original board
    TicTacToeBoard board0 = this->copy();
    invariants.push_back(board0);
    
    // 90 degree rotation
    TicTacToeBoard board90 = this->copy();
    board90.rotateClockwise();
    invariants.push_back(board90);
    
    // 180 degree rotation
    TicTacToeBoard board180 = board90.copy();
    board180.rotateClockwise();
    invariants.push_back(board180);
    
    // 270 degree rotation
    TicTacToeBoard board270 = board180.copy();
    board270.rotateClockwise();
    invariants.push_back(board270);
    
    // Vertical flip
    TicTacToeBoard boardFlip = this->copy();
    boardFlip.verticalFlip();
    invariants.push_back(boardFlip);
    
    // Vertical flip + 90 degree rotation
    TicTacToeBoard boardFlip90 = boardFlip.copy();
    boardFlip90.rotateClockwise();
    invariants.push_back(boardFlip90);
    
    // Vertical flip + 180 degree rotation
    TicTacToeBoard boardFlip180 = boardFlip90.copy();
    boardFlip180.rotateClockwise();
    invariants.push_back(boardFlip180);
    
    // Vertical flip + 270 degree rotation
    TicTacToeBoard boardFlip270 = boardFlip180.copy();
    boardFlip270.rotateClockwise();
    invariants.push_back(boardFlip270);
    
    // Find the smallest invariant
    TicTacToeBoard normalForm = invariants[0];
    for (size_t i = 1; i < invariants.size(); i++) {
        if (invariants[i] < normalForm) {
            normalForm = invariants[i];
        }
    }
    
    return normalForm;
}

bool TicTacToeBoard::isTerminal() const {
    // Game is terminal if there's a winner or it's a draw
    int winner = getWinner();
    return winner != -2;
}

int TicTacToeBoard::getWinner() const {
    // Check for win patterns
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
        
        if (!isEmpty(a) && 
            get(a) == get(b) && 
            get(a) == get(c)) {
            // We have a winner
            return get(a); // Returns 0 or 1
        }
    }
    
    // Check for draw - if board is full
    if (isFull()) {
        return -1; // Draw
    }
    
    // Game still in progress
    return -2;
}

bool TicTacToeBoard::isFull() const {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] == -1) {
                return false; // Found an empty cell
            }
        }
    }
    return true; // All cells are filled
}
TicTacToeBoard TicTacToeBoard::fromString(const string& str) {
    array<array<int, 3>, 3> board;
    size_t pos = 0;
    
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            // Skip whitespace and newlines
            while (pos < str.length() && (str[pos] == ' ' || str[pos] == '\n')) {
                pos++;
            }
            
            if (pos >= str.length()) {
                // Not enough characters, leave as empty
                board[i][j] = -1;
            } else {
                char c = str[pos];
                if (c == '.' || c == '-') {
                    board[i][j] = -1; // Empty
                } else if (c == 'x' || c == 'X' || c == '0') {
                    board[i][j] = 0; // Player 0
                } else if (c == 'o' || c == 'O' || c == '1') {
                    board[i][j] = 1; // Player 1
                } else {
                    board[i][j] = -1; // Default to empty for unknown chars
                }
                pos++;
            }
        }
    }
    
    return TicTacToeBoard(board);
}

vector<int> TicTacToeBoard::getPossibleActions() const {
    vector<int> actions;
    
    for (int i = 0; i < 9; i++) {
        if (isEmpty(i)) {
            actions.push_back(i);
        }
    }
    
    return actions;
}

vector<int> TicTacToeBoard::getUniqueInvariantActions() const {
    vector<int> all_actions = getPossibleActions();
    vector<int> filtered_actions;
    unordered_set<string> seen_normal_forms;
    
    for (int action : all_actions) {
        // Create a copy of the current board
        TicTacToeBoard test_board = this->copy();
        
        // Apply the action to get the resulting board
        test_board.makeMove(action);
        
        // Get the normal form of the resulting board
        TicTacToeBoard normal_form = test_board.getNormalForm();
        
        // Convert normal form to string for comparison
        string normal_form_string = normal_form.toString();
        
        // If we haven't seen this normal form before, keep this action
        if (seen_normal_forms.find(normal_form_string) == seen_normal_forms.end()) {
            seen_normal_forms.insert(normal_form_string);
            filtered_actions.push_back(action);
        }
        // If we've seen this normal form before, discard this action (keep earlier one)
    }
    
    return filtered_actions;
}