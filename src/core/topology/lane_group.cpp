#include "topology/lane_group.h"

#include <sstream>
#include <stdexcept>

namespace topology {

template <typename LaneTagT>
std::vector<double> LaneGroup<LaneTagT>::laneWidths() const {
    std::vector<double> widths;
    widths.reserve(lanes_.size());
    for (const auto& lane : lanes_) {
        widths.push_back(lane.width());
    }
    return widths;
}

// Returns true if a > b, treating nullopt as infinity.
template <typename LaneTagT>
bool LaneGroup<LaneTagT>::lengthGreater(std::optional<double> a,
                                        std::optional<double> b) {
    if (!a && !b) return false;
    if (!a) return true;
    if (!b) return false;
    return *a > *b;
}

template <typename LaneTagT>
void LaneGroup<LaneTagT>::validateLaneLength(std::optional<double> length) const {
    const size_t n = lanes_.size();

    if (n == 0) return;

    if (n == 1) {
        if (!lanes_[0].length()) return;
        if (length && *length <= *lanes_[0].length()) {
            std::ostringstream msg;
            msg << "Lane length " << *length
                << " must be greater than left neighbor length "
                << *lanes_[0].length() << " (or nullopt).";
            throw std::invalid_argument(msg.str());
        }
        return;
    }

    // n >= 2
    if (lengthGreater(lanes_[n - 2].length(), lanes_[n - 1].length())) {
        if (!length || !lanes_[n - 1].length() ||
            *length >= *lanes_[n - 1].length()) {
            std::ostringstream msg;
            msg << "Lane length must be less than left neighbor length "
                << *lanes_[n - 1].length()
                << " (nullopt not allowed in decreasing sequence).";
            throw std::invalid_argument(msg.str());
        }
    }
}

template <typename LaneTagT>
void LaneGroup<LaneTagT>::validateStopLineOffset(double offset) const {
    const size_t n = lanes_.size();
    if (n <= 1) return;

    const bool nonDecreasing =
        lanes_[1].stopLineOffset() >= lanes_[0].stopLineOffset();
    const double last = lanes_[n - 1].stopLineOffset();

    if (nonDecreasing && offset < last) {
        std::ostringstream msg;
        msg << "Stop line offset " << offset << " must be >= " << last
            << " (non-decreasing sequence).";
        throw std::invalid_argument(msg.str());
    }
    if (!nonDecreasing && offset > last) {
        std::ostringstream msg;
        msg << "Stop line offset " << offset << " must be <= " << last
            << " (non-increasing sequence).";
        throw std::invalid_argument(msg.str());
    }
}

template <typename LaneTagT>
void LaneGroup<LaneTagT>::createLane(EdgeId scope, double width,
                                     std::optional<double> length,
                                     double stopLineOffset) {
    validateLaneLength(length);
    validateStopLineOffset(stopLineOffset);
    LaneIdT id = idGen_.next(scope);
    laneIndex_.emplace(id, lanes_.size());
    laneIds_.push_back(id);
    lanes_.emplace_back(width, stopLineOffset, length);
}

template <typename LaneTagT>
void LaneGroup<LaneTagT>::createLanes(EdgeId scope, int count, double width) {
    for (int i = 0; i < count; i++) {
        createLane(scope, width);
    }
}

// Explicit instantiations
template class LaneGroup<EntryLaneTag>;
template class LaneGroup<ExitLaneTag>;

}  // namespace topology
