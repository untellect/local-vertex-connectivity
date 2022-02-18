#include "vertex_connectivity.h"

#include "random.h"
#include "diagnostics.h"

namespace vertex_connectivity {
    void localec(resettable_graph& SG, vertex x, size_t nu, size_t k, bool& found_cut) {
        nu *= 2; // Volume multiplier.

        debug::localec_call call;
        call.nu = nu; // Before source degree removal because (1) it counts as a visited vertex and (2) otherwise nu values would be spread out.
        vertex_set unique_vertices_visited;

        size_t source_degree = SG[x].size();
        if (nu <= source_degree) {
            call.vertices_visited = call.unique_vertices_visited = 1;
            debug::localec_log.push_back(call);
            return;
        }
        nu -= source_degree; // Subtract outgoing edges of x from the volume parameter because they won't be counted later.
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
                        call.vertices_visited += SG._visited_vertices.size();
                        std::copy(SG._visited_vertices.begin(), SG._visited_vertices.end(), std::inserter(unique_vertices_visited, unique_vertices_visited.end()));
                        call.unique_vertices_visited = unique_vertices_visited.size();
                        call.found_cut = true;
                        debug::localec_log.push_back(call);

                        found_cut = true;
                        return;
                    }

                    SG.backtrack_inplace(dfs);
                    dfs.i++;
                    continue;
                }

                vertex w = SG[dfs.v][dfs.i];
                call.edges_visited++;

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

            call.vertices_visited += SG._visited_vertices.size();
            std::copy(SG._visited_vertices.begin(), SG._visited_vertices.end(), std::inserter(unique_vertices_visited, unique_vertices_visited.end()));
        }

        call.unique_vertices_visited = unique_vertices_visited.size();
        debug::localec_log.push_back(call);

        found_cut = false;
        return;
    }
}
