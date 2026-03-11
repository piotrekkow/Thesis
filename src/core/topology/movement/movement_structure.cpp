// movement_structure.cpp
#include "movement_structure.h"

#include <qlogging.h>

#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <unordered_set>

#include "network.h"
#include "network_utils.h"
#include "topology/edge.h"
#include "topology/lane_group.h"
#include "topology/node.h"

namespace topology {

MovementStructure::Builder::Builder(Network& network, NodeId nodeId)
    : network_(network), nodeId_(nodeId) {}

MovementStructure::Builder& MovementStructure::Builder::addMovement(
    EdgeId from, EdgeId to, std::vector<EntryLaneId> lanes,
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
    if (lanes.size() > exitLaneCount) {
        std::ostringstream msg;
        msg << "Proposed movement at " << nodeId_ << " from " << from << " to "
            << to << " allocates " << lanes.size()
            << " lanes but there are only " << exitLaneCount
            << " lanes at the target exit.";
        throw std::invalid_argument(msg.str());
    }

    auto& orderedIds = movementsByEdge_[from];

    if (!orderedIds.empty()) {
        if (!validLaneSharing(orderedIds, lanes)) {
            std::ostringstream msg;
            msg << "Proposed movements at " << nodeId_ << " from " << from
                << " share more than 1 lane with each other.";
            throw std::logic_error(msg.str());
        }
    }

    double newAngle = odAngle(network_, from, to);

    auto pos = std::lower_bound(orderedIds.begin(), orderedIds.end(), newAngle,
                                [this](MovementId id, double angle) {
                                    const Movement& m = movements_.at(id);
                                    return odAngle(network_, m.fromEdge(),
                                                   m.toEdge()) > angle;
                                });

    MovementId newId = movementIdGen_.next(nodeId_);
    const auto& exitGroup = network_.edge(to).exit();
    std::vector<ExitLaneId> exitLanes(exitGroup.laneIds().begin(),
                                      exitGroup.laneIds().end());
    movements_.emplace(newId, Movement(from, std::move(lanes), to,
                                       std::move(exitLanes), geometrySpec));
    orderedIds.emplace(pos, newId);

    return *this;
}

MovementStructure MovementStructure::Builder::build() {
    qDebug().noquote().nospace()
        << "###\nBuilding movements at " << nodeId_ << ":\n";
    for (const auto& [edgeId, movements] : movementsByEdge_) {
        std::ostringstream msg;
        msg << "layout " << edgeId << ":\n";
        for (const auto& m : movements) {
            msg << "to:" << movements_.at(m).toEdge() << ", ";
            auto a = odAngle(network_, movements_.at(m).fromEdge(),
                             movements_.at(m).toEdge());

            auto dir = odDirection(a);
            if (dir == Direction::UTURN) msg << "uturn";
            if (dir == Direction::LEFT) msg << "left";
            if (dir == Direction::THROUGH) msg << "through";
            if (dir == Direction::RIGHT) msg << "right";
            msg << ", @" << a << ", lanes: ";
            for (const auto lId : movements_.at(m).entryLanes()) {
                msg << lId << '/';
            }
            msg << '\n';
        }
        qDebug().noquote() << QString::fromStdString(msg.str());
        if (!validLaneUtilization(movements, edgeId)) {
            throw std::logic_error(
                "Not all, too many or wrong order of lane usage in this "
                "layout");
        }
    }
    return MovementStructure(std::move(movements_),
                             std::move(movementsByEdge_));
}

MovementStructure::MovementStructure(
    std::unordered_map<MovementId, Movement> movements,
    std::unordered_map<EdgeId, std::vector<MovementId>> movementsByEdge)
    : movements_(std::move(movements)),
      movementsByEdge_(std::move(movementsByEdge)) {}

const std::vector<EdgeId>& MovementStructure::Builder::incomingEdges() const {
    return network_.node(nodeId_).incomingEdges();
}

const std::vector<EdgeId>& MovementStructure::Builder::outgoingEdges() const {
    return network_.node(nodeId_).outgoingEdges();
}

bool MovementStructure::Builder::validLaneSharing(
    const std::vector<MovementId>& ids, const std::vector<EntryLaneId>& lanes) {
    std::unordered_set<EntryLaneId> laneSet(lanes.begin(), lanes.end());
    for (const auto& id : ids) {
        const Movement& movement = movements_.at(id);
        size_t shared = 0;
        for (const auto& laneId : movement.entryLanes()) {
            if (laneSet.count(laneId)) shared++;
        }
        if (shared > 1) return false;
    }
    return true;
}

bool MovementStructure::Builder::validLaneUtilization(
    const std::vector<MovementId>& movementIds, EdgeId edgeId) {
    if (movementIds.empty()) return false;

    const EntryLaneGroup& entryGroup = network_.edge(edgeId).entry();
    const std::vector<EntryLaneId>& allLaneIds = entryGroup.laneIds();
    if (allLaneIds.empty()) return false;

    // Replicate the index-based contiguity check using indexOf lookups.
    // Movements are ordered left-to-right; each movement's lane set must be
    // contiguous and adjacent (allowing 1 shared lane) to the previous one.
    size_t currentMaxIdx = 0;
    bool firstMovement = true;
    int i = 0;

    for (const auto& movementId : movementIds) {
        const Movement& m = movements_.at(movementId);
        const auto& mLanes = m.entryLanes();
        if (mLanes.empty()) return false;

        size_t firstIdx = entryGroup.indexOf(mLanes.front());

        if (firstMovement) {
            if (firstIdx != 0) {
                qWarning() << "First movement does not start at lane 0.";
                return false;
            }
            firstMovement = false;
        } else if (firstIdx != currentMaxIdx && firstIdx != currentMaxIdx + 1) {
            qWarning()
                << "WARNING" << movementId
                << ": Movements are not contiguous.Invalid at movement index"
                << i << "from the left, where lane range starts from"
                << firstIdx << "but lane" << currentMaxIdx
                << "was already reached during checks of previous "
                   "movements.";
            return false;
        }
        currentMaxIdx = entryGroup.indexOf(mLanes.back());
        i++;
    }

    return currentMaxIdx == allLaneIds.size() - 1;
}

}  // namespace topology
