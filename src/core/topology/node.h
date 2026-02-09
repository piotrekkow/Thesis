#pragma once

#include <optional>
#include <vector>

#include "crossing.h"
#include "id.h"
#include "id_generator.h"
#include "movement_structure.h"


class Network;

class Node {
   public:
    Node(NodeId id) : id_(id) {}
    ~Node() = default;

    Node(const Node&) = delete;
    Node& operator=(const Node&) = delete;

    Node(Node&&) = default;
    Node& operator=(Node&&) = default;

    void addIncomingEdge(EdgeId id) { incomingEdges_.push_back(id); }
    void addOutgoingEdge(EdgeId id) { outgoingEdges_.push_back(id); }

    const std::vector<EdgeId>& incomingEdges() const { return incomingEdges_; }
    const std::vector<EdgeId>& outgoingEdges() const { return outgoingEdges_; }

    MovementStructure::Builder createMovementBuilder(Network& network);
    // setMovementStructure only via builder
    void setMovementStructure(MovementStructure&& structure);
    const std::optional<MovementStructure>& movementStructure() const;

   private:
    NodeId id_;

    std::vector<EdgeId> incomingEdges_;
    std::vector<EdgeId> outgoingEdges_;

    std::optional<MovementStructure> movementStructure_;
    std::unordered_map<CrossingId, Crossing> crossings_;
    ScopedIdGenerator<CrossingTag, NodeId> crossingIdGen_;
};