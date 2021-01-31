#include "pch.h"

#include "random.h"

namespace util {
    size_t select_randomly(const std::vector<size_t>& vec) {
        return vec[random_int(0, vec.size() - 1)];
    }
    size_t random_int(size_t min, size_t max) {
        return std::uniform_int_distribution<size_t>(min, max)(random_engine);
    }
}
