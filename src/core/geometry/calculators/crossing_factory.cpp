#include "geometry/calculators/crossing_factory.h"

#include "line.h"
#include "network.h"
#include "position.h"
#include "topology/crossing.h"
#include "topology/node.h"
#include "vector2.h"

namespace geometry {

Crossing CrossingFactory::build(const Network& network, CrossingId id) {
    const topology::Node& node = network.node(id.payload());
    const topology::Crossing& topo = node.crossing(id);

    utils::Line rim1 = utils::Line(topo.p1(), topo.p2());
    utils::Vector2 dir = (topo.p2() - topo.p1()).normalized();

    utils::Vector2 perp = utils::Vector2(-dir.dy, dir.dx);

    utils::Line rim2 = utils::Line(topo.p1() + perp * topo.width(),
                                   topo.p2() + perp * topo.width());

    return Crossing({rim1, rim2});
}

}  // namespace geometry