#include "random.h"

namespace util {
    size_t random_int(size_t min, size_t max) {
        return std::uniform_int_distribution<size_t>(min, max)(random_engine);
    }
    size_t random_volume(size_t degree) {
        return degree / std::uniform_real_distribution<double>(0.0, 1.0)(random_engine);
    }
}
