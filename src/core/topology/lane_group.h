#pragma once

#include <optional>
#include <unordered_map>
#include <vector>

#include "id.h"
#include "id_generator.h"
#include "position.h"
#include "topology/lane.h"

namespace topology {

template <typename LaneTagT>
class LaneGroup {
   public:
    using LaneIdT = Id<LaneTagT, EdgeId>;

    LaneGroup(utils::Position position, double heading)
        : position_(position), heading_(heading), speedLimit_(13.88) {}
    ~LaneGroup() = default;

    LaneGroup(const LaneGroup&) = delete;
    LaneGroup& operator=(const LaneGroup&) = delete;

    LaneGroup(LaneGroup&&) = default;
    LaneGroup& operator=(LaneGroup&&) = default;

    utils::Position position() const { return position_; }
    double heading() const { return heading_; }
    size_t laneCount() const { return lanes_.size(); }

    // Lane access
    const Lane& lane(size_t index) const { return lanes_[index]; }
    const Lane& lane(LaneIdT id) const { return lanes_[laneIndex_.at(id)]; }

    // Ordered lane IDs
    const std::vector<LaneIdT>& laneIds() const { return laneIds_; }
    LaneIdT laneId(size_t index) const { return laneIds_[index]; }

    // Index lookup by LaneId
    size_t indexOf(LaneIdT id) const { return laneIndex_.at(id); }

    // Width compatibility accessor (for geometry calculations)
    std::vector<double> laneWidths() const;

    // Create a lane with identity scoped to the given EdgeId
    void createLane(EdgeId scope, double width = 3.5,
                    std::optional<double> length = std::nullopt,
                    double stopLineOffset = 0.0);
    void createLanes(EdgeId scope, int count, double width = 3.5);

    double speedLimit() const { return speedLimit_; }
    void setSpeedLimit(double v) { speedLimit_ = v; }

   private:
    void validateLaneLength(std::optional<double> length) const;
    void validateStopLineOffset(double offset) const;
    static bool lengthGreater(std::optional<double> a, std::optional<double> b);

    utils::Position position_;
    double heading_;
    double speedLimit_;

    std::vector<Lane> lanes_;
    std::vector<LaneIdT> laneIds_;
    std::unordered_map<LaneIdT, size_t> laneIndex_;
    ScopedIdGenerator<LaneTagT, EdgeId> idGen_;
};

using EntryLaneGroup = LaneGroup<EntryLaneTag>;
using ExitLaneGroup  = LaneGroup<ExitLaneTag>;

}  // namespace topology
