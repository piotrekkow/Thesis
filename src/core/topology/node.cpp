#include "node.h"

#include "movement_structure.h"

MovementStructure::Builder Node::createMovementBuilder(Network& network) {
    return MovementStructure::Builder(network, id_);
}

void Node::setMovementStructure(MovementStructure&& structure) {
    movementStructure_ = std::move(structure);
}

const std::optional<MovementStructure>& Node::movementStructure() const {
    return movementStructure_;
}