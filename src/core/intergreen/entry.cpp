#include "intergreen/entry.h"

#include <cmath>
#include <limits>

#include "intergreen/conflict_data.h"

namespace intergreen {
namespace {
int ceilToInt(double value) {
    double c = std::ceil(value);

    if (c > std::numeric_limits<int>::max())
        return std::numeric_limits<int>::max();

    if (c < std::numeric_limits<int>::min())
        return std::numeric_limits<int>::min();

    return static_cast<int>(c);
}
}  // namespace

Entry::Entry(ConflictData rollingData, ConflictData restData,
             GroupData groupData)
    : rollingData_(std::move(rollingData)),
      restData_(std::move(restData)),
      groupData_(std::move(groupData)) {}

bool Entry::applyAdjustment(const Adjustment& adj) {
    Adjustment merged = adjustment_;
    if (adj.v_c && adj.v_c <= 0.0) return false;
    if (adj.s_c && adj.s_c < 0) return false;
    if (adj.l_v && adj.l_v < 0.0) return false;
    if (adj.s_e && adj.s_e < 0.0) return false;
    if (adj.v_e && adj.v_e <= 0.0) return false;
    if (adj.a_e && adj.a_e <= 0.0) return false;

    auto apply = [](auto& field, const auto& opt) {
        if (opt) field = *opt;
    };

    apply(merged.s_c, adj.s_c);
    apply(merged.l_v, adj.l_v);
    apply(merged.v_c, adj.v_c);
    apply(merged.s_e, adj.s_e);
    apply(merged.v_e, adj.v_e);
    apply(merged.a_e, adj.a_e);
    apply(merged.t_y, adj.t_y);
    apply(merged.timeOverride, adj.timeOverride);
    apply(merged.simultaneousPerm, adj.simultaneousPerm);

    adjustment_ = merged;
    return true;
}

double Entry::raw() const { return data().raw(isEntryFromRest_); }

double Entry::Data::raw(bool isEntryFromRest) const {
    double t_c = (s_c + l_v) / v_c;
    double t_e = 0;
    if (v_e && a_e)
        t_e = isEntryFromRest ? (s_e / *v_e) + deltaRollingEntryTime
                              : std::sqrt((2.0 * s_e + 1.5) / *a_e);
    return t_y + t_c - t_e;
}

int Entry::time() const {
    if (adjustment_.timeOverride) return *adjustment_.timeOverride;
    int ceiled = ceilToInt(raw());
    return std::max(ceiled, groupData_.regulatroyMin);
}

const Entry::Data Entry::data() const {
    auto collisionData = isEntryFromRest_ ? restData_ : rollingData_;

    Data result;

    result.s_c = adjustment_.s_c.value_or(collisionData.s_c);
    result.v_c = adjustment_.v_c.value_or(collisionData.v_c);
    result.s_e = adjustment_.s_e.value_or(collisionData.s_e);
    if (collisionData.v_e)
        result.v_e = adjustment_.v_e.value_or(*collisionData.v_e);
    if (groupData_.a_e) result.a_e = adjustment_.a_e.value_or(*groupData_.a_e);
    result.l_v = adjustment_.l_v.value_or(groupData_.l_v);
    result.t_y = adjustment_.t_y.value_or(groupData_.t_y);
    result.deltaRollingEntryTime = adjustment_.deltaRollingEntryTime.value_or(
        groupData_.deltaRollingEntryTime);

    return result;
}
}  // namespace intergreen