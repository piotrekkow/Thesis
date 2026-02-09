#pragma once

#include "position.h"

class Crossing {
   public:
    Crossing();
    ~Crossing() = default;

    Crossing(const Crossing&) = delete;
    Crossing& operator=(const Crossing&) = delete;

    Crossing(Crossing&&) = default;
    Crossing& operator=(Crossing&&) = default;

   private:
    geom::Position p1_;
    geom::Position p2_;
    double width_;
};