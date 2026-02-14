#include "topology/node.h"

#include <qlogging.h>

#include "geometry/calculators/crossing_factory.h"


namespace topology {

MovementStructure::Builder Node::createMovementBuilder(Network& network) {
    return MovementStructure::Builder(network, id_);
}

void Node::setMovementStructure(MovementStructure&& structure) {
    movementStructure_ = std::move(structure);
}

const std::optional<MovementStructure>& Node::movementStructure() const {
    return movementStructure_;
}

CrossingId Node::createCrossing(utils::Position p1, utils::Position p2,
                                double width) {
    if (p1 == p2) {
        qWarning() << "Created crossing with 0 length.";
    }
    auto newId = crossingIdGen_.next(id_);
    crossings_.emplace(newId, Crossing(p1, p2, width));
    return newId;
}

}  // namespace topology