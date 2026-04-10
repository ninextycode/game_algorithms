#pragma once

#include "traversal/graph_traversal.h"
#include <fstream>
#include <sstream>

template<typename T>
requires is_base_of_v<TypedGameNode<T>, T>
const vector<typename GraphTraversal<T>::ActionChildPair> GraphTraversal<T>::empty_children_;

template<typename T>
requires is_base_of_v<TypedGameNode<T>, T>
GraphTraversal<T>::GraphTraversal(shared_ptr<const T> root, int depth_limit)
    : depth_limit_(depth_limit) {
    performTraversal(root);
}

template<typename T>
requires is_base_of_v<TypedGameNode<T>, T>
size_t GraphTraversal<T>::getNodeCount() const {
    return nodes_.size();
}

template<typename T>
requires is_base_of_v<TypedGameNode<T>, T>
shared_ptr<const T> GraphTraversal<T>::getNode(size_t node_id) const {
    if (node_id >= nodes_.size()) {
        throw out_of_range("GraphTraversal::getNode: node_id " + to_string(node_id) + 
                          " out of range [0, " + to_string(nodes_.size()) + ")");
    }
    return nodes_[node_id];
}

template<typename T>
requires is_base_of_v<TypedGameNode<T>, T>
size_t GraphTraversal<T>::getChildId(size_t parent_id, int action) const {
    if (parent_id >= nodes_.size()) {
        throw out_of_range("GraphTraversal::getChildId: parent_id " + to_string(parent_id) + 
                          " out of range [0, " + to_string(nodes_.size()) + ")");
    }
    
    auto it = parent_to_children_.find(parent_id);
    if (it == parent_to_children_.end()) {
        throw out_of_range("GraphTraversal::getChildId: parent_id " + to_string(parent_id) + 
                          " has no children");
    }
    
    for (const auto& [child_action, child_id] : it->second) {
        if (child_action == action) {
            return child_id;
        }
    }
    
    throw out_of_range("GraphTraversal::getChildId: action " + to_string(action) + 
                      " not found for parent_id " + to_string(parent_id));
}

template<typename T>
requires is_base_of_v<TypedGameNode<T>, T>
size_t GraphTraversal<T>::getParentId(size_t child_id) const {
    return getParentAndAction(child_id).first;
}

template<typename T>
requires is_base_of_v<TypedGameNode<T>, T>
typename GraphTraversal<T>::ParentActionPair GraphTraversal<T>::getParentAndAction(size_t child_id) const {
    if (child_id >= nodes_.size()) {
        throw out_of_range("GraphTraversal::getParentAndAction: child_id " + to_string(child_id) + 
                          " out of range [0, " + to_string(nodes_.size()) + ")");
    }
    
    if (child_id == 0) {
        throw out_of_range("GraphTraversal::getParentAndAction: root node (id=0) has no parent");
    }
    
    auto it = child_to_parent_.find(child_id);
    if (it == child_to_parent_.end()) {
        throw out_of_range("GraphTraversal::getParentAndAction: child_id " + to_string(child_id) + 
                          " has no parent mapping");
    }
    
    return it->second;
}

template<typename T>
requires is_base_of_v<TypedGameNode<T>, T>
const vector<typename GraphTraversal<T>::ActionChildPair>& GraphTraversal<T>::getChildren(size_t parent_id) const {
    if (parent_id >= nodes_.size()) {
        throw out_of_range("GraphTraversal::getChildren: parent_id " + to_string(parent_id) + 
                          " out of range [0, " + to_string(nodes_.size()) + ")");
    }
    
    auto it = parent_to_children_.find(parent_id);
    if (it == parent_to_children_.end()) {
        return empty_children_;
    }
    return it->second;
}

template<typename T>
requires is_base_of_v<TypedGameNode<T>, T>
void GraphTraversal<T>::performTraversal(shared_ptr<const T> root) {
    node_queue_.push({0, 0});
    nodes_.push_back(root);
    
    if (root->getType() == GameNode::Type::Terminal) {
        return;
    }

    while (!node_queue_.empty()) {
        processNextQueuedNode();
    }
}

template<typename T>
requires is_base_of_v<TypedGameNode<T>, T>
void GraphTraversal<T>::processNextQueuedNode() {
    auto [current_node_idx, current_depth] = node_queue_.front();
    node_queue_.pop();

    shared_ptr<const T> current_node = nodes_[current_node_idx];
    const vector<int>& legal_actions = current_node->getLegalActions();
    vector<ActionChildPair> children;
    
    int child_depth = current_depth + 1;
    bool within_depth_limit = (depth_limit_ < 0) || (child_depth < depth_limit_);
    
    for (int action : legal_actions) {
        shared_ptr<const T> child_node;
        child_node = current_node->applyActionTyped(action);

        size_t child_idx = nodes_.size();
        nodes_.push_back(child_node);
        
        children.emplace_back(action, child_idx);
        child_to_parent_[child_idx] = {current_node_idx, action};
            
        if (child_node->getType() != GameNode::Type::Terminal && within_depth_limit) {
            node_queue_.push({child_idx, child_depth});
        }
    }
        
    if (!children.empty()) {
        parent_to_children_[current_node_idx] = std::move(children);
    }
}

template<typename T>
requires is_base_of_v<TypedGameNode<T>, T>
void GraphTraversal<T>::exportGraph(const string& filename) const {
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

template<typename T>
requires is_base_of_v<TypedGameNode<T>, T>
string GraphTraversal<T>::createNodeString(size_t node_idx) const {
    const shared_ptr<const T> node = nodes_[node_idx];
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

template<typename T>
requires is_base_of_v<TypedGameNode<T>, T>
string GraphTraversal<T>::createEdgeString(
    size_t parent_idx, 
    int action, 
    size_t child_idx
) const {
    const shared_ptr<const T> parent = nodes_[parent_idx];
    ostringstream oss;
    oss << "{\"source\": " << parent_idx \
        << ", \"target\": " << child_idx \
        << ", \"action\": " << action;

    string action_str = parent->actionToString(action);
    oss << ", \"action_string\": \"" << escape(action_str) << "\"";

    oss << "}";
    return oss.str();
}

template<typename T>
requires is_base_of_v<TypedGameNode<T>, T>
string GraphTraversal<T>::escape(const string& str) {
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

