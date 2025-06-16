#include "tictactoe/TTTInvariant.h"
#include <unordered_set>
#include <sstream>

TTTInvariant::TTTInvariant()
    : TTTInvariant(TicTacToeNode()) 
{ }

TTTInvariant::TTTInvariant(const TicTacToeNode& internal_ttt_node)
     : internal_ttt_node_(internal_ttt_node),
    legal_invariant_actions_(
        internal_ttt_node_.getBoard().getUniqueInvariantActions()
    )
{ }

GameNode::Type TTTInvariant::getType() const {
    return internal_ttt_node_.getType();
}

const vector<double>& TTTInvariant::getTerminalUtilities() const {
    return internal_ttt_node_.getTerminalUtilities();
}

const vector<double>& TTTInvariant::getChanceProbabilities() const {
    return internal_ttt_node_.getChanceProbabilities();
}

int TTTInvariant::getCurrentPlayer() const {
    return internal_ttt_node_.getCurrentPlayer();
}

const vector<int>& TTTInvariant::getLegalActions() const {
    return legal_invariant_actions_;
}

shared_ptr<const GameNode> TTTInvariant::applyAction(int action) const {
    // Get the next TicTacToeNode from the wrapped node
    shared_ptr<const TicTacToeNode> next_internal_node = \
        static_pointer_cast<const TicTacToeNode>(
            internal_ttt_node_.applyAction(action)
        );

    // Convert board to normal form to avoid confusion in action number correspondence with actions on the board
    // The board on which actions are taken should be in normal form
    TicTacToeBoard normal_board = next_internal_node->getBoard().getNormalForm();
    TicTacToeNode normalized_node(normal_board);
    
    // Wrap it in a new TTTInvariant
    return make_shared<const TTTInvariant>(normalized_node);
}

size_t TTTInvariant::getInfoSetKeyInt() const {
    return internal_ttt_node_.getInfoSetKeyInt();
}

string TTTInvariant::getInfoSetKeyString() const {
    return internal_ttt_node_.getInfoSetKeyString();
}
