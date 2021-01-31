#pragma once

#include "resettable_graph.h"
#include "sparsify.h"

namespace vertex_connectivity {
	using namespace graph;

	/*
	 * Does *not* reset the split graph.
	 * Does *not* return the cut. If a cut is found, it can be found from outside through the set of reachable vertices.
	 */
	typedef void(localec_fn)(resettable_graph& SG, vertex x, size_t v, size_t k, bool& found_cut);
	localec_fn localec_simple, localec_optimised, localec_optimised_degreecounting, localec_optimised_marking_degreecounting, localec_optimised_marking_degreecounting_vol3;

	/*
	 * Updates the cut if a cut of size less than k is found.
	 * Resets the split graph.
	 */
	typedef void(unbalancedvc_fn)(const adjacency_list& G, resettable_graph& SG, size_t k, size_t min_vol, size_t max_vol, localec_fn localec, vertex& x, vertex_set& cut, bool& found_cut);
	unbalancedvc_fn unbalancedvc_skip, unbalancedvc_undirected_edgesampling_pickfirst, unbalancedvc_undirected_edgesampling_pickbest;

	/*
	 * Updates the cut if a cut of size less than k is found.
	 * Resets the split graph.
	 */
	typedef void(balancedvc_fn)(const adjacency_list& G, resettable_graph& SG, size_t k, size_t repetitions, vertex_set& cut, bool& found_cut);
	balancedvc_fn balancedvc_skip, balancedvc_edgesampling_fordfulkerson_dfs_pickfirst, balancedvc_edgesampling_fordfulkerson_dfs_pickbest;
	// Versions that create a new residual adjacency_list every time. Performance is about the same.
	typedef void(balancedvc_adj_fn)(const adjacency_list& G, size_t k, size_t repetitions, vertex_set& cut, bool& found_cut);
	balancedvc_adj_fn balancedvc_skip, balancedvc_edgesampling_fordfulkerson_dfs_pickfirst, balancedvc_edgesampling_fordfulkerson_dfs_pickbest;

	/*
	 * Solves the decision version of the problem 
	 */
	typedef void(localbasedvc_fn)(const adjacency_list& G, size_t k, vertex_set& cut, bool& found_cut, localec_fn localec, unbalancedvc_fn unbalancedvc, balancedvc_fn balancedvc);
	localbasedvc_fn vertex_connectivity_localbased_edgesampling, vertex_connectivity_localbased_edgesampling_skiptrivialvolume;
}
