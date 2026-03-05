#include "topology/movement/lane_range.h"

#include <algorithm>
#include <stdexcept>

namespace topology {

LaneRange::LaneRange(int single) : first_(single), last_(single) {
    if (single < 0)
        throw std::invalid_argument("Lane index must be non-negative");
}

LaneRange::LaneRange(int f, int l) : first_(f), last_(l) {
    if (f < 0 || l < 0 || f > l)
        throw std::invalid_argument("Invalid lane range");
}

size_t LaneRange::count() const {
    return static_cast<size_t>(last_ - first_ + 1);
}

size_t LaneRange::sharedLaneCount(const LaneRange& other) const {
    if (!overlaps(other)) return 0;

    int overlapFirst = std::max(first_, other.first_);
    int overlapLast = std::min(last_, other.last_);

    return static_cast<size_t>(overlapLast - overlapFirst + 1);
}

}  // namespace topology