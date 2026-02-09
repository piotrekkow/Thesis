#pragma once

class Lane {
   public:
    Lane(double width) : width_(width) {}

    double width() const { return width_; }

   private:
    double width_;
};