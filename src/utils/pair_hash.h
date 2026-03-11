#pragma once

#include <functional>

namespace utils {

// Generic combiner — use this as a building block
inline void hashCombine(std::size_t& seed, std::size_t hash) noexcept {
    seed ^= hash + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

struct PairHash {
    template <class T1, class T2>
    std::size_t operator()(const std::pair<T1, T2>& p) const noexcept {
        std::size_t seed = std::hash<T1>{}(p.first);
        hashCombine(seed, std::hash<T2>{}(p.second));
        return seed;
    }
};

}  // namespace utils