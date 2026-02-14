#pragma once

#include <unordered_map>

#include "geometry/movement.h"
#include "id.h"

class Network;

namespace geometry {
class MovementFactory;

class MovementMap {
   public:
    const std::unordered_map<MovementId, Movement>& movements() const {
        return movements_;
    }
    const Movement* tryFind(MovementId movementId) const;

   private:
    std::unordered_map<MovementId, Movement> movements_;
    MovementMap(std::unordered_map<MovementId, Movement> movements)
        : movements_(std::move(movements)) {}
    friend class MovementFactory;
};
}  // namespace geometry