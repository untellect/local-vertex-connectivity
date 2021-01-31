#include "pch.h"

#include "vertex_connectivity.h"
#include "resettable_graph.h"
#include "util.h"
#include "graphgen.h"
#include "maxflow.h"

namespace {
	static bool flow_success = false;
	static bool localec_basic_success = false;
}

TEST(VC, RFG_MAXFLOW) {
	using namespace graph;
	using namespace vertex_connectivity;
	using namespace maxflow;

	size_t L_size = 5;
	size_t S_size = 2;
	size_t R_size = 5;
	size_t k = 5;

	for (size_t i = 0; i < 5; i++) {
		auto& generated = random_forest_graph_with_planted_cut_and_sets(L_size, S_size, R_size, k);
		adjacency_list G = generated.G;
		vertex_set L = generated.L;
		vertex_set S = generated.S;
		vertex_set R = generated.R;

		adjacency_list SG_adj = split_graph(G);
		resettable_graph SG(SG_adj);
		vertex x_out = 2 * (*L.begin()) + 1;
		vertex y_in = 2 * (*R.begin());

		size_t flow = ford_fulkerson_dfs(SG_adj, x_out, y_in, S_size + 1);
		ASSERT_EQ(flow, S_size) << "Failed on adjacency_list";
		flow = ford_fulkerson_dfs(SG, x_out, y_in, S_size + 1);
		ASSERT_EQ(flow, S_size) << "Failed on resettable_graph";
		SG.reset_changes();
		flow = ford_fulkerson_dfs(SG, x_out, y_in, S_size + 1);
		ASSERT_EQ(flow, S_size) << "Failed on resettable_graph after reset";
	}

	flow_success = true;
}

TEST(VC, RFG_LOCAL_BASIC) {
	using namespace graph;
	using namespace vertex_connectivity;

	ASSERT_TRUE(flow_success) << "Prerequisite test fails.";

	vc_undirected_optimisation_fn* VC = LOCAL_basic;

	for (size_t i = 0; i < 5; i++) {
		adjacency_list G = random_forest_graph_with_planted_cut(2, 5, 100, 20);
		sparsification_labelling labelling = nagamochi_ibaraki_labelling(G);

		vertex_set cut = VC(labelling);
		ASSERT_TRUE(is_valid_vertex_cut(G, cut));
		ASSERT_GE(cut.size(), 5);
	}
	for (size_t i = 0; i < 5; i++) {
		adjacency_list G = random_forest_graph_with_planted_cut(10, 5, 1000, 20);
		sparsification_labelling labelling = nagamochi_ibaraki_labelling(G);

		vertex_set cut = VC(labelling);
		ASSERT_TRUE(is_valid_vertex_cut(G, cut));
		ASSERT_GE(cut.size(), 5);
	}
	for (size_t i = 0; i < 5; i++) {
		adjacency_list G = random_forest_graph_with_planted_cut(50, 5, 50, 20);
		sparsification_labelling labelling = nagamochi_ibaraki_labelling(G);

		vertex_set cut = VC(labelling);
		ASSERT_TRUE(is_valid_vertex_cut(G, cut));
		ASSERT_GE(cut.size(), 5);
	}
}

TEST(VC, RFG_LOCAL) {
	using namespace graph;
	using namespace vertex_connectivity;

	ASSERT_TRUE(flow_success) << "Prerequisite test fails.";

	vc_undirected_optimisation_fn* VC = LOCAL;

	for (size_t i = 0; i < 5; i++) {
		adjacency_list G = random_forest_graph_with_planted_cut(2, 5, 100, 20);
		sparsification_labelling labelling = nagamochi_ibaraki_labelling(G);

		vertex_set cut = VC(labelling);
		ASSERT_TRUE(is_valid_vertex_cut(G, cut));
		ASSERT_GE(cut.size(), 5);
	}
	for (size_t i = 0; i < 5; i++) {
		adjacency_list G = random_forest_graph_with_planted_cut(10, 5, 1000, 20);
		sparsification_labelling labelling = nagamochi_ibaraki_labelling(G);

		vertex_set cut = VC(labelling);
		ASSERT_TRUE(is_valid_vertex_cut(G, cut));
		ASSERT_GE(cut.size(), 5);
	}
	for (size_t i = 0; i < 5; i++) {
		adjacency_list G = random_forest_graph_with_planted_cut(50, 5, 50, 20);
		sparsification_labelling labelling = nagamochi_ibaraki_labelling(G);

		vertex_set cut = VC(labelling);
		ASSERT_TRUE(is_valid_vertex_cut(G, cut));
		ASSERT_GE(cut.size(), 5);
	}
}

TEST(VC, RFG_LOCALplus) {
	using namespace graph;
	using namespace vertex_connectivity;

	ASSERT_TRUE(flow_success) << "Prerequisite test fails.";

	vc_undirected_optimisation_fn* VC = LOCALplus;

	for (size_t i = 0; i < 5; i++) {
		adjacency_list G = random_forest_graph_with_planted_cut(2, 5, 100, 20);
		sparsification_labelling labelling = nagamochi_ibaraki_labelling(G);

		vertex_set cut = VC(labelling);
		ASSERT_TRUE(is_valid_vertex_cut(G, cut));
		ASSERT_GE(cut.size(), 5);
	}
	for (size_t i = 0; i < 5; i++) {
		adjacency_list G = random_forest_graph_with_planted_cut(10, 5, 1000, 20);
		sparsification_labelling labelling = nagamochi_ibaraki_labelling(G);

		vertex_set cut = VC(labelling);
		ASSERT_TRUE(is_valid_vertex_cut(G, cut));
		ASSERT_GE(cut.size(), 5);
	}
	for (size_t i = 0; i < 5; i++) {
		adjacency_list G = random_forest_graph_with_planted_cut(50, 5, 50, 20);
		sparsification_labelling labelling = nagamochi_ibaraki_labelling(G);

		vertex_set cut = VC(labelling);
		ASSERT_TRUE(is_valid_vertex_cut(G, cut));
		ASSERT_GE(cut.size(), 5);
	}
}

TEST(VC, RFG_LOCALHplus) {
	using namespace graph;
	using namespace vertex_connectivity;

	ASSERT_TRUE(flow_success) << "Prerequisite test fails.";

	vc_undirected_optimisation_fn* VC = LOCALHplus;

	for (size_t i = 0; i < 5; i++) {
		adjacency_list G = random_forest_graph_with_planted_cut(2, 5, 100, 20);
		sparsification_labelling labelling = nagamochi_ibaraki_labelling(G);

		vertex_set cut = VC(labelling);
		ASSERT_TRUE(is_valid_vertex_cut(G, cut));
		ASSERT_GE(cut.size(), 5);
	}
	for (size_t i = 0; i < 5; i++) {
		adjacency_list G = random_forest_graph_with_planted_cut(10, 5, 1000, 20);
		sparsification_labelling labelling = nagamochi_ibaraki_labelling(G);

		vertex_set cut = VC(labelling);
		ASSERT_TRUE(is_valid_vertex_cut(G, cut));
		ASSERT_GE(cut.size(), 5);
	}
	for (size_t i = 0; i < 5; i++) {
		adjacency_list G = random_forest_graph_with_planted_cut(50, 5, 50, 20);
		sparsification_labelling labelling = nagamochi_ibaraki_labelling(G);

		vertex_set cut = VC(labelling);
		ASSERT_TRUE(is_valid_vertex_cut(G, cut));
		ASSERT_GE(cut.size(), 5);
	}
}