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

                if (dfs.v != x) {
                    if (--edges_left == 0) {
                        break;
                    }
                }

                vertex w = SG[dfs.v][dfs.i];
                call.edges_visited++;

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
            call.vertices_visited += SG._visited_vertices.size();
            std::copy(SG._visited_vertices.begin(), SG._visited_vertices.end(), std::inserter(unique_vertices_visited, unique_vertices_visited.end()));
        }

        call.unique_vertices_visited = unique_vertices_visited.size();
        debug::localec_log.push_back(call);

        found_cut = false;
        return;
    }
}
