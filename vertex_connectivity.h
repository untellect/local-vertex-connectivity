#pragma once

#include "graph_basics.h"
#include "sparsify.h"

namespace vertex_connectivity {
	using namespace graph;

	typedef void(vc_undirected_decision_fn)(const adjacency_list& G, size_t k, vertex_set& cut, bool& found_cut);
	typedef vertex_set(vc_undirected_optimisation_from_decision_fn)(sparsification_labelling& labelling, vc_undirected_decision_fn vc);
	vc_undirected_optimisation_from_decision_fn doubling_pick_first, doubling_linear_retry;

	typedef vertex_set(vc_undirected_optimisation_fn)(sparsification_labelling& labelling);
	// versions 2-3 & 5-6 skip calculations to find cut candidates when sink excess is high.
	// versions 4-6 use a nonrandom degree at-most-k vertex for doubling (which doesn't count towards error rate because nonrandom)
	vc_undirected_optimisation_fn HRG_v1, HRG_v2, HRG_v3, HRG_v4, HRG_v5, HRG_v6, HRG_v7;
	vc_undirected_optimisation_fn LOCAL_basic;
	vc_undirected_optimisation_fn LOCAL,  LOCALplus,  LOCALHplus;
	vc_undirected_optimisation_fn LOCALF, LOCALFplus, LOCALFHplus, LOCALFHplus3;
	vc_undirected_optimisation_fn LOCALFplusNOFF, LOCALFHplus3NOFF;
}