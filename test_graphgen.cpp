#include "pch.h"

#include "graphgen.h"

#include <algorithm> // any_of

using namespace graph;

TEST(GRAPHS, MAKE_RFG) {
    const size_t L = 3;
    const size_t S = 2;
    const size_t R = 4;
    const size_t k = 5;

    ASSERT_LE(L + S, k); // v in L has degree less than k
    ASSERT_GT(S + R, k); // v in S, R have degree at least k

    adjacency_list RFG = random_forest_graph_with_planted_cut(L, S, R, k);

    vertex_set L_set, S_set, R_set;

    for (vertex v = 0; v < RFG.size(); v++) {
        if (RFG[v].size() < k) {
            L_set.insert(v);
        }
    }
    for (vertex v = 0; v < RFG.size(); v++) {
        if (L_set.count(v)) continue;
        if (std::any_of(RFG[v].begin(), RFG[v].end(), [&L_set](vertex w) { return L_set.count(w) == 1;})) {
            S_set.insert(v);
        }
        else {
            R_set.insert(v);
        }
    }
    
    EXPECT_EQ(L_set.size(), L); // The number of sub-5 degree vertices (L+S <= k)
    EXPECT_EQ(S_set.size(), S); // Non-L vertices with edges from L
    EXPECT_EQ(R_set.size(), R); // Non-L vertices without edges from L
}