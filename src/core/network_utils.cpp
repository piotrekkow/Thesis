#include "network_utils.h"

#include <cmath>
#include <numbers>

#include "network.h"
#include "od_pair.h"
#include "vector2.h"

namespace {
double odAngle(Network& network, ODPair od) {
    return odAngle(network, od.origin, od.destination);
}
}  // namespace

double odAngle(const Network& network, EdgeId fromId, EdgeId toId) {
    const auto& from = network.edge(fromId).entry();
    const auto& to = network.edge(toId).exit();

    utils::Vector2 movementDir = to.position() - from.position();
    if (movementDir.isZero()) return std::numbers::pi;
    utils::Vector2 entryDir = utils::Vector2::fromAngle(from.heading());
    utils::Vector2 exitDir = utils::Vector2::fromAngle(to.heading());

    // Negate to convert from screen coordinates (y-down, where right turns
    // produce positive cross products) to math convention (y-up), which
    // odDirection expects: positive = left, negative = right.
    return -(entryDir.angleTo(movementDir) + movementDir.angleTo(exitDir));
}

// angle ∈ [-π/4,  π/4] → THROUGH
// angle ∈ ( π/4, 3π/4] → LEFT
// angle ∈ (3π/4,   +∞) → UTURN
// angle ∈ (  -∞, -π/4) → RIGHT
Direction odDirection(double odAngle) {
    constexpr double a = std::numbers::pi / 4;
    constexpr double b = 3 * std::numbers::pi / 4;

    if (std::abs(odAngle) <= a) return Direction::THROUGH;
    if (odAngle < -a) return Direction::RIGHT;
    if (odAngle <= b) return Direction::LEFT;
    return Direction::UTURN;
}

double saturationFlowCollisionFree(Network& network, ODPair od) {
    Direction dir = odDirection(odAngle(network, od));
    if (dir == Direction::THROUGH) return 1900;
    if (dir == Direction::RIGHT) return 1600;
    if (dir == Direction::LEFT) return 1750;
    // Direction::UTURN
    return 1470;
}