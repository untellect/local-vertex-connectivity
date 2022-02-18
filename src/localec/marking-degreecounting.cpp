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
        const size_t max_edges = nu;

        for (size_t iter = 0; iter < k; ++iter) {
            size_t markable_left = max_edges;                   // Always mark the full amount.
            size_t edges_left = util::random_int(1, max_edges); // This is the chosen edge to reverse from.
            vertex reversal_target = x;
            SG.new_search();
            SG.visit(x, { x, 0 });
            resettable_graph::internal_location dfs = { x, 0 };
            while (markable_left > 0) {
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

                    // Mark up to (markable_left) edges at vertex (w).
                    size_t marked = SG.mark_edges_degreecounting(w, markable_left);
                    markable_left -= marked;
                    if (edges_left) {
                        if (marked >= edges_left) {
                            edges_left = 0;
                            reversal_target = w;
                        }
                        else {
                            edges_left -= marked;
                        }
                    }
                }
            }
            if (iter != k - 1) {
                SG.reverse_source_to_vertex(reversal_target);
            }
        }

        found_cut = false;
        return;
    }
}
