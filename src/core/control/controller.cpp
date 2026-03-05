#include "controller.h"

#include <sstream>

#include "conflict/map_factory.h"
#include "geometry/calculators/crossing_factory.h"
#include "geometry/calculators/movement_factory.h"
#include "intergreen/matrix_factory.h"
#include "intergreen_constraints.h"
#include "network.h"
#include "signal_constraints.h"

Controller::Controller(IntersectionId intersectionId)
    : intersectionId_(intersectionId) {}

void Controller::computeIntergreenMatrix(const Network& network) {
    const auto& nodes = network.intersection(intersectionId_).nodes();

    conflict::Map intersectionConflictMap;

    for (const auto& [nodeId, node] : nodes) {
        if (!node.movementStructure()) {
            std::ostringstream msg;
            msg << "Node " << nodeId
                << " has no movement structure. Cannot create intergreen "
                   "matrix.";
            throw std::runtime_error(msg.str());
        }

        // build geometry
        auto movementGeos = geometry::MovementFactory::build(
            network, *node.movementStructure());

        std::unordered_map<CrossingId, geometry::Crossing> crossingGeos;
        for (const auto& [crossId, _] : node.crossings()) {
            crossingGeos.emplace(
                crossId, geometry::CrossingFactory::build(network, crossId));
        }

        intersectionConflictMap.merge(conflict::MapFactory::compute(
            movementGeos, crossingGeos, *node.movementStructure()));
    }

    intergreenMatrix_ = intergreen::MatrixFactory::compute(
        network, *this, intersectionConflictMap, IntergreenConstraints{},
        SignalConstraints{});
}