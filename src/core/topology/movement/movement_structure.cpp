// movement_structure.cpp
#include "movement_structure.h"

#include <algorithm>
#include <stdexcept>

#include "edge.h"
#include "lane_group.h"
#include "network.h"
#include "node.h"
#include "position.h"
#include "vector2.h"

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

bool validateLaneSharing(const std::vector<Movement>& movements,
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
    utils::Position fromPos = fromEdge.exit().position();
    utils::Position toPos = toEdge.entry().position();

    utils::Vector2 movementDirection = toPos - fromPos;
    utils::Vector2 fromDirection =
        utils::Vector2::fromAngle(fromEdge.exit().heading());

    return fromDirection.angleTo(movementDirection);
}

void sortByHeading(std::vector<Movement>& movements) {
    std::sort(movements.begin(), movements.end(),
              [](const Movement& a, const Movement& b) {
                  return a.heading() < b.heading();  // sort descending
              });
}

auto findNewMovementPosition(std::vector<Movement>& movements,
                             double newHeading) {
    auto pos = std::lower_bound(movements.begin(), movements.end(), newHeading,
                                [](const Movement& m, double heading) {
                                    return m.heading() < heading;
                                });

    return pos;
}

bool validateAllLanes(const std::vector<Movement>& movements,
                      size_t laneCount) {
    if (movements.empty() || laneCount == 0) return false;

    size_t currentMaxReached = 0;
    bool firstMovement = true;

    for (const auto& m : movements) {
        size_t first = m.laneRange().first();

        if (firstMovement) {
            if (first != 0) return false;
            firstMovement = false;
        } else if (first != currentMaxReached &&
                   first != currentMaxReached + 1) {
            return false;
        }
        currentMaxReached = m.laneRange().last();
    }

    // Final check: Did the last movement actually reach the edge of the road?
    return currentMaxReached == laneCount - 1;
}
}  // namespace

MovementStructure::Builder::Builder(Network& network, NodeId nodeId)
    : network_(network), nodeId_(nodeId) {}

MovementStructure::Builder& MovementStructure::Builder::addMovement(
    EdgeId from, EdgeId to, LaneRange laneRange,
    MovementGeometrySpec geometrySpec) {
    if (std::find(incomingEdges().begin(), incomingEdges().end(), from) ==
        incomingEdges().end()) {
        throw std::invalid_argument("Edge not in incoming edges");
    }
    if (std::find(outgoingEdges().begin(), outgoingEdges().end(), to) ==
        outgoingEdges().end()) {
        throw std::invalid_argument("Edge not in outgoing edges");
    }

    auto& movements = movementStructure_[from];

    if (!movements.empty()) {
        if (!validateLaneSharing(movements, laneRange))
            throw std::logic_error(
                "Proposed movement shares more than 1 lane with existing "
                "movement(s)");
    }

    double newHeading = edgeToEdgeHeading(network_, from, to);
    auto pos = findNewMovementPosition(movements, newHeading);
    movements.emplace(pos, from, laneRange, to, newHeading, geometrySpec);

    return *this;
}

MovementStructure MovementStructure::Builder::build() {
    for (const auto& [edgeId, movements] : movementStructure_) {
        if (!validateAllLanes(movements,
                              network_.edge(edgeId).exit().laneCount()))
            throw std::logic_error(
                "Movement structure invalid: not all lanes used");
    }
    return MovementStructure(std::move(movementStructure_));
}

MovementStructure::MovementStructure(
    std::unordered_map<EdgeId, std::vector<Movement>> movements)
    : movementStructure_(std::move(movements)) {}

const std::vector<EdgeId>& MovementStructure::Builder::incomingEdges() const {
    return network_.node(nodeId_).incomingEdges();
}

const std::vector<EdgeId>& MovementStructure::Builder::outgoingEdges() const {
    return network_.node(nodeId_).outgoingEdges();
}