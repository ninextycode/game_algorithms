#pragma once

#include "abstract/nodes/game_node.h"
#include <type_traits>

using namespace std;
// T should extend TypedGameNode<T>
template<typename T>
requires is_base_of_v<TypedGameNode<T>, T>
class Iterator {
public:
    Iterator() = default;
    virtual ~Iterator() = default;

    virtual bool hasNext() const = 0;
    virtual shared_ptr<const T> next() const = 0;
};
