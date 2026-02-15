#include "crossing_collision_map.h"

#include "collision_point.h"
#include "geometry/crossing.h"
#include "geometry/movement_map.h"
#include "topology/movement/movement_structure.h"
#include "utils/polyline_utils.h"

namespace {
std::optional<std::pair<CollisionPoint, CollisionPoint>>
findCrossingCollisionPair(
    const utils::Polyline& movementPoly,
    const std::pair<utils::Line, utils::Line>& crossingRimPair) {
    auto isWRim1 = utils::firstAndLastIntersection(
        movementPoly, crossingRimPair.first);  // intersections with rim1
    // if no intersection with first rim, then no intersection with crossing
    if (!isWRim1.has_value()) return std::nullopt;
    auto isWRim2 = utils::firstAndLastIntersection(
        movementPoly, crossingRimPair.second);  // intersections with rim2

    // if first and last intersection is the same - that means that only one
    // collision with crossing
    if (isWRim1->first.point == isWRim1->second.point) {
        double movEntDistance =
            isWRim1->first.distanceA < isWRim2->first.distanceA
                ? isWRim1->first.distanceA
                : isWRim2->first.distanceA;
        double movExitDistance =
            isWRim1->second.distanceA > isWRim2->second.distanceA
                ? isWRim1->second.distanceA
                : isWRim2->second.distanceA;
        utils::Position movEntPos =
            isWRim1->first.distanceA < isWRim2->first.distanceA
                ? isWRim1->first.point
                : isWRim2->first.point;
        utils::Position movExitPos =
            isWRim1->second.distanceA > isWRim2->second.distanceA
                ? isWRim1->second.point
                : isWRim2->second.point;
        // assume clearing distance for crossing - crossing length,
        // entering distance for crossing - 0.0
        return std::make_pair(
            CollisionPoint(movEntDistance, crossingRimPair.first.length(),
                           movEntPos),
            CollisionPoint(movExitDistance, 0.0, movExitPos));
    } else {
        // u-turn - are both collisions on the rim 1
        if (isWRim1->first.distanceA < isWRim2->first.distanceA) {
            return std::make_pair(CollisionPoint(isWRim1->first.distanceA,
                                                 crossingRimPair.first.length(),
                                                 isWRim1->first.point),
                                  CollisionPoint(isWRim1->second.distanceA, 0.0,
                                                 isWRim1->second.point));
        } else {
            return std::make_pair(CollisionPoint(isWRim2->first.distanceA,
                                                 crossingRimPair.first.length(),
                                                 isWRim2->first.point),
                                  CollisionPoint(isWRim2->second.distanceA, 0.0,
                                                 isWRim2->second.point));
        }
    }
    return std::nullopt;
}
}  // namespace

CrossingCollisionMap CrossingCollisionMap::build(
    const geometry::MovementMap& geometryMap,
    const std::unordered_map<CrossingId, geometry::Crossing>& geoCrossingMap) {
    CrossingCollisionMap ccmap;

    const auto& movementGeometries = geometryMap.movements();
    const auto& crossingGeometries = geoCrossingMap;

    for (const auto& [cId, cGeo] : crossingGeometries) {
        const auto& rimPair = cGeo.rims();
        for (const auto& [mId, mGeo] : movementGeometries) {
            for (const auto& poly : mGeo.paths()) {
                auto optCollisionPair =
                    findCrossingCollisionPair(poly, rimPair);
                if (optCollisionPair.has_value()) {
                    ccmap.crossingCollisions_[cId].addCollisionPointPair(
                        optCollisionPair.value());
                }
            }
        }
    }

    return ccmap;
}