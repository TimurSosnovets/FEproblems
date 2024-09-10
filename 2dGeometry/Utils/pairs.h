#pragma once
#include <vector>

template <typename T>
std::vector<std::pair<T,T>> make_round_pairs(const std::vector<T>& sequence) {
    int lenght = sequence.size();
    std::vector<std::pair<T,T>> pairs;
    for (int i = 0; i < lenght; ++i) {
        pairs.emplace_back(sequence[i], sequence[(i+1) % lenght]);
    }
    return pairs;
};
