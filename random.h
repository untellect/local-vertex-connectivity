#pragma once

#include <random>

namespace util {
	static std::default_random_engine random_engine((unsigned int)time((time_t)NULL));

    size_t select_randomly(const std::vector<size_t>& vec);
    size_t random_int(size_t min, size_t max);

    template<typename T>
    void random_shuffle(std::vector<T>& vec) {
        std::shuffle(vec.begin(), vec.end(), random_engine);
    }
}
