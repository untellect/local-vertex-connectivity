#include "pch.h"

#include "vertex_connectivity.h"
#include "localvc.h"
#include "hrg.h"
#include "sparsify.h"
#include "random.h"
#include "util.h"

#include "debug.h"

namespace vertex_connectivity {
	vertex_set doubling_pick_first(sparsification_labelling& labelling, vc_undirected_decision_fn vc) {
		bool found_cut = false;
		vertex_set cut;

		for (size_t k = 1; !found_cut; k *= 2) {
			adjacency_list G = labelling.get_sparsified_undirected_graph(k);
			vc(G, k, cut, found_cut);
		}

		return cut;
	}
	vertex_set doubling_linear_retry(sparsification_labelling& labelling, vc_undirected_decision_fn vc) {
		vertex_set cut;
		size_t low = 0;
		bool found_cut = false;
		// Find a starting point by doubling.
		for (size_t k = 1; !found_cut; k *= 2) {
			adjacency_list G = labelling.get_sparsified_undirected_graph(k);			
			vc(G, k, cut, found_cut);
			if(!found_cut)
				low = k;
		}

		// While the previous iteration found a cut, try to find a smaller cut.
		// If cut size is (low), then we found a smaller cut while doubling.
		for (size_t k = cut.size(); low < k; k = cut.size()) {
			adjacency_list G = labelling.get_sparsified_undirected_graph(k);
			found_cut = false;
			vc(G, k, cut, found_cut);
			if (!found_cut)
				low = k;
		}

		return cut;
	}

	template<
		localec_fn localec,
		unbalancedvc_fn unbalancedvc,
		balancedvc_fn balancedvc ,
		localbasedvc_fn local
	>
	void LOCAL_decision(const adjacency_list& G, size_t k, vertex_set& cut, bool& found_cut) {
		local(G, k, cut, found_cut, localec, unbalancedvc, balancedvc);
	}

	vertex_set LOCAL_basic(sparsification_labelling& labelling) {
		return doubling_linear_retry(labelling, LOCAL_decision<
			localec_simple,
			unbalancedvc_undirected_edgesampling_pickfirst,
			balancedvc_edgesampling_fordfulkerson_dfs_pickfirst,
			vertex_connectivity_localbased_edgesampling
		>);
	}
	vertex_set LOCAL_basic_balanced_only(sparsification_labelling& labelling) {
		return doubling_linear_retry(labelling, LOCAL_decision<
			localec_simple,
			unbalancedvc_skip,
			balancedvc_edgesampling_fordfulkerson_dfs_pickfirst,
			vertex_connectivity_localbased_edgesampling
		>);
	}
	vertex_set LOCAL_basic_unbalanced_only(sparsification_labelling& labelling) {
		return doubling_linear_retry(labelling, LOCAL_decision<
			localec_simple,
			unbalancedvc_undirected_edgesampling_pickfirst,
			balancedvc_skip,
			vertex_connectivity_localbased_edgesampling
		>);
	}

	vertex_set LOCAL(sparsification_labelling& labelling) {
		return doubling_linear_retry(labelling, LOCAL_decision<
			localec_optimised,
			unbalancedvc_undirected_edgesampling_pickfirst,
			balancedvc_edgesampling_fordfulkerson_dfs_pickfirst,
			vertex_connectivity_localbased_edgesampling_skiptrivialvolume
		>);
	}
	vertex_set LOCALplus(sparsification_labelling& labelling) {
		return doubling_linear_retry(labelling, LOCAL_decision<
			localec_optimised_degreecounting,
			unbalancedvc_undirected_edgesampling_pickfirst,
			balancedvc_edgesampling_fordfulkerson_dfs_pickfirst,
			vertex_connectivity_localbased_edgesampling_skiptrivialvolume
		>);
	}
	vertex_set LOCALHplus(sparsification_labelling& labelling) {
		return doubling_linear_retry(labelling, LOCAL_decision<
			localec_optimised_marking_degreecounting,
			unbalancedvc_undirected_edgesampling_pickfirst,
			balancedvc_edgesampling_fordfulkerson_dfs_pickfirst,
			vertex_connectivity_localbased_edgesampling_skiptrivialvolume
		>);
	}

	vertex_set LOCALF(sparsification_labelling& labelling) {
		return doubling_pick_first(labelling, LOCAL_decision<
			localec_optimised,
			unbalancedvc_undirected_edgesampling_pickbest,
			balancedvc_edgesampling_fordfulkerson_dfs_pickbest,
			vertex_connectivity_localbased_edgesampling_skiptrivialvolume
		>);
	}
	vertex_set LOCALFplus(sparsification_labelling& labelling) {
		return doubling_pick_first(labelling, LOCAL_decision<
			localec_optimised_degreecounting,
			unbalancedvc_undirected_edgesampling_pickbest,
			balancedvc_edgesampling_fordfulkerson_dfs_pickbest,
			vertex_connectivity_localbased_edgesampling_skiptrivialvolume
		>);
	}
	vertex_set LOCALFHplus(sparsification_labelling& labelling) {
		return doubling_pick_first(labelling, LOCAL_decision<
			localec_optimised_marking_degreecounting,
			unbalancedvc_undirected_edgesampling_pickbest,
			balancedvc_edgesampling_fordfulkerson_dfs_pickbest,
			vertex_connectivity_localbased_edgesampling_skiptrivialvolume
		>);
	}
	vertex_set LOCALFHplus3(sparsification_labelling& labelling) {
		return doubling_pick_first(labelling, LOCAL_decision<
			localec_optimised_marking_degreecounting_vol3,
			unbalancedvc_undirected_edgesampling_pickbest,
			balancedvc_edgesampling_fordfulkerson_dfs_pickbest,
			vertex_connectivity_localbased_edgesampling_skiptrivialvolume
		>);
	}

	vertex_set LOCALFplusNOFF(sparsification_labelling& labelling) {
		return doubling_pick_first(labelling, LOCAL_decision<
			localec_optimised_degreecounting,
			unbalancedvc_undirected_edgesampling_pickbest,
			balancedvc_skip,
			vertex_connectivity_localbased_edgesampling_skiptrivialvolume
		>);
	}
	

	vertex_set LOCALFHplus3NOFF(sparsification_labelling& labelling) {
		return doubling_pick_first(labelling, LOCAL_decision<
			localec_optimised_marking_degreecounting_vol3,
			unbalancedvc_undirected_edgesampling_pickbest,
			balancedvc_skip,
			vertex_connectivity_localbased_edgesampling_skiptrivialvolume
		>);
	}

	// Warning: This version misbehaves when k is large (near or even above n) because vertices are sampled with repetition.
	template<hrg::min_vc_split_fn min_vc_split>
	void HRG_decision(const adjacency_list& G, size_t k, vertex_set& cut, bool& found_cut) {
		const size_t n = G.size();

		double iterations = k < n ? ceil(1 / log2((double)n / k)) : k;

		if constexpr (debug::hrg_decision) {
			std::cout << iterations << " iterations at k=" << k << std::endl;
		}

		for (size_t i = 0; i < iterations; i++) {
			vertex_set iter_cut;
			bool iter_found = false;
			vertex source = util::random_int(0, n - 1);

			if constexpr (debug::hrg_decision) {
				auto t = std::chrono::steady_clock.now();
				min_vc_split(G, source, k, iter_cut, iter_found); // Individually timed.
				auto dt = (std::chrono::steady_clock.now() - t);
				std::cout << (double)std::chrono::duration_cast<std::chrono::microseconds>(dt) / 1000000 << " seconds." << std::endl;
			}
			else {
				min_vc_split(G, source, k, iter_cut, iter_found); // Just run.
			}

			if (iter_found && iter_cut.size() < k) {
				found_cut = true;
				swap(cut, iter_cut);
				k = cut.size();
			}
		}
	}

	// Warning: This version misbehaves when k is large (near or even above n) because vertices are sampled with repetition.
	template<hrg::min_vc_split_fn min_vc_split>
	void HRG_decision2(const adjacency_list& G, size_t k, vertex_set& cut, bool& found_cut) {
		const size_t n = G.size();

		double iterations = k < n ? ceil(1 / log2((double)n / k)) : k;

		if constexpr (debug::hrg_decision) {
			std::cout << iterations << " iterations at k=" << k << std::endl;
		}

		edge_sampler sampler(G);

		for (size_t i = 0; i < iterations; i++) {
			vertex_set iter_cut;
			bool iter_found = false;
			vertex source = sampler.get_source(); // util::random_int(0, n - 1);

			if constexpr (debug::hrg_decision) {
				auto t = std::chrono::steady_clock.now();
				min_vc_split(G, source, k, iter_cut, iter_found); // Individually timed.
				auto dt = (std::chrono::steady_clock.now() - t);
				std::cout << (double)std::chrono::duration_cast<std::chrono::microseconds>(dt) / 1000000 << " seconds." << std::endl;
			}
			else {
				min_vc_split(G, source, k, iter_cut, iter_found); // Just run.
			}

			if (iter_found && iter_cut.size() < k) {
				found_cut = true;
				swap(cut, iter_cut);
				k = cut.size();
			}
		}
	}


	/* The version where we use a low degree vertex for doubling. */
	template<hrg::min_vc_split_fn min_vc_split>
	vertex_set doubling_hrg(sparsification_labelling& labelling) {
		size_t n = labelling.get_n();

		vertex_set cut;
		bool found_cut = false;

		size_t k = 1;
		for (; !found_cut; k *= 2) {
			adjacency_list G = labelling.get_sparsified_undirected_graph(k);
			vertex source = labelling.get_last_vertex();
			min_vc_split(G, source, k, cut, found_cut);
		}

		k = cut.size(); // kappa <= k < 4 * kappa
	
		for (size_t i = 0; i < ceil(1 / log2((double)n / k)); i++) {
			adjacency_list G = labelling.get_sparsified_undirected_graph(k);
			vertex source = util::random_int(0, n - 1);
			min_vc_split(G, source, k, cut, found_cut);
			if (found_cut) {
				k = cut.size();
			}
		}

		return cut;
	}

	vertex_set HRG_v1(sparsification_labelling& labelling) {
		return doubling_pick_first(labelling, HRG_decision<hrg::min_vc_split_v1>);
	}
	vertex_set HRG_v2(sparsification_labelling& labelling) {
		return doubling_pick_first(labelling, HRG_decision<hrg::min_vc_split_v2>);
	}
	vertex_set HRG_v3(sparsification_labelling& labelling) {
		return doubling_pick_first(labelling, HRG_decision<hrg::min_vc_split_v3>);
	}
	vertex_set HRG_v4(sparsification_labelling& labelling) {
		return doubling_hrg<hrg::min_vc_split_v1>(labelling);
	}
	vertex_set HRG_v5(sparsification_labelling& labelling) {
		return doubling_hrg<hrg::min_vc_split_v2>(labelling);
	}
	vertex_set HRG_v6(sparsification_labelling& labelling) {
		return doubling_hrg<hrg::min_vc_split_v3>(labelling);
	}
	vertex_set HRG_v7(sparsification_labelling& labelling) {
		return doubling_pick_first(labelling, HRG_decision2<hrg::min_vc_split_v3>);
	}
}
