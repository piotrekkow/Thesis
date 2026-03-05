#include "conflict/map_factory.h"

#include <optional>
#include <stdexcept>

#include "geometry/crossing.h"
#include "geometry/movement.h"
#include "id.h"
#include "polyline_utils.h"
#include "topology/movement/movement_structure.h"

namespace conflict {
namespace {
// compute movement - movement collisions
void conflictsMovMov(Map& cm, const geometry::Movement& cGeo,
                     const geometry::Movement& eGeo, MovementId cMId,
                     MovementId eMId) {
    for (const auto& cPath : cGeo.paths()) {
        for (const auto& ePath : eGeo.paths()) {
            auto polylineIntersection = utils::firstIntersection(cPath, ePath);
            if (!polylineIntersection.has_value()) continue;
            auto pi = polylineIntersection.value();

            cm[cMId][eMId].push_back(
                {pi.distanceA, pi.distanceB, pi.point, std::nullopt});
        }
    }
}

// compute clearing movement - entering crossing collisions
void conflictsMovCross(Map& cm, const geometry::Movement& cGeo,
                       const geometry::Crossing& eGeo, MovementId cMId,
                       CrossingId eXId) {
    for (const auto& cPath : cGeo.paths()) {
        auto intersectionWithRim1 =
            utils::lastIntersection(cPath, eGeo.rims().first);
        if (!intersectionWithRim1.has_value()) continue;

        auto intersectionWithRim2 =
            utils::lastIntersection(cPath, eGeo.rims().second);
        if (!intersectionWithRim2.has_value())
            throw std::logic_error(
                "Movement doesn't cross crossing an even amount of times.");

        auto& furthest =
            intersectionWithRim1->distanceA > intersectionWithRim2->distanceA
                ? intersectionWithRim1
                : intersectionWithRim2;

        cm[cMId][eXId].push_back(
            {furthest->distanceA, 0.0, furthest->point, std::nullopt});
    }
}

// compute clearing crossing - entering movement collisions
void conflictsCrossMov(Map& cm, const geometry::Crossing& cGeo,
                       const geometry::Movement& eGeo, CrossingId cXId,
                       MovementId eMId) {
    for (const auto& ePath : eGeo.paths()) {
        auto intersectionWithRim1 =
            utils::firstIntersection(ePath, cGeo.rims().first);
        if (!intersectionWithRim1.has_value()) continue;
        auto intersectionWithRim2 =
            utils::firstIntersection(ePath, cGeo.rims().second);
        if (!intersectionWithRim2.has_value())
            throw std::logic_error(
                "Movement doesn't cross crossing an even amount of times.");

        auto& closest =
            intersectionWithRim1->distanceA < intersectionWithRim2->distanceA
                ? intersectionWithRim1
                : intersectionWithRim2;

        cm[cXId][eMId].push_back({cGeo.rims().first.length(),
                                  closest->distanceB, closest->point,
                                  std::nullopt});
    }
}

}  // namespace

Map MapFactory::compute(
    const std::unordered_map<MovementId, geometry::Movement>& movementGeos,
    const std::unordered_map<CrossingId, geometry::Crossing>& crossingGeos,
    const topology::MovementStructure& mStructure) {
    const auto& movementsAllEdges = mStructure.movementsAllEdges();

    Map cm;

    for (const auto& [eId, mIds] : movementsAllEdges)
        for (auto mId : mIds) assert(movementGeos.contains(mId));

    /// MOVEMENT-MOVEMENT Collisions

    // clearing (from) edge id / clearing movement id's
    for (const auto& [cEId, cMIds] : movementsAllEdges) {
        for (const auto& cMId : cMIds) {
            const auto& cGeo = movementGeos.at(cMId);
            // entering (from) edge id / clearing movement id's
            for (const auto& [eEId, eMIds] : movementsAllEdges) {
                // movements originating at the same edge don't collide
                if (cEId == eEId) continue;
                for (const auto& eMId : eMIds) {
                    const auto& eGeo = movementGeos.at(eMId);
                    conflictsMovMov(cm, cGeo, eGeo, cMId, eMId);
                }
            }
        }
    }

    // MOVEMENT-CROSSING Collsions
    for (const auto& [cMId, cGeo] : movementGeos) {
        for (const auto& [eXId, eGeo] : crossingGeos) {
            conflictsMovCross(cm, cGeo, eGeo, cMId, eXId);
        }
    }

    // CROSSING-MOVEMENT Collisions
    for (const auto& [cXId, cGeo] : crossingGeos) {
        for (const auto& [eMId, eGeo] : movementGeos) {
            conflictsCrossMov(cm, cGeo, eGeo, cXId, eMId);
        }
    }

    return cm;
}
}  // namespace conflict