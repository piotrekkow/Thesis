#include "geometry/movement_map.h"

namespace geometry {

// may not exist, therefore returns a pointer
const Movement* MovementMap::tryFind(MovementId movementId) const {
    auto it = movements_.find(movementId);
    if (it == movements_.end()) return nullptr;
    return &it->second;
}

}  // namespace geometry
