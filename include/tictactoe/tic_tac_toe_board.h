#pragma once

#include <array>
#include <string>
#include <vector>

using namespace std;

class TicTacToeBoard {
public:
    // Constructors
    TicTacToeBoard(); // Create empty board, player 0 starts
    TicTacToeBoard(const array<array<int, 3>, 3>& board); // From 2D array
    TicTacToeBoard(const array<int, 9>& flatBoard); // From flat 1D array (for compatibility)
    
    // Create a copy of the board
    TicTacToeBoard copy() const;
    
    // Board transformations (mutable - modify this instance)
    void verticalFlip(); // Swap rows 0 and 2
    void horizontalFlip(); // Swap columns 0 and 2
    void rotateClockwise(); // 90 degree clockwise
    void rotateCounterclockwise(); // 90 degree counterclockwise
    
    // Accessors
    int get(int row, int col) const; // Get value at position
    int get(int flat_index) const; // Get using flat index
    const array<array<int, 3>, 3>& getBoard() const; // Get full 2D board
    array<int, 9> getFlatBoard() const; // Get flattened board
    int getCurrentPlayer() const; // Get current player
    
    // Utility
    string toString() const;
    bool isEmpty(int row, int col) const;
    bool isEmpty(int flat_index) const;
    
    // Mutators
    void makeMove(int row, int col); // Make move for current player and switch
    void makeMove(int flat_index); // Make move for current player and switch

    // Compare two boards lexicographically (row by row)
    bool operator<(const TicTacToeBoard& other) const;
    bool operator==(const TicTacToeBoard& other) const;
    
    // Get the "normal form" of the board - the smallest among all 8 invariants
    TicTacToeBoard getNormalForm() const;

    // Check if the board is in a terminal state (game over)
    bool isTerminal() const;

    // Get the winner of the game:
    // 0 - player 0 (X) wins
    // 1 - player 1 (O) wins
    // -1 - draw
    // -2 - game in progress
    int getWinner() const;

    // Check if the board is full (no empty cells)
    bool isFull() const;

    // Create board from string representation
    static TicTacToeBoard fromString(const string& str);
 
    // Get flat indices of all empty cells
    vector<int> getPossibleActions() const;

    // Get unique actions based on normal form invariants
    vector<int> getUniqueInvariantActions() const;

private:
    int determineCurrentPlayer() const;
    array<array<int, 3>, 3> board;
    int current_player; // 0 or 1
};