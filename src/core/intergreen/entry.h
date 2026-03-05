#pragma once

#include "intergreen/adjustment.h"
#include "intergreen/conflict_data.h"
#include "intergreen/group_data.h"

namespace intergreen {
class Entry {
   public:
    struct Data {
        double s_c, v_c, s_e;
        std::optional<double> v_e, a_e;
        double l_v;
        int t_y;
        double deltaRollingEntryTime;

        double raw(bool isEntryFromRest) const;
        Data(ConflictData icd, GroupData igd)
            : s_c(icd.s_c),
              v_c(icd.v_c),
              s_e(icd.s_e),
              v_e(icd.v_e),
              a_e(igd.a_e),
              l_v(igd.l_v),
              t_y(igd.t_y),
              deltaRollingEntryTime(igd.deltaRollingEntryTime) {}
        Data() = default;
    };

    Entry(ConflictData rollingData, ConflictData restData, GroupData groupData);

    bool applyAdjustment(const Adjustment& adj);

    double raw() const;
    int time() const;
    const Data data() const;

    const Adjustment& adjustment() const { return adjustment_; }

    bool isEntryFromRest() const { return isEntryFromRest_; }
    void setEntryFromRest(bool isEntryFromRest) {
        isEntryFromRest_ = isEntryFromRest;
    }

    bool canProceedSimultaneously() const {
        return adjustment_.simultaneousPerm.value_or(
            groupData_.canProceedSimultaneously);
    }

   private:
    ConflictData rollingData_;
    ConflictData restData_;
    GroupData groupData_;
    Adjustment adjustment_;

    bool isEntryFromRest_ = false;
};
}  // namespace intergreen