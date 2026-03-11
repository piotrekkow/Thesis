#pragma once

#include "id.h"

class Network;
namespace topology {
class Edge;
}

enum class Direction { UTURN, LEFT, THROUGH, RIGHT };

// Returns signed turn angle: positive = left, negative = right (real-world convention).
// Internally compensates for y-down screen coordinate system.
double odAngle(const Network& network, EdgeId fromId, EdgeId toId);
Direction odDirection(double odAngle);