#include "vertex_connectivity.h"

#include "random.h"

namespace vertex_connectivity {
    void localec(resettable_graph& SG, vertex x, size_t nu, size_t k, bool& found_cut) {
        nu *= 2; // Volume multiplier.

        size_t source_degree = SG[x].size();
        if (nu <= source_degree) {
            return;
        }
        nu -= source_degree; // Subtract outgoing edges of x from the volume parameter because x is *guaranteed* in L
        size_t max_edges = nu * k;

        for (size_t iter = 0; iter < k; ++iter) {
            size_t edges_left = util::random_int(1, max_edges);
            if (iter == k - 1) edges_left = max_edges;

            SG.new_search();
            SG.visit(x, { x, 0 });
            resettable_graph::internal_location dfs = { x, 0 }; // dfs "current" pointer
            while (true) {
                if (dfs.i >= SG[dfs.v].size()) {
                    if (dfs.v == x) {
                        found_cut = true;
                        return;
                    }

                    SG.backtrack_inplace(dfs);
                    dfs.i++;
                    continue;
                }

                if (dfs.v != x) {
                    if (--edges_left == 0) {
                        break;
                    }
                }

                vertex w = SG[dfs.v][dfs.i];
                if (SG.visited(w)) {
                    dfs.i++;
                }
                else {
                    SG.visit(w, dfs);
                    dfs = { w, 0 };
                }
            }
            if (iter != k - 1) {
                SG.reverse_source_to_vertex(dfs.v);
            }
        }

        found_cut = false;
        return;
    }
}
