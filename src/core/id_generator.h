#pragma once

#include "id.h"

template <typename Tag>
class GlobalIdGenerator {
   public:
    using IdType = Id<Tag, void>;

    GlobalIdGenerator() = default;

    GlobalIdGenerator(const GlobalIdGenerator&) = delete;
    GlobalIdGenerator& operator=(const GlobalIdGenerator&) = delete;

    GlobalIdGenerator(GlobalIdGenerator&&) = default;
    GlobalIdGenerator& operator=(GlobalIdGenerator&&) = default;

    IdType next() { return IdType{next_++}; }

   private:
    uint32_t next_ = 0;
};

template <typename Tag, typename Payload>
class ScopedIdGenerator {
   public:
    using IdType = Id<Tag, Payload>;

    ScopedIdGenerator() = default;

    ScopedIdGenerator(const ScopedIdGenerator&) = delete;
    ScopedIdGenerator& operator=(const ScopedIdGenerator&) = delete;

    ScopedIdGenerator(ScopedIdGenerator&&) = default;
    ScopedIdGenerator& operator=(ScopedIdGenerator&&) = default;

    IdType next(const Payload& payload) { return IdType{payload, next_++}; }

   private:
    uint32_t next_ = 0;
};
