#pragma once

#include "traversal/iterator.h"
#include <stdexcept>
#include <vector>

using namespace std;

template<typename T>
requires is_base_of_v<TypedGameNode<T>, T>
class DFSIterator : public Iterator<T> {
public:
    explicit DFSIterator(shared_ptr<const T> root = nullptr);

    bool hasNext() const override;
    shared_ptr<const T> next() const override;

private:
    void pushChildren(const shared_ptr<const T>& node) const;

    mutable vector<shared_ptr<const T>> node_stack_;
};

#include "traversal/dfs_iterator.hpp"
