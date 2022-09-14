#pragma once

#include "resettable_graph.h"
#include "sparsify.h"
#include "util.h"

namespace vertex_connectivity {
	// is residual graph SG k-connected near x?
	void localec(resettable_graph& SG, vertex x, size_t v, size_t k, bool& found_cut);
	// Does G have an unbalanced vertex cut smaller than k?
	void unbalancedvc(const adjacency_list& G, resettable_graph& SG, edge_sampler& sampler, size_t k, size_t min_vol, size_t max_vol, double sample_size_multiplier, vertex_set& cut, bool& found_cut);
	// Does G have a balanced vertex cut smaller than k?
	void balancedvc(const adjacency_list& G, resettable_graph& SG, edge_sampler& sampler, size_t k, size_t repetitions, vertex_set& cut, bool& found_cut);

	// Maximum flow from vertex x to vertex y, up to a maximum value k.
	size_t maxflow(resettable_graph& SG, vertex x, vertex y, size_t k);

	// Is G k-connected? (a cut that is found should be probably optimal)
	void vc_decision(const adjacency_list& G, size_t k, vertex_set& cut, bool& found_cut);
	// What is the connectivity of G?
	vertex_set vc_optimisation(const adjacency_list& G);
	vertex_set vc_optimisation(sparsification_labelling& labelling);
}