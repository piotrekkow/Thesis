#include "intergreen/matrix_factory.h"

#include <numbers>
#include <optional>

#include "controller.h"
#include "intergreen/entry.h"
#include "intergreen_constraints.h"
#include "network.h"
#include "overloaded.h"
#include "signal_constraints.h"
#include "signal_group.h"
#include "vector2.h"

namespace intergreen {
namespace {
int yellowTime(const IntergreenConstraints& ic, const SignalConstraints& sc,
               SignalGroup::Type type) {
    return ic.proceedToStopIsIntergreen(type) == true
               ? sc.aspectDurationConstraints(type).proceedToStop
               : 0;
}

const std::vector<conflict::Point>* findConflicts(const conflict::Map& cm,
                                                  const StreamId& from,
                                                  const StreamId& to) {
    auto itFrom = cm.find(from);
    if (itFrom == cm.end()) return nullptr;

    auto itTo = itFrom->second.find(to);
    if (itTo == itFrom->second.end()) return nullptr;

    return &itTo->second;
}

double speedLimit(const Network& n, MovementId mId) {
    auto fromId =
        n.node(mId.payload()).movementStructure()->movement(mId).fromEdge();
    auto toId =
        n.node(mId.payload()).movementStructure()->movement(mId).toEdge();

    const auto& fromEdge = n.edge(fromId);
    const auto& toEdge = n.edge(toId);

    return std::min(fromEdge.entry().speedLimit(), toEdge.exit().speedLimit());
}

std::optional<double> streamSpeedLimit(const Network& network,
                                       const StreamId& stream) {
    return std::visit(
        overloaded{
            [&](MovementId mId) {
                return std::optional<double>{speedLimit(network, mId)};
            },
            [&](CrossingId) -> std::optional<double> { return std::nullopt; }},
        stream);
}

std::optional<MovementId> getMovementId(const StreamId& v) {
    if (auto ptr = std::get_if<MovementId>(&v)) {
        return *ptr;
    }
    return std::nullopt;
}

bool isThroughMovement(const Network& network, MovementId mId) {
    const auto& mStr = network.node(mId.payload()).movementStructure().value();
    const auto& mov = mStr.movement(mId);

    const auto& entry = network.edge(mov.fromEdge()).entry();
    const auto& exit = network.edge(mov.toEdge()).exit();

    double dir = utils::angleBetween(entry.heading(), exit.heading());
    return std::abs(dir) < std::numbers::pi / 4;
}

bool isLeftmostOrRightmostAndLaneShared(const Network& network,
                                        MovementId mId) {
    if (isThroughMovement(network, mId)) return false;

    const auto& mStr = network.node(mId.payload()).movementStructure().value();
    const auto& mov = mStr.movement(mId);
    if (mov.laneRange().count() == 1) return false;
    const auto& movementsFromEdge = mStr.movementsByEdge(mov.fromEdge());

    for (const auto& otherId : movementsFromEdge) {
        if (otherId == mId) continue;
        if (!isThroughMovement(network, otherId)) continue;

        const auto& other = mStr.movement(otherId);
        if (mov.laneRange().sharedLaneCount(other.laneRange()) > 0) return true;
    }
    return false;
}

bool areFromOppositeApproaches(const Network& network, MovementId a,
                               MovementId b) {
    if (a.payload() != b.payload()) return false;  // different intersections

    const auto& mStr = network.node(a.payload()).movementStructure().value();
    EdgeId fromA = mStr.movement(a).fromEdge();
    EdgeId fromB = mStr.movement(b).fromEdge();

    double headingA = network.edge(fromA).entry().heading();
    double headingB = network.edge(fromB).entry().heading();

    // Opposite approaches have headings ~180° apart
    double diff = utils::angleBetween(headingA, headingB);
    return std::abs(diff) > 3.0 * std::numbers::pi / 4.0;
}

bool areFromSameOrOppositeApproaches(const Network& network, MovementId a,
                                     MovementId b) {
    if (a.payload() != b.payload()) return false;

    const auto& mStr = network.node(a.payload()).movementStructure().value();
    EdgeId fromA = mStr.movement(a).fromEdge();
    EdgeId fromB = mStr.movement(b).fromEdge();

    if (fromA == fromB) return true;  // same approach
    return areFromOppositeApproaches(network, a, b);
}

void updateCandidate(const ConflictData& candidate, ConflictData& best,
                     const GroupData& groupData, bool isEntryFromRest) {
    if (Entry::Data(candidate, groupData).raw(isEntryFromRest) >
        Entry::Data(best, groupData).raw(isEntryFromRest))
        best = candidate;
}

}  // namespace

Matrix MatrixFactory::compute(const Network& network,
                              const Controller& controller,
                              const conflict::Map& cm,
                              const IntergreenConstraints& ic,
                              const SignalConstraints& sc) {
    Matrix result;

    // for all possible signal group interactions set entry to nullopt meaning
    // no collision
    for (const auto& [c, _] : controller.signalGroups()) {
        for (const auto& [e, __] : controller.signalGroups()) {
            if (c == e) continue;
            result.matrix_[c][e] = std::nullopt;
        }
    }

    for (const auto& [cId, cSg] : controller.signalGroups()) {
        for (const auto& [eId, eSg] : controller.signalGroups()) {
            if (cId == eId) continue;
            auto ig = findBestIntergreenForSignalGroupPair(
                {cSg, cId, eSg, eId, cm, network, ic, sc});

            if (ig) {
                result.matrix_[cId][eId] =
                    Entry(ig->rollingData, ig->restData, ig->groupData);
            } else {
                result.matrix_[cId][eId] = std::nullopt;
            }
        }
    }

    return result;
}

std::optional<MatrixFactory::CriticalResult>
MatrixFactory::findBestIntergreenForSignalGroupPair(SearchContext ctx) {
    GroupData groupData{
        .t_y = yellowTime(ctx.ic, ctx.sc, ctx.cSg.type()),
        .l_v = ctx.ic.additionalLength(ctx.cSg.type()),
        .a_e = ctx.ic.maxAcceleration(ctx.eSg.type()),
        .canProceedSimultaneously = true,  // temporary
        .deltaRollingEntryTime = ctx.ic.deltaRollingEntryTime(),
        .regulatroyMin = ctx.ic.minIntergreenTime(ctx.cSg.type())};

    ConflictData criticalRollingData;
    ConflictData criticalRestData;

    bool firstCheck = true;

    for (const auto& cStream : ctx.cSg.streams()) {
        for (const auto& eStream : ctx.eSg.streams()) {
            if (auto conflicts = findConflicts(ctx.cm, cStream, eStream)) {
                for (const auto& cp : *conflicts) {
                    ConflictData candidate =
                        findIntergreenForStreamPair(ctx, cStream, eStream, cp);
                    if (firstCheck) {
                        criticalRollingData = candidate;
                        criticalRestData = candidate;
                        firstCheck = false;
                    } else {
                        updateCandidate(candidate, criticalRollingData,
                                        groupData, false);
                        updateCandidate(candidate, criticalRestData, groupData,
                                        true);
                    }

                    if (!isSimultaneousProceedPermitted(ctx, cStream,
                                                        eStream)) {
                        groupData.canProceedSimultaneously = false;
                    }
                }
            }
        }
    }

    if (firstCheck) return std::nullopt;
    return CriticalResult{criticalRollingData, criticalRestData, groupData};
}

ConflictData MatrixFactory::findIntergreenForStreamPair(
    SearchContext ctx, StreamId cSId, StreamId eSId,
    const conflict::Point& cp) {
    auto clearingSpeedLimit = streamSpeedLimit(ctx.network, cSId);
    double v_c = ctx.ic.clearingVelocity(ctx.cSg.type(), clearingSpeedLimit);
    auto enteringSpeedLimit = streamSpeedLimit(ctx.network, eSId);
    auto v_e = ctx.ic.enteringVelocity(ctx.eSg.type(), enteringSpeedLimit);

    return ConflictData{cp.position(), cp.clearingDistance(), v_c,
                        cp.enteringDistance(), v_e};
}

bool MatrixFactory::isSimultaneousProceedPermitted(SearchContext ctx,
                                                   StreamId cSId,
                                                   StreamId eSId) {
    // Do grupy par strumieni kolizyjnych o niedopuszczalnym jednoczesnym
    // zezwoleniu na ruch zalicza się następujące pary:

    // a) strumień pojazdów sterowany sygnałem kierunkowym i dowolny inny
    //    strumień kolizyjny,
    if (ctx.cSg.broadType() == SignalGroup::BroadType::PROTECTED ||
        ctx.eSg.broadType() == SignalGroup::BroadType::PROTECTED)
        return false;

    // b) dowolny strumień pojazdów sterowany sygnałem ogólnym i kolidujący
    //    strumień pojazdów z wlotu innego niż przeciwległy,
    if (ctx.cSg.broadType() == SignalGroup::BroadType::GENERAL &&
        ctx.cSg.streamType() == SignalGroup::StreamType::VEHICLE) {
        auto cMId = getMovementId(cSId);
        auto eMId = getMovementId(eSId);
        if (cMId && eMId &&
            !areFromOppositeApproaches(ctx.network, *cMId, *eMId))
            return false;
    }

    // c1) dowolny strumień pieszy i dowolny strumień
    // kolizyjny z wyjątkiem strumienia pojazdów opuszczających skrzyżowanie
    // skręcających w lewo lub w prawo z pasa sterowanego sygnałem ogólnym,
    if (ctx.cSg.streamType() == SignalGroup::StreamType::PEDESTRIAN ||
        ctx.eSg.streamType() == SignalGroup::StreamType::PEDESTRIAN) {
        if (ctx.cSg.broadType() == SignalGroup::BroadType::GENERAL) {
            auto mId = std::get_if<MovementId>(&cSId);
            if (mId && isThroughMovement(ctx.network, *mId)) return false;
        }

        if (ctx.eSg.broadType() == SignalGroup::BroadType::GENERAL) {
            auto mId = std::get_if<MovementId>(&eSId);
            if (mId && isThroughMovement(ctx.network, *mId)) return false;
        }

        // c2) dowolny strumień pieszy i dowolny strumień kolizyjny z wyjątkiem
        // strumienia pojazdów sterowanych sygnałem dopuszczającym skręcanie w
        // kierunku wskazanym strzałką,
        if (ctx.cSg.type() != SignalGroup::Type::CONDITIONAL_ARROW_S ||
            ctx.eSg.type() != SignalGroup::Type::CONDITIONAL_ARROW_S)
            return false;
    }

    // d) dowolny strumień rowerowy i dowolny strumień
    // kolizyjny z wyjątkiem strumienia pojazdów opuszczających skrzyżowanie
    // skręcających w lewo lub w prawo z pasa sterowanego sygnałem ogólnym,
    if (ctx.cSg.streamType() == SignalGroup::StreamType::CYCLIST ||
        ctx.eSg.streamType() == SignalGroup::StreamType::CYCLIST) {
        if (ctx.cSg.broadType() == SignalGroup::BroadType::GENERAL) {
            auto mId = std::get_if<MovementId>(&cSId);
            if (mId && isThroughMovement(ctx.network, *mId)) return false;
        }

        if (ctx.eSg.broadType() == SignalGroup::BroadType::GENERAL) {
            auto mId = std::get_if<MovementId>(&eSId);
            if (mId && isThroughMovement(ctx.network, *mId)) return false;
        }
    }

    // e1) dowolny strumień tramwajowy i dowolny strumień kolizyjny,

    if (ctx.cSg.streamType() == SignalGroup::StreamType::TRAM) {
        auto cMId = getMovementId(cSId);
        auto eMId = getMovementId(eSId);

        // z wyjątkiem pary strumień tramwajowy na wprost
        if (ctx.eSg.streamType() == SignalGroup::StreamType::TRAM && eMId &&
            isThroughMovement(ctx.network, *eMId))
            return true;

        // z wyjątkiem pary strumień pojazdów z tego samego wlotu lub z wlotu
        // przeciwległego skręcających w lewo z pasa ruchu wspólnego dla
        // kierunku na wprost i w lewo (w prawo)

        if (eMId && cMId &&
            areFromSameOrOppositeApproaches(ctx.network, *cMId, *eMId) &&
            isLeftmostOrRightmostAndLaneShared(ctx.network, *eMId))
            return true;

        return false;
    }
    return true;
}
}  // namespace intergreen