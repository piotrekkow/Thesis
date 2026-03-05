#pragma once

#include <unordered_map>

#include "conflict/point.h"
#include "stream_id.h"

namespace conflict {
using Map =
    std::unordered_map<StreamId,
                       std::unordered_map<StreamId, std::vector<Point>>>;
}