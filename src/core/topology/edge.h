#pragma once

#include "id.h"
#include "topology/lane_group.h"

namespace topology {
class Edge {
   public:
    Edge(NodeId from, utils::Position exitPos, NodeId to,
         utils::Position entryPos);
    ~Edge() = default;

    Edge(const Edge&) = delete;
    Edge& operator=(const Edge&) = delete;

    Edge(Edge&&) = default;
    Edge& operator=(Edge&&) = default;

    NodeId from() const { return from_; }
    NodeId to() const { return to_; }

    LaneGroup& exit() { return exit_; }
    LaneGroup& entry() { return entry_; }

    const LaneGroup& exit() const { return exit_; }
    const LaneGroup& entry() const { return entry_; }

   private:
    NodeId from_;
    NodeId to_;

    LaneGroup exit_;
    LaneGroup entry_;
};
}  // namespace topology