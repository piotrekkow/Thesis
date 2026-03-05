#pragma once

#include <unordered_map>

#include "signal_group.h"

struct Threshold {
    double max;
    double value;
};

class IntergreenConstraints {
   public:
    double additionalLength(SignalGroup::Type sgType) const {
        auto sType = SignalGroup::toStreamType(sgType);
        return additionalLength_.at(sType);
    }

    // double clearingVelocity(MovementGroupType m, double curvature) const {
    //     auto it = std::upper_bound(
    //         curvatureToClearingVelocityMap_.at(m).begin(),
    //         curvatureToClearingVelocityMap_.at(m).end(), curvature,
    //         [](double curvature, const Threshold& threshold) {
    //             return curvature < threshold.max;
    //         });
    //     double max = maxClearingVelocity_.at(m);
    //     return std::min(it->value, max);
    // }

    bool proceedToStopIsIntergreen(SignalGroup::Type sgType) const {
        return proceedToStopIsIntergreen_.at(sgType);
    }

    double clearingVelocity(SignalGroup::Type sgType,
                            std::optional<double> speedLimit) const {
        auto sType = SignalGroup::toStreamType(sgType);
        if (!speedLimit.has_value()) {
            return maxClearingVelocity_.at(sType);
        }

        return std::min(maxClearingVelocity_.at(sType), *speedLimit);
    }

    std::optional<double> enteringVelocity(
        SignalGroup::Type sgType, std::optional<double> speedLimit) const {
        auto sType = SignalGroup::toStreamType(sgType);
        if (minEnteringVelocity_.at(sType).has_value()) {
            if (!speedLimit.has_value()) {
                return minEnteringVelocity_.at(sType);
            }
            return std::max(minEnteringVelocity_.at(sType).value(),
                            *speedLimit);
        }
        return std::nullopt;
    }

    std::optional<double> maxAcceleration(SignalGroup::Type sgType) const {
        auto sType = SignalGroup::toStreamType(sgType);
        return maxAcceleration_.at(sType);
    }

    int minIntergreenTime(SignalGroup::Type clearingSgType) const {
        auto sType = SignalGroup::toStreamType(clearingSgType);
        return minIntergreenTimeForClearingStream_.at(sType);
    }

    double deltaRollingEntryTime() const { return deltaRollingEntryTime_; }

    // todo: make configurable
    int minIntergreenTime(SignalGroup::Type enteringSgType) {
        auto eBType = SignalGroup::toBroadType(enteringSgType);
        return eBType == SignalGroup::BroadType::CROSSING ? 4 : 1;
    }

   private:
    std::unordered_map<SignalGroup::StreamType, double> additionalLength_ = {
        {SignalGroup::StreamType::TRAM, 27.0},
        {SignalGroup::StreamType::BUS, 14.0},
        {SignalGroup::StreamType::VEHICLE, 10.0},
        {SignalGroup::StreamType::PEDESTRIAN, 0.0},
        {SignalGroup::StreamType::CYCLIST, 0.0}};

    std::unordered_map<SignalGroup::StreamType, double> maxClearingVelocity_ = {
        {SignalGroup::StreamType::TRAM, 10.0},
        {SignalGroup::StreamType::BUS, 10.0},
        {SignalGroup::StreamType::VEHICLE, 14.0},
        {SignalGroup::StreamType::PEDESTRIAN, 1.4},
        {SignalGroup::StreamType::CYCLIST, 4.2}};

    std::unordered_map<SignalGroup::StreamType, std::optional<double>>
        minEnteringVelocity_ = {
            {SignalGroup::StreamType::TRAM, 16.7},
            {SignalGroup::StreamType::BUS, 16.7},
            {SignalGroup::StreamType::VEHICLE, 16.7},
            {SignalGroup::StreamType::PEDESTRIAN, std::nullopt},
            {SignalGroup::StreamType::CYCLIST, std::nullopt}};

    std::unordered_map<SignalGroup::StreamType, std::optional<double>>
        maxAcceleration_ = {{SignalGroup::StreamType::TRAM, 1.2},
                            {SignalGroup::StreamType::BUS, 2.0},
                            {SignalGroup::StreamType::VEHICLE, 3.5},
                            {SignalGroup::StreamType::PEDESTRIAN, std::nullopt},
                            {SignalGroup::StreamType::CYCLIST, std::nullopt}};

    // std::unordered_map<MovementGroupType, std::vector<Threshold>>
    //     curvatureToClearingVelocityMap_ = {
    //         {MovementGroupType::TRAM_T,
    //          {{0.2, 10.0},
    //           {0.5, 8.33},
    //           {std::numeric_limits<double>::max(), 5.55}}},
    //         {MovementGroupType::PROTECTED_T,
    //          {{0.2, 10.0},
    //           {0.5, 8.33},
    //           {std::numeric_limits<double>::max(), 5.55}}},
    //         {MovementGroupType::BUS_B,
    //          {{0.2, 10.0},
    //           {0.5, 8.33},
    //           {std::numeric_limits<double>::max(), 5.55}}},
    //         {MovementGroupType::GENERAL_K,
    //          {{0.2, 13.88},
    //           {0.5, 11.11},
    //           {std::numeric_limits<double>::max(), 8.33}}},
    //         {MovementGroupType::PROTECTED_K,
    //          {{0.2, 13.88},
    //           {0.5, 11.11},
    //           {std::numeric_limits<double>::max(), 8.33}}},
    //         {MovementGroupType::CONDITIONAL_ARROW_S,
    //          {{0.2, 13.88},
    //           {0.5, 11.11},
    //           {std::numeric_limits<double>::max(), 8.33}}},
    //         {MovementGroupType::TRAM_RAILROAD_CROSSING_K,
    //          {{0.2, 13.88},
    //           {0.5, 11.11},
    //           {std::numeric_limits<double>::max(), 8.33}}},
    // };

    std::unordered_map<SignalGroup::Type, bool> proceedToStopIsIntergreen_ = {
        {SignalGroup::Type::TRAM_T, true},
        {SignalGroup::Type::PROTECTED_T, true},
        {SignalGroup::Type::BUS_B, true},
        {SignalGroup::Type::GENERAL_K, true},
        {SignalGroup::Type::PROTECTED_K, true},
        {SignalGroup::Type::CONDITIONAL_ARROW_S, false},
        {SignalGroup::Type::PEDESTRIAN_P, false},
        {SignalGroup::Type::CYCLIST_R, false}};

    std::unordered_map<SignalGroup::StreamType, int>
        minIntergreenTimeForClearingStream_ = {
            {SignalGroup::StreamType::TRAM, 1},
            {SignalGroup::StreamType::BUS, 1},
            {SignalGroup::StreamType::VEHICLE, 1},
            {SignalGroup::StreamType::PEDESTRIAN, 4},
            {SignalGroup::StreamType::CYCLIST, 4}};

    // t_d = s_d/v_d
    double deltaRollingEntryTime_ = 1;
};