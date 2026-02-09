#pragma once

#include <vector>

#include "position.h"

class LaneGroup {
   public:
    LaneGroup(geom::Position position, double heading,
              std::vector<double> laneWidths = {})
        : position_(position), heading_(heading), laneWidths_(laneWidths) {}
    ~LaneGroup() = default;

    LaneGroup(const LaneGroup&) = delete;
    LaneGroup& operator=(const LaneGroup&) = delete;

    LaneGroup(LaneGroup&&) = default;
    LaneGroup& operator=(LaneGroup&&) = default;

    geom::Position position() const { return position_; }
    double heading() const { return heading_; }
    size_t laneCount() const { return laneWidths_.size(); }

   private:
    geom::Position
        position_;  // position relative to position of node exit feeds into
    double heading_;

    std::vector<double> laneWidths_;
};