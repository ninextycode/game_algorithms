#pragma once

#include "traversal/iterator.h"
#include <queue>

using namespace std;

template<typename T> requires is_base_of_v<TypedGameNode<T>, T>
class BFSIterator : public Iterator<T> {
public:
    explicit BFSIterator(shared_ptr<const T> root = nullptr);

    bool hasNext() const override;
    shared_ptr<const T> next() const override;

private:
    void enqueueChildren(const shared_ptr<const T>& node) const;

    mutable queue<shared_ptr<const T>> node_queue_;
};

#include "traversal/bfs_iterator.hpp"
