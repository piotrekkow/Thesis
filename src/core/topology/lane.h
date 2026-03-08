#pragma once

#include <optional>

namespace topology {

class Lane {
   public:
    Lane(double width = 3.5, double stopLineOffset = 0.0,
         std::optional<double> length = std::nullopt)
        : width_(width), stopLineOffset_(stopLineOffset), length_(length) {}

    double width() const { return width_; }
    double stopLineOffset() const { return stopLineOffset_; }
    const std::optional<double>& length() const { return length_; }

   private:
    double width_;
    double stopLineOffset_;
    std::optional<double> length_;
};

}  // namespace topology
