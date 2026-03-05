#pragma once
#include <cstddef>

namespace topology {

class LaneRange {
   public:
    LaneRange(int single);
    LaneRange(int f, int l);

    size_t count() const;
    bool contains(int lane) const;
    size_t sharedLaneCount(const LaneRange& other) const;

    int first() const { return first_; }
    int last() const { return last_; }

   private:
    int first_;
    int last_;

    bool overlaps(const LaneRange& other) const {
        return first_ <= other.last_ && last_ >= other.first_;
    }
};

}  // namespace topology