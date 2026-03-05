#pragma once

#include <optional>

#include "position.h"

namespace intergreen {
struct ConflictData {
    utils::Position point;
    double s_c, v_c, s_e;
    std::optional<double> v_e;
};
}  // namespace intergreen