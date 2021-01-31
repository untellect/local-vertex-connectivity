#include "pch.h"

#include "mains.h"

#include "config.h"
#include "vc_benchmarker.h"
#include "graphgen.h"


namespace mains {
	void RFG_generation_experiment() {
		using namespace std;
		using namespace chrono;
		using namespace graph;

		high_resolution_clock::time_point t1, t2;

		size_t L = 5;
		size_t S = 10;
		size_t k = 60;

		const bool use_gen1 = true;
		const bool use_gen2 = true;

		cout << "L: " << L << endl;
		cout << "S: " << S << endl;
		cout << "k: " << k << endl;
		cout << "n";
		if constexpr (use_gen1) {
			cout << "\tgen1\tm1";
		}
		if constexpr (use_gen2) {
			cout << "\tgen2\tm2";
		}
		cout << endl;

		for (size_t n = 5000; n <= 1000000; n += 5000) // Nested loop without extra braces
		for (size_t i = 0; i < 1; i++) {
			cout << n;

			if (use_gen1) {
				t1 = high_resolution_clock::now();
				adjacency_list G1 = random_forest_graph_with_planted_cut(L, S, n - L - S, k);
				t2 = high_resolution_clock::now();

				double seconds_1 = (double)duration_cast<milliseconds>(t2 - t1).count() / 1000;

				size_t m1 = 0;
				for (auto& vec : G1)
					m1 += vec.size();

				cout << "\t" << seconds_1 << "\t" << m1 / 2;
			}
			{
				t1 = high_resolution_clock::now();
				adjacency_list G2 = random_forest_graph_with_planted_cut_v2(L, S, n - L - S, k);
				t2 = high_resolution_clock::now();

				double seconds_2 = (double)duration_cast<milliseconds>(t2 - t1).count() / 1000;

				size_t m2 = 0;
				for (auto& vec : G2)
					m2 += vec.size();

				cout << "\t" << seconds_2 << "\t" << m2 / 2;
			}
			cout << endl;
		}
	}

	void pathological_forest_graph_test() {
		using namespace std;
		using namespace vertex_connectivity;
		using namespace graph;

		vc_benchmarker bm({ "LOCALF", "LOCALFd", "LOCFhd3" });

		std::vector<size_t> L_10;
		for (size_t L = 1; L <= 500; L = (11 * L / 10) + (L % 10 ? 1 : 0)) L_10.push_back(L);

		// 31 - (1000-31
		for (size_t L : {0}) {
			size_t S = 7;
			size_t R = 1000 - L - S;
			size_t k = S+6;

			adjacency_list G = pathological_forest_graph_with_planted_cut(L, S, R, k);
			string graphname = "pathological-n1000-L" + to_string(L);
			size_t sample_size = 10;
			bm.run(G, graphname, sample_size);
		}

		bm.print_mode = bm.condensed_summary;
		bm.print_header(cout);
		bm.print_results(cout);

	}

	void sandbox() {
		using namespace std;
		using namespace graph;
		using namespace vertex_connectivity;
	}
}