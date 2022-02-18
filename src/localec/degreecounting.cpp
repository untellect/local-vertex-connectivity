#include "vertex_connectivity.h"

#include "random.h"

namespace vertex_connectivity {
    void localec(resettable_graph& SG, vertex x, size_t nu, size_t k, bool& found_cut) {
        nu *= 2; // Volume multiplier.

        const size_t source_degree = SG[x].size();
        if (nu <= source_degree) {
            return;
        }
        nu -= source_degree; // Subtract outgoing edges of x from the volume parameter because x is *guaranteed* in L
        const size_t max_edges = nu * k;

        for (size_t iter = 0; iter < k; ++iter) {
            size_t edges_left = util::random_int(1, max_edges);
            SG.new_search();
            SG.visit(x, { x, 0 });
            resettable_graph::internal_location dfs = { x, 0 };
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

                vertex w = SG[dfs.v][dfs.i];
                
                if (SG.visited(w)) {
                    dfs.i++;
                }
                else {
                    SG.visit(w, dfs);
                    dfs = { w, 0 };

                    // Count edges here.
                    if (SG[w].size() >= edges_left) {
                        if (iter != k - 1) {
                            SG.reverse_source_to_vertex(w);
                        }
                        break;
                    }
                    else {
                        edges_left -= SG[w].size();
                    }
                }
            }
        }

        found_cut = false;
        return;
    }
}
