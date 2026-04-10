#pragma once
#include "abstract/regret_data/regret_data.h"
#include <unordered_map>
#include <string>
#include <type_traits>

using namespace std;

template <typename Key>
concept RegretDataKey = \
    is_same_v<Key, string> || \
    is_same_v<Key, size_t>;

template <RegretDataKey Key>
using RegretDataMap = unordered_map<Key, RegretData>;