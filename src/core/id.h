#pragma once

#include <concepts>
#include <cstdint>
#include <limits>

// Forward declaration
template <typename Tag, typename Payload = void>
class Id;

template <typename Tag>
class GlobalIdGenerator;

template <typename Tag, typename Payload>
class ScopedIdGenerator;

// Concept
template <typename T>
concept IdType = requires(const T& id) {
    { id.valid() } -> std::convertible_to<bool>;
    { id.value() } -> std::convertible_to<uint32_t>;
};

// Primary template: scoped ID
template <typename Tag, IdType Payload>
class Id<Tag, Payload> {
   public:
    using value_type = uint32_t;

    friend class ScopedIdGenerator<Tag, Payload>;

    constexpr bool valid() const {
        return value_ != invalid() && payload_.valid();
    }

    constexpr value_type value() const { return value_; }
    constexpr const Payload& payload() const { return payload_; }

    friend constexpr bool operator==(const Id&, const Id&) = default;

   private:
    constexpr Id() : value_(invalid()), payload_{} {}
    explicit constexpr Id(Payload payload, value_type value)
        : value_(value), payload_(payload) {}

    static constexpr value_type invalid() {
        return std::numeric_limits<value_type>::max();
    }

    value_type value_;
    Payload payload_;
};

// Specialization: global ID
template <typename Tag>
class Id<Tag, void> {
   public:
    using value_type = uint32_t;

    friend class GlobalIdGenerator<Tag>;

    constexpr bool valid() const { return value_ != invalid(); }

    constexpr value_type value() const { return value_; }

    friend constexpr bool operator==(const Id&, const Id&) = default;

   private:
    constexpr Id() : value_(invalid()) {}
    explicit constexpr Id(value_type value) : value_(value) {}

    static constexpr value_type invalid() {
        return std::numeric_limits<value_type>::max();
    }

    value_type value_;
};

// logging

#include <QDebug>
#include <cctype>
#include <ostream>
#include <sstream>
#include <string_view>

template <typename Tag>
char tagChar() {
    std::string_view name = typeid(Tag).name();
    if (name.empty()) return '?';

    // Handle MSVC: skip "struct " or "class "
    if (name.starts_with("struct ")) {
        name.remove_prefix(7);
    } else if (name.starts_with("class ")) {
        name.remove_prefix(6);
    }

    // Handle GCC/Clang: skip digit prefixes (mangled names like "7RoadTag")
    while (!name.empty() && std::isdigit(static_cast<unsigned char>(name[0]))) {
        name.remove_prefix(1);
    }

    return name.empty() ? '?' : std::tolower(name[0]);
}

// Base case: global ID
template <typename Tag>
void print_full_id(std::ostream& os, const Id<Tag, void>& id, char sep = '-') {
    if (!id.valid()) {
        os << "<invalid>";
        return;
    }
    os << tagChar<Tag>() << id.value();
}

// Recursive case: scoped ID
template <typename Tag, IdType Payload>
void print_full_id(std::ostream& os, const Id<Tag, Payload>& id) {
    if (!id.valid()) {
        os << "<invalid>";
        return;
    }

    // First print the parent scope
    print_full_id(os, id.payload());

    // Then this scope
    os << tagChar<Tag>() << id.value();
}

template <typename Tag, typename Payload>
std::ostream& operator<<(std::ostream& os, const Id<Tag, Payload>& id) {
    print_full_id(os, id);
    return os;
}

template <typename Tag, typename Payload>
inline QDebug operator<<(QDebug debug, const Id<Tag, Payload>& id) {
    QDebugStateSaver saver(debug);
    std::stringstream ss;
    print_full_id(ss, id);
    debug.nospace() << ss.str().c_str();
    return debug;
}

namespace std {
template <typename Tag, typename Payload>
struct hash<Id<Tag, Payload>> {
    size_t operator()(const Id<Tag, Payload>& id) const {
        if constexpr (std::is_same_v<Payload, void>) {
            return std::hash<uint32_t>{}(id.value());
        } else {
            size_t h1 = std::hash<Payload>{}(id.payload());
            size_t h2 = std::hash<uint32_t>{}(id.value());
            return h1 ^ (h2 << 1);
        }
    }
};
}  // namespace std

struct EdgeTag {};
struct NodeTag {};
struct IntersectionTag {};
struct CrossingTag {};

using IntersectionId = Id<IntersectionTag, void>;
using EdgeId = Id<EdgeTag, void>;
using NodeId = Id<NodeTag, IntersectionId>;
using CrossingId = Id<CrossingTag, NodeId>;
