#include "intergreen/matrix.h"

namespace intergreen {

const Entry* Matrix::entry(SignalGroupId c, SignalGroupId e) const {
    auto it = matrix_.find(c);
    if (it == matrix_.end()) return nullptr;
    auto it2 = it->second.find(e);
    return it2 != it->second.end() ? &it2->second : nullptr;
}

const std::unordered_map<SignalGroupId, Entry>* Matrix::entriesFor(
    SignalGroupId clearingId) const {
    auto it = matrix_.find(clearingId);
    return it != matrix_.end() ? &it->second : nullptr;
}
}  // namespace intergreen