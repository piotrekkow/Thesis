#pragma once

#include <array>
#include <unordered_map>

#include "signal_group.h"

enum class SignalAspect {
    RED,
    REDYELLOW,
    YELLOW,
    GREEN,
    GREEN_FLASHING,
    YELLOW_FLASHING,
    REDYELLOW_FLASHING,
    DARK,
    INVALID
};

enum AspectCharacter {
    STOP,
    STOP_TO_PROCEED,
    PROCEED,
    PROCEED_TO_STOP,
};

class AspectOrderDefinition {
   public:
    AspectOrderDefinition(std::array<SignalAspect, 4> order) {
        aspectOrder_ = order;
    }
    SignalAspect aspect(AspectCharacter aspectCharacter) const {
        return aspectOrder_[aspectCharacter];
    }
    void setAspect(AspectCharacter aspectCharacter, SignalAspect aspect) {
        aspectOrder_[aspectCharacter] = aspect;
    }
    void setOrder(std::array<SignalAspect, 4> order) { aspectOrder_ = order; }

   private:
    // zero index - stop (eg red),
    // first index - stop -> proceed (eg redyellow),
    // second index - proceed (eg green),
    // third index - proceed -> stop (eg yellow),
    std::array<SignalAspect, 4> aspectOrder_{SignalAspect::INVALID};
};

class SignalConstraints {
   public:
    struct Duration {
        int minPretimedProceed;
        int minActuatedProceed;
        int stopToProceed;
        int proceedToStop;
    };

    const AspectOrderDefinition& aspectOrder(SignalGroup::Type t) const {
        return aspectOrders_.at(t);
    }

    const Duration& aspectDurationConstraints(SignalGroup::Type t) const {
        return aspectDurationConstraints_.at(t);
    }

   private:
    std::unordered_map<SignalGroup::Type, AspectOrderDefinition> aspectOrders_ =
        {{SignalGroup::Type::TRAM_T,
          AspectOrderDefinition{{SignalAspect::RED, SignalAspect::REDYELLOW,
                                 SignalAspect::GREEN, SignalAspect::YELLOW}}},
         {SignalGroup::Type::BUS_B,
          AspectOrderDefinition{{SignalAspect::RED, SignalAspect::REDYELLOW,
                                 SignalAspect::GREEN, SignalAspect::YELLOW}}},
         {SignalGroup::Type::GENERAL_K,
          AspectOrderDefinition{{SignalAspect::RED, SignalAspect::REDYELLOW,
                                 SignalAspect::GREEN, SignalAspect::YELLOW}}},
         {SignalGroup::Type::PROTECTED_K,
          AspectOrderDefinition{{SignalAspect::RED, SignalAspect::REDYELLOW,
                                 SignalAspect::GREEN, SignalAspect::YELLOW}}},

         {SignalGroup::Type::CONDITIONAL_ARROW_S,
          AspectOrderDefinition{{SignalAspect::DARK, SignalAspect::DARK,
                                 SignalAspect::GREEN, SignalAspect::DARK}}},
         {SignalGroup::Type::PROTECTED_T,
          AspectOrderDefinition{{SignalAspect::RED, SignalAspect::INVALID,
                                 SignalAspect::YELLOW_FLASHING,
                                 SignalAspect::YELLOW}}},
         {SignalGroup::Type::PEDESTRIAN_P,
          AspectOrderDefinition{{SignalAspect::RED, SignalAspect::INVALID,
                                 SignalAspect::GREEN,
                                 SignalAspect::GREEN_FLASHING}}},
         {SignalGroup::Type::CYCLIST_R,
          AspectOrderDefinition{{SignalAspect::RED, SignalAspect::INVALID,
                                 SignalAspect::GREEN,
                                 SignalAspect::GREEN_FLASHING}}}};

    std::unordered_map<SignalGroup::Type, Duration> aspectDurationConstraints_ =
        {{SignalGroup::Type::TRAM_T, Duration{7, 5, 1, 3}},
         {SignalGroup::Type::PROTECTED_T, Duration{7, 5, 1, 3}},
         {SignalGroup::Type::BUS_B, Duration{7, 5, 1, 3}},
         {SignalGroup::Type::GENERAL_K, Duration{8, 5, 1, 3}},
         {SignalGroup::Type::PROTECTED_K, Duration{8, 5, 1, 3}},
         {SignalGroup::Type::CONDITIONAL_ARROW_S, Duration{8, 5, 0, 0}},
         {SignalGroup::Type::PEDESTRIAN_P, Duration{4, 4, 0, 4}},
         {SignalGroup::Type::CYCLIST_R, Duration{4, 4, 0, 4}}};
};