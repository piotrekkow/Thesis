#include "geometry/edge.h"

namespace geometry {

Edge::Edge(std::vector<utils::Line> exits, std::vector<utils::Line> entries)
    : exits_(std::move(exits)), entries_(std::move(entries)) {}

}  // namespace geometry
