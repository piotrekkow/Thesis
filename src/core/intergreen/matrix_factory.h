#pragma once

#include "conflict/map.h"
#include "id.h"
#include "intergreen/conflict_data.h"
#include "intergreen/group_data.h"
#include "intergreen/matrix.h"
#include "signal_group.h"

class Network;
class Controller;
class IntergreenConstraints;
class SignalConstraints;

namespace intergreen {
class MatrixFactory {
   public:
    static Matrix compute(const Network& network, const Controller& controller,
                          const conflict::Map& cm,
                          const IntergreenConstraints& ic,
                          const SignalConstraints& sc);

   private:
    struct SearchContext {
        SignalGroup cSg;
        SignalGroupId cId;
        SignalGroup eSg;
        SignalGroupId eId;
        const conflict::Map& cm;
        const Network& network;
        const IntergreenConstraints& ic;
        const SignalConstraints& sc;
    };

    struct CriticalResult {
        ConflictData rollingData;
        ConflictData restData;
        GroupData groupData;
    };

    static std::optional<CriticalResult> findBestIntergreenForSignalGroupPair(
        SearchContext ctx);

    static ConflictData findIntergreenForStreamPair(SearchContext ctx,
                                                    StreamId cSId,
                                                    StreamId eSId,
                                                    const conflict::Point& cp);

    static bool isSimultaneousProceedPermitted(SearchContext ctx, StreamId cSId,
                                               StreamId eSId);
};
}  // namespace intergreen