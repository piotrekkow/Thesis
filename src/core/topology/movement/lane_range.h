#pragma once
#include <cstddef>

namespace topology {

class LaneRange {
   public:
    LaneRange(int single);
    LaneRange(int f, int l);

    size_t count() const { return last_ - first_ + 1; }
    bool contains(int lane) const { return lane >= first_ && lane <= last_; }
    size_t sharedLaneCount(const LaneRange& other) const;
    size_t first() const { return first_; }
    size_t last() const { return last_; }

   private:
    size_t first_;
    size_t last_;

    bool overlaps(const LaneRange& other) const {
        return first_ <= other.last_ && last_ >= other.first_;
    }
};

}  // namespace topology