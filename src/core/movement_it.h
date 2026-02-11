// movement id differs from other id implementation in that it's not
// hierarchical it's a composite of different id's which themselves provide
// identity

#pragma once

#include <QDebug>
#include <ostream>
#include <sstream>

#include "id.h"

class MovementId {
   public:
    static constexpr MovementId create(NodeId node, EdgeId from, EdgeId to) {
        return MovementId{node, from, to};
    }

    constexpr bool valid() const {
        return node_.valid() && from_.valid() && to_.valid();
    }

    constexpr NodeId node() const { return node_; }
    constexpr EdgeId from() const { return from_; }
    constexpr EdgeId to() const { return to_; }

    friend constexpr bool operator==(const MovementId& a, const MovementId& b) {
        return a.node_ == b.node_ && a.from_ == b.from_ && a.to_ == b.to_;
    }

    friend constexpr bool operator!=(const MovementId& a, const MovementId& b) {
        return !(a == b);
    }

   private:
    explicit constexpr MovementId(NodeId node, EdgeId from, EdgeId to)
        : node_{node}, from_{from}, to_{to} {}

    NodeId node_;
    EdgeId from_;
    EdgeId to_;
};

// printing

inline std::ostream& operator<<(std::ostream& os, const MovementId& id) {
    if (!id.valid()) {
        os << "<invalid>";
        return os;
    }

    // Print hierarchical context, then movement: "i0n2:e5→e7"
    print_full_id(os, id.node());
    os << ":e" << id.from().value() << "→e" << id.to().value();
    return os;
}

inline QDebug operator<<(QDebug debug, const MovementId& id) {
    QDebugStateSaver saver(debug);
    std::stringstream ss;
    ss << id;
    debug.nospace() << ss.str().c_str();
    return debug;
}

// hashing

namespace std {
template <>
struct hash<MovementId> {
    size_t operator()(const MovementId& m) const noexcept {
        // Use same combining strategy as Id<Tag, Payload>
        size_t h1 = std::hash<NodeId>{}(m.node());
        size_t h2 = std::hash<EdgeId>{}(m.from());
        size_t h3 = std::hash<EdgeId>{}(m.to());
        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
};
}  // namespace std