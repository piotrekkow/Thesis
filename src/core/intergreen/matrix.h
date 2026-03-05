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

    // const IntergreenEntry& entry(SignalGroupId clearingId,
    //                              SignalGroupId enteringId) const;

    // IntergreenEntry& entry(SignalGroupId clearingId, SignalGroupId
    // enteringId);

    // void print() const;
    const std::unordered_map<
        SignalGroupId, std::unordered_map<SignalGroupId, std::optional<Entry>>>&
    matrix() const {
        return matrix_;
    }

   private:
    std::unordered_map<SignalGroupId,
                       std::unordered_map<SignalGroupId, std::optional<Entry>>>
        matrix_;
};
}  // namespace intergreen