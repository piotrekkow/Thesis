#pragma once

#include <vector>

#include "utils/line.h"

namespace geometry {

class Edge {
   public:
    Edge(std::vector<utils::Line> exits, std::vector<utils::Line> entries);

    const std::vector<utils::Line>& exits() const { return exits_; }
    const std::vector<utils::Line>& entries() const { return entries_; }

   private:
    std::vector<utils::Line> exits_;
    std::vector<utils::Line> entries_;
};

}  // namespace geometry