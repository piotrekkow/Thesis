#pragma once

#include <optional>

namespace intergreen {
struct GroupData {
    // yellow duration time
    int t_y;
    // vehicle length
    double l_v;
    // entering acceleration
    std::optional<double> a_e;
    bool canProceedSimultaneously;
    double deltaRollingEntryTime;
    int regulatroyMin;
};
}  // namespace intergreen