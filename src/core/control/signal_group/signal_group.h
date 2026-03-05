#pragma once

#include "stream_id.h"

class SignalGroup {
   public:
    enum class Type {
        TRAM_T,
        PROTECTED_T,
        BUS_B,
        GENERAL_K,
        PROTECTED_K,
        CONDITIONAL_ARROW_S,
        PEDESTRIAN_P,
        CYCLIST_R
    };

    enum class BroadType {
        PROTECTED,
        GENERAL,
        CROSSING,
        TRAM,
        SPECIAL,
        INVALID
    };

    enum class StreamType { TRAM, BUS, PEDESTRIAN, CYCLIST, VEHICLE, INVALID };

    template <typename IdT>
    SignalGroup(Type type, const std::vector<IdT>& ids) : type_(type) {
        streams_.reserve(ids.size());
        for (const auto& id : ids) streams_.emplace_back(id);
    }
    Type type() const { return type_; }
    StreamType streamType() const { return toStreamType(type_); }

    BroadType broadType() const { return toBroadType(type_); }
    const std::vector<StreamId>& streams() const { return streams_; };

    static constexpr SignalGroup::StreamType toStreamType(SignalGroup::Type t) {
        switch (t) {
            case SignalGroup::Type::TRAM_T:
            case SignalGroup::Type::PROTECTED_T:
                return SignalGroup::StreamType::TRAM;
            case SignalGroup::Type::GENERAL_K:
            case SignalGroup::Type::PROTECTED_K:
            case SignalGroup::Type::CONDITIONAL_ARROW_S:
                return SignalGroup::StreamType::VEHICLE;
            case SignalGroup::Type::PEDESTRIAN_P:
                return SignalGroup::StreamType::PEDESTRIAN;
            case SignalGroup::Type::CYCLIST_R:
                return SignalGroup::StreamType::CYCLIST;
            case SignalGroup::Type::BUS_B:
                return SignalGroup::StreamType::BUS;
        }
        return SignalGroup::StreamType::INVALID;
    }

    static constexpr SignalGroup::BroadType toBroadType(SignalGroup::Type t) {
        switch (t) {
            case SignalGroup::Type::PROTECTED_T:
            case SignalGroup::Type::PROTECTED_K:
                return SignalGroup::BroadType::PROTECTED;
            case SignalGroup::Type::BUS_B:
            case SignalGroup::Type::GENERAL_K:
                return SignalGroup::BroadType::GENERAL;
            case SignalGroup::Type::CONDITIONAL_ARROW_S:
                return SignalGroup::BroadType::SPECIAL;
            case SignalGroup::Type::TRAM_T:
                return SignalGroup::BroadType::TRAM;
            case SignalGroup::Type::PEDESTRIAN_P:
            case SignalGroup::Type::CYCLIST_R:
                return SignalGroup::BroadType::CROSSING;
        }
        return SignalGroup::BroadType::INVALID;
    }

   private:
    Type type_;
    std::vector<StreamId> streams_;
};
