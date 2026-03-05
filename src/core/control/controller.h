#pragma once

#include <unordered_map>

#include "id.h"
#include "id_generator.h"
#include "intergreen/matrix.h"
#include "signal_group.h"
#include "stream_id.h"

class Network;

class Controller {
   public:
    explicit Controller(IntersectionId intersection);

    template <IdType IdT>
    SignalGroupId createSignalGroup(SignalGroup::Type type,
                                    const std::vector<IdT>& ids) {
        auto newId = signalGroupGen_.next(intersectionId_);
        signalGroups_.emplace(newId, SignalGroup(type, ids));

        for (const auto& id : ids) {
            StreamId sid = id;
            streamToGroups_[sid].push_back(newId);
        }

        return newId;
    }

    const SignalGroup& signalGroup(SignalGroupId sg) const {
        return signalGroups_.at(sg);
    }

    const std::unordered_map<StreamId, std::vector<SignalGroupId>>&
    streamToGroups() const {
        return streamToGroups_;
    }

    const std::unordered_map<SignalGroupId, SignalGroup>& signalGroups() const {
        return signalGroups_;
    }

    void computeIntergreenMatrix(const Network& network);
    const std::optional<intergreen::Matrix>& intergreenMatrix() const {
        return intergreenMatrix_;
    }

   private:
    IntersectionId intersectionId_;

    std::unordered_map<StreamId, std::vector<SignalGroupId>> streamToGroups_;

    std::unordered_map<SignalGroupId, SignalGroup> signalGroups_;
    ScopedIdGenerator<SignalGroupTag, IntersectionId> signalGroupGen_;

    std::optional<intergreen::Matrix> intergreenMatrix_;
};