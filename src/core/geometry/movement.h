#pragma once

#include "polyline.h"
namespace geometry {

class Movement {
   public:
    explicit Movement(std::vector<utils::Polyline> paths)
        : paths_(std::move(paths)) {}

    const std::vector<utils::Polyline>& paths() const { return paths_; }

   private:
    std::vector<utils::Polyline> paths_;
};

}  // namespace geometry