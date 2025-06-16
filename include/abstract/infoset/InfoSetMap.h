#pragma once
#include "abstract/infoset/InfoSet.h"
#include <concepts>

using namespace std;

template <typename Key>
concept InfoSetKey = \
    is_same_v<Key, string> || \
    is_same_v<Key, size_t>;

template <InfoSetKey Key>
using InfoSetMap = unordered_map<Key, InfoSet>;