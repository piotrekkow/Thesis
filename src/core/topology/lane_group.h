#pragma once

#include <vector>

#include "position.h"

namespace topology {

class LaneGroup {
   public:
    LaneGroup(utils::Position position, double heading,
              std::vector<double> laneWidths = {})
        : position_(position), heading_(heading), laneWidths_(laneWidths) {}
    ~LaneGroup() = default;

    LaneGroup(const LaneGroup&) = delete;
    LaneGroup& operator=(const LaneGroup&) = delete;

    LaneGroup(LaneGroup&&) = default;
    LaneGroup& operator=(LaneGroup&&) = default;

    utils::Position position() const { return position_; }
    double heading() const { return heading_; }
    size_t laneCount() const { return laneWidths_.size(); }

    const std::vector<double>& laneWidths() const { return laneWidths_; }
    void createLane(double width = 3.5);
    void createLanes(int count, double width = 3.5);

   private:
    utils::Position
        position_;  // position relative to position of node exit feeds into
    double heading_;

    std::vector<double> laneWidths_;
};

}  // namespace topology