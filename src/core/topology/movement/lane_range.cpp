#include "lane_range.h"

#include <algorithm>
#include <stdexcept>

LaneRange::LaneRange(int single) : first_(single), last_(single) {}

LaneRange::LaneRange(int f, int l) : first_(f), last_(l) {
    if (f > l) throw std::invalid_argument("Invalid lane range");
}

size_t LaneRange::sharedLaneCount(const LaneRange& other) const {
    if (!overlaps(other)) return 0;
    size_t overlapFirst = std::max(first_, other.first_);
    size_t overlapLast = std::min(last_, other.last_);
    return overlapLast - overlapFirst + 1;
}