#pragma once

#include "vertex_connectivity.h"
#include "hrg.h"

#include <array>
#include <chrono>

namespace vertex_connectivity {

	class vc_benchmarker {
	public:
		struct algorithm {
			const vc_undirected_optimisation_fn* VC;
			const std::string name;
			bool active = false;
		};
		static bool algname_comp(const std::string& alg1, const std::string& alg2);

		static const size_t ALG_VERSIONS = 17;
		std::array<algorithm, ALG_VERSIONS> algorithms = {
		vc_benchmarker::algorithm {LOCAL_basic, "LOCAL_basic"},	// 0
		vc_benchmarker::algorithm {LOCAL, "LOCAL"},				// 1
		vc_benchmarker::algorithm {LOCALplus, "LOCALd"},		// 2
		vc_benchmarker::algorithm {LOCALHplus, "LOCALhd"},		// 3
		vc_benchmarker::algorithm {LOCALF, "LOCALF"},			// 4
		vc_benchmarker::algorithm {LOCALFplus, "LOCALFd"},		// 5
		vc_benchmarker::algorithm {LOCALFHplus, "LOCAFhd"},		// 6
		vc_benchmarker::algorithm {LOCALFHplus3, "LOCFhd3"},	// 7
		vc_benchmarker::algorithm {HRG_v1, "HRG1"},				// 8
		/* Versions 2-3 perform *slightly* better than 1 on average (and 5-6 compared to 4). */
		vc_benchmarker::algorithm {HRG_v2, "HRG2"},				// 9
		vc_benchmarker::algorithm {HRG_v3, "HRG"},				// 10
		/*
		 * Versions 4-6 have generally higher running times.
		 * Lower variance for each graph but higher between graphs.
		 */
		vc_benchmarker::algorithm {HRG_v4, "HRG4"},				// 11
		vc_benchmarker::algorithm {HRG_v5, "HRG5"},				// 12
		vc_benchmarker::algorithm {HRG_v6, "HRG6"},				// 13
		vc_benchmarker::algorithm {HRG_v7, "HRG7"},				// 14

		vc_benchmarker::algorithm {LOCALFplusNOFF, "LOCAFdn"},	// 15
		vc_benchmarker::algorithm {LOCALFHplus3NOFF, "LOFhd3n"},// 16 // Becoming a bit ugly...
		};

		/* Using bit masks for print mode specification. */
		enum {
			quiet		= 1 << 0, // Specifically during computation.
			summary		= 1 << 1,
			condensed	= 1 << 2,
			csv			= 1 << 3,
			normal						= 0,
			normal_quiet				= quiet,
			condensed_summary			= condensed | summary,
			condensed_summary_quiet		= condensed | summary | quiet,
			condensed_csv				= condensed | csv,
			condensed_summary_csv		= condensed | summary | csv,
			condensed_summary_csv_quiet = condensed | summary | csv | quiet,
		} print_mode = normal;

	private:
		struct graph_results {
			struct alg_result {
				size_t k;
				std::chrono::steady_clock::duration t;
			};
			std::string graphid;
			size_t N, n, m, delta, kappa;
			std::array<std::vector<alg_result>, ALG_VERSIONS> algorithm_results;

			graph_results(const adjacency_list& G, std::string graphid, size_t sample_size);
		};
		std::vector<graph_results> results;

		void print_graph_results(std::ostream& out, graph_results& g_res);
		void run_helper(const adjacency_list& G, const std::string& graphid, size_t alg_i);
	public:
		vc_benchmarker(const std::initializer_list<std::string>& il);
		vc_benchmarker(std::string algname);
		vc_benchmarker();
		void run(const adjacency_list& G, std::string graphid, size_t sample_size);
		void print_header(std::ostream& out);

		void print_results(std::ostream& out);
		void print_latest_results(std::ostream& out);
	};
}
