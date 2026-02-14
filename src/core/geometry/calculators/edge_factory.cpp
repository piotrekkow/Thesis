#include "geometry/calculators/edge_factory.h"

#include <numbers>

#include "geometry/edge.h"
#include "network.h"
#include "topology/edge.h"
#include "utils/position.h"
#include "utils/vector2.h"

namespace geometry {
namespace {

std::vector<utils::Line> laneUnitBoundVectors(utils::Position handle,
                                              double heading,
                                              std::vector<double> widths) {
    std::vector<utils::Line> positions;

    if (widths.empty()) {
        return positions;
    }

    utils::Vector2 perpendicular =
        utils::Vector2::fromAngle(heading - std::numbers::pi / 2.0);
    double totalWidth = 0;
    for (size_t i = 0; i < widths.size(); i++) {
        double widthNow = totalWidth + widths[i] / 2.0;
        utils::Position p1 = handle - perpendicular * widthNow;
        utils::Position p2 = p1 + utils::Vector2::fromAngle(heading);
        positions.push_back(utils::Line(p1, p2));
        totalWidth += widths[i];
    }

    return positions;
}
}  // namespace

Edge EdgeFactory::build(const Network& network, EdgeId id) {
    const topology::Edge& topo = network.edge(id);
    const auto& x = topo.exit();
    const auto& e = topo.entry();

    auto xLines =
        laneUnitBoundVectors(x.position(), x.heading(), x.laneWidths());
    auto eLines =
        laneUnitBoundVectors(e.position(), e.heading(), e.laneWidths());

    return Edge(xLines, eLines);
}

}  // namespace geometry