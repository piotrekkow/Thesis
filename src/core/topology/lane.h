#pragma once

namespace topology {

class Lane {
   public:
    Lane(double width) : width_(width) {}

    double width() const { return width_; }

   private:
    double width_;
};

}  // namespace topology