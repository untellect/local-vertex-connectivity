#include "pch.h"

#include "localvc.h"
#include "random.h"

#include "debug.h"

namespace vertex_connectivity {
	void localec_simple(resettable_graph& SG, vertex x, size_t nu, size_t k, bool& found_cut) {
        nu *= 2; // Volume multiplier.

        size_t max_edges = nu * k + 1; // Should this "+ 1" be removed?

        for (size_t iter = 0; iter < k; ++iter) {
            std::vector<vertex> sample_pool;
            size_t edges_left = max_edges;

            SG.new_search();
            SG.visit(x, { x, 0 });
            resettable_graph::internal_location dfs = { x, 0 }; // dfs "current" pointer
            while (true) {
                if (dfs.i >= SG[dfs.v].size()) {
                    if (dfs.v == x) {
                        found_cut = true;
                        return;
                    }

                    SG.backtrack_dfs(dfs);
                    dfs.i++;
                    continue;
                }

                sample_pool.push_back(dfs.v);
                if (--edges_left == 0) {
                    break;
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
                vertex v = util::select_randomly(sample_pool);
                SG.reverse_source_to_vertex(v);
            }
        }

        found_cut = false;
        return;
	}

    void localec_optimised(resettable_graph& SG, vertex x, size_t nu, size_t k, bool& found_cut) {
        nu *= 2; // Volume multiplier.
        constexpr bool debug = true;

        if constexpr (debug::localec_on && debug) {
            if (!debug::local_debug.empty())
                debug::local_debug.back().finalise();
            debug::local_debug.push_back({ SG.size() / 2, nu });

            debug::local_debug.back().visit_vertex(x); // We visit the first vertex *once* no matter what.
        }

        size_t source_degree = SG[x].size();
        if (nu <= source_degree) {
            return;
        }
        nu -= source_degree; // Subtract outgoing edges of x from the volume parameter because they won't be counted later.
        size_t max_edges = nu * k;// -source_degree;

        for (size_t iter = 0; iter < k; ++iter) {
            if constexpr (debug::localec_on && debug) {
                if(iter != 0)
                    debug::local_debug.back().visit_vertex(x);
            }

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

                    SG.backtrack_dfs(dfs);
                    dfs.i++;
                    continue;
                }

                if (dfs.v != x) {
                    if (--edges_left == 0) {
                        break;
                    }
                }

                vertex w = SG[dfs.v][dfs.i];

                if constexpr (debug::localec_on && debug) {
                    debug::local_debug.back().visit_edge(dfs.v, w);
                }

                if (SG.visited(w)) {
                    dfs.i++;
                }
                else {
                    SG.visit(w, dfs);
                    dfs = { w, 0 };

                    if constexpr (debug::localec_on && debug) {
                        debug::local_debug.back().visit_vertex(w);
                    }
                }
            }
            if (iter != k - 1) {
                SG.reverse_source_to_vertex(dfs.v);
            }
        }

        found_cut = false;
        return;
    }

    void localec_optimised_degreecounting(resettable_graph& SG, vertex x, size_t nu, size_t k, bool& found_cut) {
        nu *= 2; // Volume multiplier.
        constexpr bool debug = true;

        if constexpr (debug::localec_on && debug) {
            if (!debug::localdeg_debug.empty())
                debug::localdeg_debug.back().finalise();
            debug::localdeg_debug.push_back({ SG.size() / 2, nu });
        }

        const size_t source_degree = SG[x].size();
        if (nu <= source_degree) {
            return;
        }
        nu -= source_degree; // Subtract outgoing edges of x from the volume parameter because they won't be counted later.
        const size_t max_edges = nu * k;// -source_degree;

        for (size_t iter = 0; iter < k; ++iter) {
            if constexpr (debug::localec_on && debug) {
                debug::localdeg_debug.back().visit_vertex(x);
            }

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

                    SG.backtrack_dfs(dfs);
                    dfs.i++;
                    continue;
                }

                vertex w = SG[dfs.v][dfs.i];
                
                if constexpr (debug::localec_on && debug) {
                    debug::localdeg_debug.back().visit_edge(dfs.v, w);
                }
                
                if (SG.visited(w)) {
                    dfs.i++;
                }
                else {
                    SG.visit(w, dfs);
                    dfs = { w, 0 };

                    if constexpr (debug::localec_on && debug) {
                        debug::localdeg_debug.back().visit_vertex(w);
                    }

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
    }
    void localec_optimised_marking_degreecounting(resettable_graph& SG, vertex x, size_t nu, size_t k, bool& found_cut) {
        nu *= 2; // Volume multiplier.
        constexpr bool debug = true;

        if constexpr (debug::localec_on && debug) {
            if (!debug::localhdeg_debug.empty())
                debug::localhdeg_debug.back().finalise();
            debug::localhdeg_debug.push_back({ SG.size() / 2, nu });
        }

        size_t source_degree = SG[x].size();
        if (nu <= source_degree) {
            return;
        }
        nu -= source_degree; // Subtract outgoing edges of x from the volume parameter because they won't be counted later.
        const size_t max_edges = nu;// -source_degree;

        for (size_t iter = 0; iter < k; ++iter) {
            if constexpr (debug::localec_on && debug) {
                debug::localhdeg_debug.back().visit_vertex(x);
            }

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

                    SG.backtrack_dfs(dfs);
                    dfs.i++;
                    continue;
                }

                vertex w = SG[dfs.v][dfs.i];

                if constexpr (debug::localec_on && debug) {
                    debug::localhdeg_debug.back().visit_edge(dfs.v, w);
                }

                if (SG.visited(w)) {
                    dfs.i++;
                }
                else {
                    SG.visit(w, dfs);
                    dfs = { w, 0 };

                    if constexpr (debug::localec_on && debug) {
                        debug::localhdeg_debug.back().visit_vertex(w);
                    }

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
    }

    void localec_optimised_marking_degreecounting_vol3(resettable_graph& SG, vertex x, size_t nu, size_t k, bool& found_cut) {
        nu *= 3; // Volume multiplier.
        constexpr bool debug = true;

        if constexpr (debug::localec_on && debug) {
            if (!debug::localhdeg_debug.empty())
                debug::localhdeg_debug.back().finalise();
            debug::localhdeg_debug.push_back({ SG.size() / 2, nu });
        }

        size_t source_degree = SG[x].size();
        if (nu <= source_degree) {
            return;
        }
        nu -= source_degree; // Subtract outgoing edges of x from the volume parameter because they won't be counted later.
        const size_t max_edges = nu;// -source_degree;

        for (size_t iter = 0; iter < k; ++iter) {
            if constexpr (debug::localec_on && debug) {
                debug::localhdeg_debug.back().visit_vertex(x);
            }

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

                    SG.backtrack_dfs(dfs);
                    dfs.i++;
                    continue;
                }

                vertex w = SG[dfs.v][dfs.i];

                if constexpr (debug::localec_on && debug) {
                    debug::localhdeg_debug.back().visit_edge(dfs.v, w);
                }

                if (SG.visited(w)) {
                    dfs.i++;
                }
                else {
                    SG.visit(w, dfs);
                    dfs = { w, 0 };

                    if constexpr (debug::localec_on && debug) {
                        debug::localhdeg_debug.back().visit_vertex(w);
                    }

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
    }
}
