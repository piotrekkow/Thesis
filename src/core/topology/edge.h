#pragma once

#include "id.h"
#include "lane_group.h"

class Edge {
   public:
    Edge(NodeId from, utils::Position entryPos, NodeId to,
         utils::Position exitPos);
    ~Edge() = default;

    Edge(const Edge&) = delete;
    Edge& operator=(const Edge&) = delete;

    Edge(Edge&&) = default;
    Edge& operator=(Edge&&) = default;

    NodeId from() const { return from_; }
    NodeId to() const { return to_; }

    LaneGroup& entry() { return entry_; }
    LaneGroup& exit() { return exit_; }

    const LaneGroup& entry() const { return entry_; }
    const LaneGroup& exit() const { return exit_; }

   private:
    NodeId from_;
    NodeId to_;

    LaneGroup entry_;
    LaneGroup exit_;
};