#pragma once

#include <unordered_map>

#include "id.h"
#include "intergreen/entry.h"

class Controller;
class IntergreenConstraints;
class SignalConstraints;
class Network;

namespace intergreen {
class Matrix {
   public:
    friend class MatrixFactory;

    // @return nullptr if no conflict (therefore entry) between signal groups
    const Entry* entry(SignalGroupId clearingId,
                       SignalGroupId enteringId) const;

    const std::unordered_map<SignalGroupId, Entry>* entriesFor(
        SignalGroupId clearingId) const;

   private:
    std::unordered_map<SignalGroupId, std::unordered_map<SignalGroupId, Entry>>
        matrix_;
};
}  // namespace intergreen