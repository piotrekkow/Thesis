#include "topology/lane_group.h"

namespace topology {

void LaneGroup::createLane(double width) { laneWidths_.push_back(width); }

void LaneGroup::createLanes(int count, double width) {
    for (int i = 0; i < count; i++) {
        createLane(width);
    }
}

}  // namespace topology