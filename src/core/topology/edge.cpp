#include "edge.h"

#include "position.h"
#include "vector2.h"

Edge::Edge(NodeId from, geom::Position entryPos, NodeId to,
           geom::Position exitPos)
    : from_(from),
      to_(to),
      entry_(entryPos, (exitPos - entryPos).angle()),
      exit_(exitPos, (exitPos - entryPos).angle()) {}