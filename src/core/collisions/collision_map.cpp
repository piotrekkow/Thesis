#include "collision_map.h"

#include "geometry/movement_map.h"
#include "movement_collisions.h"

namespace {
MovementCollisions findCollisions(
    const std::vector<utils::Polyline>& approaching,
    const std::vector<utils::Polyline>& departing) {
    MovementCollisions collisions;
    for (const auto& approachingPath : approaching) {
        for (const auto& departingPath : departing) {
            auto polylineIntersection =
                utils::firstIntersection(approachingPath, departingPath);
            if (polylineIntersection.has_value()) {
                collisions.addCollisionPoint(
                    CollisionPoint(polylineIntersection.value()));
            }
        }
    }
    return collisions;
}
}  // namespace

CollisionMap CollisionMap::build(geometry::MovementMap& movementMap) {
    CollisionMap cmap;

    const auto& movementMapData = movementMap.movementMap();

    // iterate all "from" edges
    for (const auto& [fromA, movementsA] : movementMapData) {
        for (const auto& [toA, movementA] : movementsA) {
            const auto& approachPaths = movementA.paths();

            // compare against movements starting from other "from" edges
            for (const auto& [fromB, movementsB] : movementMapData) {
                if (fromB == fromA) continue;

                // Use ordering to ensure each pair of from-edges is processed
                // only once (avoid adding symmetric collision twice).
                const bool addSymmetricForB = (fromA.value() < fromB.value());

                for (const auto& [toB, movementB] : movementsB) {
                    const auto& departingPaths = movementB.paths();

                    // check all path pairs (approach path vs departing path)
                    for (const auto& aPath : approachPaths) {
                        for (const auto& bPath : departingPaths) {
                            auto polylineIntersection =
                                utils::firstIntersection(aPath, bPath);
                            if (!polylineIntersection.has_value()) continue;

                            auto pi = polylineIntersection.value();

                            // Add collision record *for movement A*:
                            // clearing = distance along A (distanceA)
                            // entering = distance along B (distanceB)
                            cmap.collisionMap_[fromA][toA].addCollisionPoint(
                                CollisionPoint(pi.distanceA, pi.distanceB,
                                               pi.point));

                            // Also add the symmetric record *for movement B*
                            // once: entering = distance along B (distanceB)
                            // clearing  = distance along A (distanceA)
                            if (addSymmetricForB) {
                                cmap.collisionMap_[fromB][toB]
                                    .addCollisionPoint(CollisionPoint(
                                        pi.distanceB, pi.distanceA, pi.point));
                            }
                        }
                    }
                }
            }
        }
    }

    return cmap;
}