#include "collision_map.h"

#include "geometry/movement_map.h"
#include "movement_collisions.h"
#include "topology/movement/movement_structure.h"

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
                            cmap.movementCollisions_[{clearingId, enteringId}]
                                .addCollisionPoint(CollisionPoint(
                                    pi.distanceA, pi.distanceB, pi.point));

                            // Add symmetric collision record for entering
                            // movement clearing = distance along B (distanceB)
                            // entering = distance along A (distanceA)
                            if (addSymmetricForB) {
                                cmap.movementCollisions_[{enteringId,
                                                          clearingId}]
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
