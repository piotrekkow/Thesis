#pragma once

#include <utility>

#include "utils/line.h"

namespace geometry {

class Crossing {
   public:
    explicit Crossing(std::pair<utils::Line, utils::Line> rims)
        : rims_(std::move(rims)) {}

    const std::pair<utils::Line, utils::Line>& rims() const { return rims_; }

   private:
    std::pair<utils::Line, utils::Line> rims_;
};

}  // namespace geometry