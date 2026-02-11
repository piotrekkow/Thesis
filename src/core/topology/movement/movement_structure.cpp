// movement_structure.cpp
#include "movement_structure.h"

#include <qlogging.h>

#include <algorithm>
#include <numbers>
#include <sstream>
#include <stdexcept>

#include "network.h"
#include "position.h"
#include "topology/edge.h"
#include "topology/lane_group.h"
#include "topology/node.h"
#include "vector2.h"

namespace topology {

namespace {
// movements can share at most 1 lane with neighbor
bool validateLaneSharing(const std::vector<Movement>& movements) {
    for (size_t i = 0; i < movements.size() - 1; ++i) {
        size_t shared = movements[i].laneRange().sharedLaneCount(
            movements[i + 1].laneRange());

        if (shared > 1) {
            return false;
        }
    }
    return true;
}

bool validLaneSharing(const std::vector<Movement>& movements,
                      LaneRange laneRange) {
    for (const auto& movement : movements) {
        if (movement.laneRange().sharedLaneCount(laneRange) > 1) {
            return false;
        }
    }
    return true;
}

double edgeToEdgeHeading(Network& network, EdgeId from, EdgeId to) {
    Edge& fromEdge = network.edge(from);
    Edge& toEdge = network.edge(to);
    utils::Position fromPos = fromEdge.entry().position();
    utils::Position toPos = toEdge.exit().position();

    utils::Vector2 movementDirection = toPos - fromPos;
    utils::Vector2 fromDirection =
        utils::Vector2::fromAngle(fromEdge.entry().heading());

    // fallback - try getting a direction to a lane entry offset by unit vector.
    if (movementDirection.isZero()) {
        // utils::Vector2 perpendicular = utils::Vector2::fromAngle(
        //     toEdge.exit().heading() - std::numbers::pi / 2.0);
        // movementDirection = (toPos + perpendicular) - fromPos;
        return -std::numbers::pi;
    }

    return fromDirection.angleTo(movementDirection);
}

void sortByHeading(std::vector<Movement>& movements) {
    std::sort(movements.begin(), movements.end(),
              [](const Movement& a, const Movement& b) {
                  return a.heading() < b.heading();  // descending
              });
}

auto findNewMovementPosition(std::vector<Movement>& movements,
                             double newHeading) {
    return std::lower_bound(movements.begin(), movements.end(), newHeading,
                            [](const Movement& m, double heading) {
                                return m.heading() <
                                       heading;  // must match descending order
                            });
}

bool validLaneUtilization(const std::vector<Movement>& movements,
                          size_t laneCount) {
    if (movements.empty() || laneCount == 0) return false;

    size_t currentMaxReached = 0;
    bool firstMovement = true;

    int i = 0;
    for (const auto& m : movements) {
        size_t first = m.laneRange().first();

        if (firstMovement) {
            if (first != 0) {
                qWarning() << "First movement does not start at lane 0.";
                return false;
            }
            firstMovement = false;
        } else if (first != currentMaxReached &&
                   first != currentMaxReached + 1) {
            qWarning() << "WARNING: Movements are not contiguous. Invalid at "
                          "movement index"
                       << i << "from the left, where lane range starts from"
                       << first << "but lane" << currentMaxReached
                       << "was already reached during checks of previous "
                          "movements.";
            return false;
        }
        currentMaxReached = m.laneRange().last();
        i++;
    }

    // Final check: Did the last movement actually reach the edge of the road?
    return currentMaxReached == laneCount - 1;
}

bool validLaneRange(const LaneRange& lr, size_t exitLaneCount) {
    return lr.count() <= exitLaneCount;
}
}  // namespace

MovementStructure::Builder::Builder(Network& network, NodeId nodeId)
    : network_(network), nodeId_(nodeId) {}

MovementStructure::Builder& MovementStructure::Builder::addMovement(
    EdgeId from, EdgeId to, LaneRange laneRange,
    MovementGeometrySpec geometrySpec) {
    if (std::find(incomingEdges().begin(), incomingEdges().end(), from) ==
        incomingEdges().end()) {
        std::ostringstream msg;
        msg << "Proposed movement at " << nodeId_ << " from " << from << " to "
            << to << " but " << from << " is not part of that node.";
        throw std::invalid_argument(msg.str());
    }
    if (std::find(outgoingEdges().begin(), outgoingEdges().end(), to) ==
        outgoingEdges().end()) {
        std::ostringstream msg;
        msg << "Proposed movement at " << nodeId_ << " from " << from << " to "
            << to << " but " << to << " is not part of that node.";
        throw std::invalid_argument(msg.str());
    }

    size_t exitLaneCount = network_.edge(to).exit().laneCount();
    if (!validLaneRange(laneRange, exitLaneCount)) {
        std::ostringstream msg;
        msg << "Proposed movement at " << nodeId_ << " from " << from << " to "
            << to << " allocates " << laneRange.count()
            << " lanes but there are only " << exitLaneCount
            << " lanes at the target exit.";

        throw std::invalid_argument(msg.str());
    }

    auto& movements = movements_[from];

    if (!movements.empty()) {
        if (!validLaneSharing(movements, laneRange)) {
            std::ostringstream msg;
            msg << "Proposed movements at " << nodeId_ << " from " << from
                << " share more than 1 lane with each other.";
            throw std::logic_error(msg.str());
        }
    }

    double newHeading = edgeToEdgeHeading(network_, from, to);
    auto pos = findNewMovementPosition(movements, newHeading);
    movements.emplace(pos, from, laneRange, to, newHeading, geometrySpec);

    return *this;
}

MovementStructure MovementStructure::Builder::build() {
    for (const auto& [edgeId, movements] : movements_) {
        if (!validLaneUtilization(movements,
                                  network_.edge(edgeId).entry().laneCount())) {
            qWarning() << "WARNING: Movement from edge" << edgeId
                       << "has following headings: ";
            int i = 0;
            for (const auto& m : movements) {
                qWarning() << "Movement" << i << "heading:" << m.heading();
                i++;
            }
            std::ostringstream msg;
            msg << "In movement structure at node " << nodeId_ << " from "
                << edgeId << " not all, or too many lanes are used.";
            throw std::logic_error(msg.str());
        }
    }
    return MovementStructure(std::move(movements_));
}

MovementStructure::MovementStructure(
    std::unordered_map<EdgeId, std::vector<Movement>> movements)
    : movements_(std::move(movements)) {}

const std::vector<EdgeId>& MovementStructure::Builder::incomingEdges() const {
    return network_.node(nodeId_).incomingEdges();
}

const std::vector<EdgeId>& MovementStructure::Builder::outgoingEdges() const {
    return network_.node(nodeId_).outgoingEdges();
}

}  // namespace topology