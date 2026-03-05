#pragma once

#include <variant>

#include "id.h"

using StreamId = std::variant<MovementId, CrossingId>;

namespace std {
template <>
struct hash<StreamId> {
    size_t operator()(const StreamId& stream) const {
        size_t h = std::visit(
            [](const auto& id) {
                return std::hash<std::decay_t<decltype(id)>>{}(id);
            },
            stream);
        return h ^ (std::hash<size_t>{}(stream.index()) << 32);
    }
};
}  // namespace std