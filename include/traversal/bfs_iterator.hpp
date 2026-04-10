#pragma once

#include "traversal/bfs_iterator.h"

template<typename T> requires is_base_of_v<TypedGameNode<T>, T>
BFSIterator<T>::BFSIterator(shared_ptr<const T> root) {
    if (root) {
        node_queue_.push(root);
    }
}

template<typename T> requires is_base_of_v<TypedGameNode<T>, T>
bool BFSIterator<T>::hasNext() const {
    return !node_queue_.empty();
}

template<typename T> requires is_base_of_v<TypedGameNode<T>, T>
shared_ptr<const T> BFSIterator<T>::next() const {
    if (node_queue_.empty()) {
        throw out_of_range("BFSIterator::next called with no remaining nodes");
    }

    shared_ptr<const T> current = node_queue_.front();
    node_queue_.pop();

    enqueueChildren(current);
    return current;
}

template<typename T> requires is_base_of_v<TypedGameNode<T>, T>
void BFSIterator<T>::enqueueChildren(const shared_ptr<const T>& node) const {
    if (node->getType() == GameNode::Type::Terminal) {
        return;
    }

    const vector<int>& legal_actions = node->getLegalActions();
    for (int action : legal_actions) {
        node_queue_.push(node->applyActionTyped(action));
    }
}
