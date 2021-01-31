#include "pch.h"

#include "localvc.h"
#include "util.h"
#include "maxflow.h"
#include "graphgen.h"

namespace vertex_connectivity {
	void balancedvc_skip(const adjacency_list& G, resettable_graph& SG, size_t k, size_t repetitions, vertex_set& cut, bool& found_cut) {}
	void balancedvc_skip(const adjacency_list& G, size_t k, size_t repetitions, vertex_set& cut, bool& found_cut) {}

	void balancedvc_edgesampling_fordfulkerson_dfs_pickfirst(const adjacency_list& G, resettable_graph& SG, size_t k, size_t repetitions, vertex_set& cut, bool& found_cut) {
		if (found_cut) {
			return;
		}

		const size_t n = G.size();
		edge_sampler sampler(G);

		for (size_t i = 0; i < repetitions; i++) {
			vertex x = sampler.get_source();
			vertex x_out = 2 * x + 1;
			vertex y_in = 2 * sampler.get_source();
			
			size_t flow = maxflow::ford_fulkerson_dfs(SG, x_out, y_in, k);

			if (flow < k) {
				vertex_set reachable_SG = get_reachable(SG, x_out);
				SG.reset_changes();
				cut = get_unsplit_cut_from_reachable_set(reachable_SG, G[x]); // Using trickery.
				found_cut = true;
				return;
			}
			SG.reset_changes();
		}
	}

	void balancedvc_edgesampling_fordfulkerson_dfs_pickbest(const adjacency_list& G, resettable_graph& SG, size_t k, size_t repetitions, vertex_set& cut, bool& found_cut) {
		if (found_cut) {
			k = cut.size();
		}

		const size_t n = G.size();
		edge_sampler sampler(G);

		for (size_t i = 0; i < repetitions; i++) {
			vertex x = sampler.get_source();
			vertex x_out = 2 * x + 1;
			vertex y_in = 2 * sampler.get_target();

			size_t flow = maxflow::ford_fulkerson_dfs(SG, x_out, y_in, k);

			if (flow < k) {
				vertex_set reachable_SG = get_reachable(SG, x_out);
				cut = get_unsplit_cut_from_reachable_set(reachable_SG, G[x]); // Using trickery.
				found_cut = true;
				k = cut.size();
			}
			SG.reset_changes();
		}
	}

	void balancedvc_edgesampling_fordfulkerson_dfs_pickfirst(const adjacency_list& G, size_t k, size_t repetitions, vertex_set& cut, bool& found_cut) {
		if (found_cut) return;

		const size_t n = G.size();
		edge_sampler sampler(G);

		for (size_t i = 0; i < repetitions; i++) {
			vertex x = sampler.get_source();
			vertex x_out = 2 * x + 1;
			vertex y_in = 2 * sampler.get_target();

			adjacency_list SG = split_graph(G);
			size_t flow = maxflow::ford_fulkerson_dfs(SG, x_out, y_in, k);

			if (flow < k) {
				vertex_set reachable_SG = get_reachable(SG, x_out);
				cut = get_unsplit_cut_from_reachable_set(reachable_SG, G[x]); // Using trickery.
				found_cut = true;
				return;
			}
		}
	}

	void balancedvc_edgesampling_fordfulkerson_dfs_pickbest(const adjacency_list& G, size_t k, size_t repetitions, vertex_set& cut, bool& found_cut) {
		const size_t n = G.size();
		edge_sampler sampler(G);

		for (size_t i = 0; i < repetitions; i++) {
			vertex x = sampler.get_source();
			vertex x_out = 2 * x + 1;
			vertex y_in = 2 * sampler.get_target();

			adjacency_list SG = split_graph(G);
			size_t flow = maxflow::ford_fulkerson_dfs(SG, x_out, y_in, k);

			if (flow < k) {
				vertex_set reachable_SG = get_reachable(SG, x_out);
				cut = get_unsplit_cut_from_reachable_set(reachable_SG, G[x]); // Using trickery.
				found_cut = true;
				k = cut.size();
			}
		}
	}
}
