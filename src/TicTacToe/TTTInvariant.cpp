#include "tictactoe/TTTInvariant.h"
#include <unordered_set>
#include <sstream>

TTTInvariant::TTTInvariant()
    : internal_ttt_node() {
}

TTTInvariant::TTTInvariant(const TicTacToeNode& internal_ttt_node)
    : internal_ttt_node(internal_ttt_node) {
}

bool TTTInvariant::isTerminal() const {
    return internal_ttt_node.isTerminal();
}

vector<double> TTTInvariant::getUtilitiesForTerminal() const {
    return internal_ttt_node.getUtilitiesForTerminal();
}

bool TTTInvariant::isChance() const {
    return internal_ttt_node.isChance();
}

vector<double> TTTInvariant::getChanceProbabilities() const {
    return internal_ttt_node.getChanceProbabilities();
}

int TTTInvariant::getCurrentPlayer() const {
    return internal_ttt_node.getCurrentPlayer();
}

vector<int> TTTInvariant::getAvailableActions() const {
    // Use the new TicTacToeBoard method directly
    return internal_ttt_node.getBoard().getUniqueInvariantActions();
}

shared_ptr<GameNode> TTTInvariant::nextGameNode(int action) const {
    // Get the next TicTacToeNode from the wrapped node
    shared_ptr<TicTacToeNode> next_internal_node = static_pointer_cast<TicTacToeNode>(
        internal_ttt_node.nextGameNode(action)
    );

    // Convert board to normal form to avoid confusion in action number correspondence with actions on (which?) board
    TicTacToeBoard normal_board = next_internal_node->getBoard().getNormalForm();
    TicTacToeNode normalized_node(normal_board);
    
    // Wrap it in a new TTTInvariant
    return make_shared<TTTInvariant>(move(normalized_node));
}

string TTTInvariant::getInfoSetKey() const {
    return internal_ttt_node.getBoard().toString();
}

