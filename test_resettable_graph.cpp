#include "pch.h"

#include "resettable_graph.h"
#include "graphgen.h"

#include <sstream>

using namespace std;
using namespace graph;

namespace {
	const adjacency_list input1 = {
		{1,2},
		{0},
		{0, 1}
	};
	const string print1 =
		"0: 1 2\n"
		"1: 0\n"
		"2: 0 1\n";
	const adjacency_list input2 = {
		{1,2,3,4},
		{},
		{1, 5},
		{2},
		{2},
		{}
	};
	const string print2 =
		"0: 1 2 3 4\n"
		"1:\n"
		"2: 1 5\n"
		"3: 2\n"
		"4: 2\n"
		"5:\n";

	void dfs_helper(resettable_graph& G, vertex x, ostringstream& ss = ostringstream()) {
		ss << x << ":";

		G.new_search();
		G.visit(x, { x, 0 });
		resettable_graph::internal_location dfs = { x, 0 };
		while (true) {
			if (dfs.i >= G[dfs.v].size()) {
				if (dfs.v == x) break;

				G.backtrack_dfs(dfs);
				dfs.i++;
				continue;
			}

			vertex w = G[dfs.v][dfs.i];

			ss << " " << dfs.v << ">" << w;

			if (G.visited(w)) {
				dfs.i++;
			}
			else {
				G.visit(w, dfs);
				dfs = { w, 0 };
			}
		}
	}

	static bool print_success = false;
	static bool dfs_success = false;
	static bool reverse_success = false;
	static bool reset_success = false;
}

TEST(GRAPHS, resettable_graph_print) {
	resettable_graph G1(input1);
	resettable_graph G2(input2);

	{
		ostringstream ss;
		ss << G1;
		ASSERT_EQ(ss.str(), print1);
	}

	{
		ostringstream ss;
		ss << G2;
		ASSERT_EQ(ss.str(), print2);
	}

	print_success = true;
}

TEST(GRAPHS, resettable_graph_dfs) {
	ASSERT_TRUE(print_success) << "Prerequisite test fails.";

	resettable_graph G(input2);
	{
		ostringstream ss;
		dfs_helper(G, 0, ss);
		string expected = "0: 0>1 0>2 2>1 2>5 0>3 3>2 0>4 4>2";
		ASSERT_EQ(ss.str(), expected);

		ASSERT_TRUE(G.visited(0));
		ASSERT_TRUE(G.visited(5));
	}
	{
		ostringstream ss;
		dfs_helper(G, 3, ss);
		string expected = "3: 3>2 2>1 2>5";
		ASSERT_EQ(ss.str(), expected);

		ASSERT_FALSE(G.visited(0));
		ASSERT_TRUE(G.visited(5));
	}
	{
		ostringstream ss;
		dfs_helper(G, 1, ss);
		string expected = "1:";
		ASSERT_EQ(ss.str(), expected);

		ASSERT_FALSE(G.visited(0));
		ASSERT_FALSE(G.visited(5));
	}
	{
		ostringstream ss;
		dfs_helper(G, 0, ss);
		string expected = "0: 0>1 0>2 2>1 2>5 0>3 3>2 0>4 4>2";
		ASSERT_EQ(ss.str(), expected);

		ASSERT_TRUE(G.visited(0));
		ASSERT_TRUE(G.visited(5));
	}

	dfs_success = true;
}

TEST(GRAPHS, resettable_graph_RFG_SG_dfs) {
	ASSERT_TRUE(print_success) << "Prerequisite test fails.";

	auto& gen = random_forest_graph_with_planted_cut_and_sets(5, 1, 5, 6);
	adjacency_list RFG = gen.G;
	resettable_graph SG(split_graph(RFG));
	vertex x_out = 1 + 2 * (*gen.L.begin());
	vertex s_in = 2 * (*gen.S.begin());
	vertex s_out = s_in + 1;
	vertex y_in = 2 * (*gen.R.begin());

	dfs_helper(SG, x_out);

	ASSERT_TRUE(SG.visited(s_in));
	ASSERT_TRUE(SG.visited(s_out));
	ASSERT_TRUE(SG.visited(y_in));

	SG.reverse_source_to_vertex(y_in);

	dfs_helper(SG, x_out);

	ASSERT_TRUE(SG.visited(s_in));
	ASSERT_FALSE(SG.visited(s_out));
	ASSERT_FALSE(SG.visited(y_in));
}

TEST(GRAPHS, resettable_graph_reverse) {
	ASSERT_TRUE(dfs_success) << "Prerequisite test fails.";

	resettable_graph G(input2);
	dfs_helper(G, 0);
	G.reverse_source_to_vertex(1);
	{
		string expected =
			"0: 4 2 3\n"
			"1: 0\n"
			"2: 1 5\n"
			"3: 2\n"
			"4: 2\n"
			"5:\n";
		ostringstream ss;
		ss << G;
		ASSERT_EQ(ss.str(), expected);
	}
	dfs_helper(G, 0);
	G.reverse_source_to_vertex(5);
	{
		string expected =
			"0: 3 2\n"
			"1: 0\n"
			"2: 1 4\n"
			"3: 2\n"
			"4: 0\n"
			"5: 2\n";
		ostringstream ss;
		ss << G;
		ASSERT_EQ(ss.str(), expected);
	}

	reverse_success = true;
}

TEST(GRAPHS, resettable_graph_reset) {
	ASSERT_TRUE(reverse_success) << "Prerequisite test fails.";

	resettable_graph G(input2);
	dfs_helper(G, 0);
	G.reverse_source_to_vertex(1);
	{
		string expected =
			"0: 4 2 3\n"
			"1: 0\n"
			"2: 1 5\n"
			"3: 2\n"
			"4: 2\n"
			"5:\n";
		ostringstream ss;
		ss << G;
		ASSERT_EQ(ss.str(), expected);
	}
	G.reset_changes();
	{
		ostringstream ss;
		ss << G;
		ASSERT_EQ(ss.str(), print2);
	}
	dfs_helper(G, 0);
	G.reverse_source_to_vertex(1);
	dfs_helper(G, 0);
	G.reverse_source_to_vertex(1);
	{
		string expected =
			"0: 3 2\n"
			"1: 0 2\n"
			"2: 4 5\n"
			"3: 2\n"
			"4: 0\n"
			"5:\n";
		ostringstream ss;
		ss << G;
		ASSERT_EQ(ss.str(), expected);
	}
	G.reset_changes();
	{
		ostringstream ss;
		ss << G;
		ASSERT_EQ(ss.str(), print2);
	}
	dfs_helper(G, 2);
	G.reverse_source_to_vertex(5);
	dfs_helper(G, 5);
	G.reverse_source_to_vertex(1);
	dfs_helper(G, 0);
	G.reverse_source_to_vertex(1);
	{
		string expected =
			"0: 4 2 3\n"
			"1: 2 0\n"
			"2: 5\n"
			"3: 2\n"
			"4: 2\n"
			"5:\n";
		ostringstream ss;
		ss << G;
		ASSERT_EQ(ss.str(), expected);
	}
	G.reset_changes();
	{
		ostringstream ss;
		ss << G;
		ASSERT_EQ(ss.str(), print2);
	}

	reset_success = true;
}
