#pragma once

#include <variant>

#include "overloaded.h"

enum class CrossingGroupType { PEDESTRIAN_P, CYCLIST_R, WARNING_O };

enum class MovementGroupType {
    TRAM_T,
    PROTECTED_T,
    BUS_B,
    GENERAL_K,
    PROTECTED_K,
    CONDITIONAL_ARROW_S,
    TRAM_RAILROAD_CROSSING_K,
};

static constexpr bool isProtected(MovementGroupType m) {
    return m == MovementGroupType::PROTECTED_K ||
           m == MovementGroupType::PROTECTED_T;
}

static constexpr bool isGeneral(MovementGroupType m) {
    return m == MovementGroupType::GENERAL_K || m == MovementGroupType::BUS_B;
}

using SignalGroupType = std::variant<MovementGroupType, CrossingGroupType>;

static char tag(SignalGroupType t) {
    std::visit(
        overloaded{[](MovementGroupType m) {
                       switch (m) {
                           case MovementGroupType::TRAM_T:
                               return 'T';
                           case MovementGroupType::BUS_B:
                               return 'S';
                           case MovementGroupType::GENERAL_K:
                               return 'K';
                           case MovementGroupType::PROTECTED_K:
                               return 'K';
                           case MovementGroupType::CONDITIONAL_ARROW_S:
                               return 'S';
                           case MovementGroupType::TRAM_RAILROAD_CROSSING_K:
                               return 'K';
                           default:
                               return '$';
                       }
                   },
                   [](CrossingGroupType c) {
                       switch (c) {
                           case CrossingGroupType::PEDESTRIAN_P:
                               return 'P';
                           case CrossingGroupType::CYCLIST_R:
                               return 'R';
                           case CrossingGroupType::WARNING_O:
                               return 'O';
                           default:
                               return '#';
                       }
                   }},
        t);

    return '#';
}