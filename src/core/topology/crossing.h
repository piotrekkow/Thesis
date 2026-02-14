#pragma once

#include "position.h"

namespace topology {

class Crossing {
   public:
    Crossing(utils::Position p1, utils::Position p2, double width)
        : p1_(p1), p2_(p2), width_(width) {}
    ~Crossing() = default;

    Crossing(const Crossing&) = delete;
    Crossing& operator=(const Crossing&) = delete;

    Crossing(Crossing&&) = default;
    Crossing& operator=(Crossing&&) = default;

    utils::Position p1() const { return p1_; }
    utils::Position p2() const { return p2_; }
    double width() const { return width_; }

   private:
    utils::Position p1_;
    utils::Position p2_;
    double width_;
};

}  // namespace topology