#include "traversal/GraphTraversal.h"
#include <unordered_set>
#include <fstream>
#include <sstream>

const vector<GraphTraversal::ActionChildPair> GraphTraversal::empty_children_;
const GraphTraversal::ParentActionPair GraphTraversal::empty_parent_{SIZE_MAX, 0};

GraphTraversal::GraphTraversal(shared_ptr<const GameNode> root) {
    performTraversal(root);
}


size_t GraphTraversal::getNodeCount() const {
    return nodes_.size();
}


void GraphTraversal::performTraversal(shared_ptr<const GameNode> root) {
    node_queue_.push(0);
    nodes_.push_back(root);
    
    if (root->getType() == GameNode::Type::Terminal) {
        return;
    }

    while (!node_queue_.empty()) {
        processNextQueuedNode();
    }
}


void GraphTraversal::processNextQueuedNode() {
    size_t current_node_idx = node_queue_.front();
    node_queue_.pop();

    shared_ptr<const GameNode> current_node = nodes_[current_node_idx];
    const vector<int>& legal_actions = current_node->getLegalActions();
    vector<ActionChildPair> children;
    
    for (int action : legal_actions) {
        shared_ptr<const GameNode> child_node = current_node->applyAction(action);
        
        size_t child_idx = nodes_.size();
        nodes_.push_back(child_node);
        
        children.emplace_back(action, child_idx);
        child_to_parent_[child_idx] = {current_node_idx, action};
            
        if (child_node->getType() != GameNode::Type::Terminal) {
            node_queue_.push(child_idx);
        }
    }
        
    if (!children.empty()) {
        parent_to_children_[current_node_idx] = move(children);
    }
}

void GraphTraversal::exportGraph(const string& filename) const {
    ofstream file(filename);
    if (!file.is_open()) {
        throw runtime_error("Could not open file: " + filename);
    }

    file << "{\n";
    file << "  \"nodes\": [\n";
    
    // Export all nodes
    for (size_t i = 0; i < nodes_.size(); ++i) {
        file << "    " << createNodeString(i);
        if (i < nodes_.size() - 1) file << ",";
        file << "\n";
    }
    
    file << "  ],\n";
    file << "  \"edges\": [\n";
    
    bool first_edge = true;
    for (size_t parent_idx = 0; parent_idx < nodes_.size(); ++parent_idx) {
        auto it = parent_to_children_.find(parent_idx);
        if (it != parent_to_children_.end()) {
            for (const auto& [action, child_idx] : it->second) {
                if (!first_edge) file << ",\n";
                file << "    " << createEdgeString(
                    parent_idx, action, child_idx
                );
                first_edge = false;
            }
        }
    }
    
    file << "\n  ]\n";
    file << "}\n";
    
    file.close();
}

string GraphTraversal::createNodeString(size_t node_idx) const {
    const shared_ptr<const GameNode> node = nodes_[node_idx];
    ostringstream oss;
    oss << "{\"id\": " << node_idx \
        << ", \"type\": \"" << node->getTypeString() << "\"" \
        << ", \"representation\": \"" << escape(node->toString()) << "\"" \
        << ", \"address\": \"" << node.get() << "\"";
    
    // Add n_children and actions for non-terminal nodes
    if (node->getType() != GameNode::Type::Terminal) {
        const auto& legal_actions = node->getLegalActions();
        oss << ", \"n_children\": " << legal_actions.size();
        oss << ", \"actions\": [";
        for (size_t j = 0; j < legal_actions.size(); ++j) {
            if (j > 0) oss << ", ";
            oss << legal_actions[j];
        }
        oss << "]";
    }
    
    if (node->getType() == GameNode::Type::Terminal) {
        const auto& utilities = node->getTerminalUtilities();
        oss << ", \"utilities\": [";
        for (size_t j = 0; j < utilities.size(); ++j) {
            if (j > 0) oss << ", ";
            oss << utilities[j];
        }
        oss << "]";

    } else if (node->getType() == GameNode::Type::Decision) {
        oss << ", \"current_player\": " << node->getCurrentPlayer();
        oss << ", \"info_set\": \"" << node->getInfoSetKeyString() << "\"";

    } else if (node->getType() == GameNode::Type::Chance) {
        const auto& probabilities = node->getChanceProbabilities();
        oss << ", \"probabilities\": [";
        for (size_t j = 0; j < probabilities.size(); ++j) {
            if (j > 0) oss << ", ";
            oss << probabilities[j];
        }
        oss << "]";
    }
    
    oss << "}";
    return oss.str();
}

string GraphTraversal::createEdgeString(
    size_t parent_idx, 
    int action, 
    size_t child_idx
) const {
    const shared_ptr<const GameNode> parent = nodes_[parent_idx];
    ostringstream oss;
    oss << "{\"source\": " << parent_idx \
        << ", \"target\": " << child_idx \
        << ", \"action\": " << action;

    string action_str = parent->actionToString(action);
    oss << ", \"action_string\": \"" << escape(action_str) << "\"";

    oss << "}";
    return oss.str();
}

string GraphTraversal::escape(const string& str) {
    string result;
    result.reserve(str.size() * 2); // Reserve space to avoid frequent reallocations
    
    for (char c : str) {
        switch (c) {
            case '\n':
                result += "\\n";
                break;
            case '\r':
                result += "\\r";
                break;
            case '\t':
                result += "\\t";
                break;
            case '"':
                result += "\\\"";
                break;
            case '\\':
                result += "\\\\";
                break;
            case '/':
                result += "\\/";
                break;
            default:
                result += c;
                break;
        }
    }
    
    return result;
}