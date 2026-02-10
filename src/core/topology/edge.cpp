#include "topology/edge.h"

#include "position.h"
#include "vector2.h"

namespace topology {
Edge::Edge(NodeId from, utils::Position exitPos, NodeId to,
           utils::Position entryPos)
    : from_(from),
      to_(to),
      exit_(exitPos, (entryPos - exitPos).angle()),
      entry_(entryPos, (entryPos - exitPos).angle()) {}
}  // namespace topology