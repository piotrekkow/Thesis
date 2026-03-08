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

    ExitLaneGroup& exit() { return exit_; }
    EntryLaneGroup& entry() { return entry_; }

    const ExitLaneGroup& exit() const { return exit_; }
    const EntryLaneGroup& entry() const { return entry_; }

   private:
    NodeId from_;
    NodeId to_;

    ExitLaneGroup exit_;
    EntryLaneGroup entry_;
};
}  // namespace topology