#pragma once

#include <optional>
namespace intergreen {
struct Adjustment {  // user-specified patches (only fields set are
                     // applied)
    std::optional<double> s_c, l_v, v_c;
    std::optional<double> s_e, v_e, a_e;
    std::optional<int> t_y;
    std::optional<bool> simultaneousPerm;  // if user changes classification
    std::optional<int> timeOverride;
    std::optional<double> deltaRollingEntryTime;
};
}  // namespace intergreen