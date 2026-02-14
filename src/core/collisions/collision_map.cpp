#include "collision_map.h"

#include "geometry/movement_map.h"
#include "movement_collisions.h"
#include "topology/movement/movement_structure.h"

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

CollisionMap CollisionMap::build(
    const geometry::MovementMap& geometryMap,
    const topology::MovementStructure& mStructure) {
    CollisionMap cmap;
    const auto& geometries = geometryMap.movements();
    const auto& topologies = mStructure.movements();
    const auto& movementsAllEdges = mStructure.movementsAllEdges();

    for (auto& [edge, ids] : movementsAllEdges)
        for (auto id : ids) assert(geometries.contains(id));

    for (const auto& [clearingFromEdgeId, clearingMovementIds] :
         movementsAllEdges) {
        // iterate all movements starting from this fromEdgeId
        for (const auto& clearingId : clearingMovementIds) {
            const auto& geoClearing = geometries.at(clearingId);

            for (const auto& [enteringFromEdgeId, enteringMovementIds] :
                 movementsAllEdges) {
                // skip if originating at the same edge
                if (clearingFromEdgeId == enteringFromEdgeId) continue;
                for (const auto& enteringId : enteringMovementIds) {
                    const auto& geoEntering = geometries.at(enteringId);

                    // use ordering to ensure each pair of from-edges is
                    // processed only once (avoid adding symmetric collision
                    // twice).
                    const bool addSymmetricForB = (clearingFromEdgeId.value() <
                                                   enteringFromEdgeId.value());

                    // check all path pairs (approach path vs departing path)
                    for (const auto& clearingPath : geoClearing.paths()) {
                        for (const auto& enteringPath : geoEntering.paths()) {
                            auto polylineIntersection =
                                utils::firstIntersection(clearingPath,
                                                         enteringPath);
                            if (!polylineIntersection.has_value()) continue;

                            auto pi = polylineIntersection.value();

                            // Add collision record for clearing movement
                            // clearing = distance along A (distanceA)
                            // entering = distance along B (distanceB)
                            cmap.movementCollisions_[clearingId]
                                .addCollisionPoint(CollisionPoint(
                                    pi.distanceA, pi.distanceB, pi.point));

                            // Add symmetric collision record for entering
                            // movement clearing = distance along B (distanceB)
                            // entering = distance along A (distanceA)
                            if (addSymmetricForB) {
                                cmap.movementCollisions_[enteringId]
                                    .addCollisionPoint(CollisionPoint(
                                        pi.distanceB, pi.distanceA, pi.point));
                            }
                        }
                    }
                }
            }
        }
    }

    // iterate all "from" edges
    // for (const auto& [fromA, movementsA] : movementMapData) {
    //     for (const auto& [toA, movementA] : movementsA) {
    //         const auto& approachPaths = movementA.paths();

    //         // compare against movements starting from other "from" edges
    //         for (const auto& [fromB, movementsB] : movementMapData) {
    //             if (fromB == fromA) continue;

    //             // Use ordering to ensure each pair of from-edges is
    //             processed
    //             // only once (avoid adding symmetric collision twice).
    //             const bool addSymmetricForB = (fromA.value() <
    //             fromB.value());

    //             for (const auto& [toB, movementB] : movementsB) {
    //                 const auto& departingPaths = movementB.paths();

    //                 // check all path pairs (approach path vs departing path)
    //                 for (const auto& aPath : approachPaths) {
    //                     for (const auto& bPath : departingPaths) {
    //                         auto polylineIntersection =
    //                             utils::firstIntersection(aPath, bPath);
    //                         if (!polylineIntersection.has_value()) continue;

    //                         auto pi = polylineIntersection.value();

    //                         // Add collision record *for movement A*:
    //                         // clearing = distance along A (distanceA)
    //                         // entering = distance along B (distanceB)
    //                         cmap.collisionMap_[fromA][toA].addCollisionPoint(
    //                             CollisionPoint(pi.distanceA, pi.distanceB,
    //                                            pi.point));

    //                         // Also add the symmetric record *for movement B*
    //                         // once: entering = distance along B (distanceB)
    //                         // clearing  = distance along A (distanceA)
    //                         if (addSymmetricForB) {
    //                             cmap.collisionMap_[fromB][toB]
    //                                 .addCollisionPoint(CollisionPoint(
    //                                     pi.distanceB, pi.distanceA,
    //                                     pi.point));
    //                         }
    //                     }
    //                 }
    //             }
    //         }
    //     }
    // }

    return cmap;
}

const MovementCollisions* CollisionMap::tryFind(MovementId movementId) const {
    auto it = movementCollisions_.find(movementId);
    if (it == movementCollisions_.end()) {
        return nullptr;
    }
    return &it->second;
}
