#pragma once

#include <unordered_map>

#include "controller.h"
#include "id.h"
#include "id_generator.h"
#include "topology/node.h"

namespace utils {
struct Position;
}

class Intersection {
   public:
    explicit Intersection(IntersectionId id) : id_(id), controller_(id) {}
    ~Intersection() = default;

    Intersection(const Intersection&) = delete;
    Intersection& operator=(const Intersection&) = delete;

    Intersection(Intersection&&) = default;
    Intersection& operator=(Intersection&&) = default;

    NodeId createNode();

    topology::Node& node(NodeId id) { return nodes_.at(id); }
    const topology::Node& node(NodeId id) const { return nodes_.at(id); }
    const std::unordered_map<NodeId, topology::Node>& nodes() const {
        return nodes_;
    }

    Controller& controller() { return controller_; }
    const Controller& controller() const { return controller_; }

   private:
    IntersectionId id_;  // used only to create a scope for generated ids
    std::unordered_map<NodeId, topology::Node> nodes_;
    ScopedIdGenerator<NodeTag, IntersectionId> nodeIdGen_;

    Controller controller_;
};