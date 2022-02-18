#include "graph_basics.h"

namespace vertex_connectivity {
    adjacency_list random_forest_graph_with_planted_cut(size_t L, size_t S, size_t R, size_t k);
    adjacency_list directed_cycles_with_planted_cut(size_t L, size_t S_in, size_t S_out, size_t R, size_t k, size_t d_avg);
}