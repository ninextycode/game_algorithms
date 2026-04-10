#pragma once

#include "traversal/dfs_iterator.h"

template<typename T> requires is_base_of_v<TypedGameNode<T>, T>
DFSIterator<T>::DFSIterator(shared_ptr<const T> root) {
    if (root) {
        node_stack_.push_back(root);
    }
}

template<typename T> requires is_base_of_v<TypedGameNode<T>, T>
bool DFSIterator<T>::hasNext() const {
    return !node_stack_.empty();
}

template<typename T> requires is_base_of_v<TypedGameNode<T>, T>
shared_ptr<const T> DFSIterator<T>::next() const {
    if (node_stack_.empty()) {
        throw out_of_range("DFSIterator::next called with no remaining nodes");
    }

    shared_ptr<const T> current = node_stack_.back();
    node_stack_.pop_back();

    pushChildren(current);
    return current;
}

template<typename T> requires is_base_of_v<TypedGameNode<T>, T>
void DFSIterator<T>::pushChildren(const shared_ptr<const T>& node) const {
    if (node->getType() == GameNode::Type::Terminal) {
        return;
    }

    const vector<int>& legal_actions = node->getLegalActions();
    for (auto it = legal_actions.rbegin(); it != legal_actions.rend(); ++it) {
        node_stack_.push_back(node->applyActionTyped(*it));
    }
}
