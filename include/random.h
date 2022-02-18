#pragma once

#include <random>
#include <algorithm>
#include <chrono>
#include "graph_basics.h"

namespace util {
    static std::mt19937_64 random_engine(std::chrono::system_clock::now().time_since_epoch().count());
    
    size_t random_int(size_t min, size_t max);
    size_t random_volume(size_t degree);

    template<typename T>
	T select_randomly(const std::vector<T>& vec) {
        return vec[random_int(0, vec.size() - 1)];
    }

    vertex_connectivity::vertex_vec sample_independent_set(const vertex_connectivity::adjacency_list& G, size_t expected_samplesize, size_t maxdegree);

    template<typename T>
    void random_shuffle(std::vector<T>& vec) {
        std::shuffle(vec.begin(), vec.end(), random_engine);
    }

    vertex_connectivity::vertex_vec random_each_with_probability_one_in(size_t N, size_t end);
    vertex_connectivity::vertex_vec random_each_with_probability(size_t N, double p);
}
