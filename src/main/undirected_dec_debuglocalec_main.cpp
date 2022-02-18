#include "vertex_connectivity.h"
#include "io.h"

#include "diagnostics.h"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <map>

int main(const int argc, const char* argv[]) {
	using namespace std;
	using namespace vertex_connectivity;
	ios_base::sync_with_stdio(false);

	if (SIZE_MAX < 0x7FFFFFFF) {
		cout << "ERROR: size_t is not at least a 32 bit integer." << endl;
		return -2;
	}
	if(argc < 4) {
		cout << "Usage: " << argv[0] << " <k> <sample_size> <input_file> [input_file_2 ...]" << endl;
		return 0;
	}
	size_t k = 1;
	try {
		k = atoi(argv[1]);
	}
	catch (const invalid_argument& ia) {
		cout << "Invalid k: " << argv[1] << endl;
		return -3;
	}
	size_t sample_size = 1;
	try {
		sample_size = atoi(argv[2]);
	}
	catch (const invalid_argument& ia) {
		cout << "Invalid sample_size: " << argv[2] << endl;
		return -3;
	}


	for(int file_i = 3; file_i < argc; file_i++) {
		ifstream in;
		const char* filename = argv[file_i];
		in.open(filename);
		if(!in.is_open()) {
			cout << "Failed to open " << filename << endl;
		}

		adjacency_list G;
		try {
			G = read_adjlist_from_edgelist_undirected(in);
		}
		catch (...) {
			cout << "Error reading input file." << endl;
			return -4;
		}

		cout << argv[file_i] << endl;

		debug::localec_log.clear();
		for(size_t i = 0; i < sample_size; ++i) {
			size_t tmp_k = k;
			try {
				sparsification_labelling labelling = sparsification_labelling::nagamochi_ibaraki(G);
				labelling.sort();
				adjacency_list FG = labelling.get_sparsified_undirected_graph(tmp_k);

				vertex_set cut;
				bool found_cut = false;
				vc_decision(FG, tmp_k, cut, found_cut);
			} catch (int e) {
				cout << "An exception occurred. Exception Nr. " << e << '\n';
			}
		}

		std::map<size_t, std::vector<debug::localec_call>> log_by_nu;
		for(debug::localec_call& call : debug::localec_log) {
			if(!log_by_nu.count(call.nu))
				log_by_nu[call.nu] = {};
			log_by_nu[call.nu].push_back(call);
		}
		cout << "nu\tN\tV\tE\tUV" << endl;
		for(auto& [nu, calls] : log_by_nu) {
			size_t V = 0, E = 0, UV = 0;
			for (debug::localec_call& call : calls) {
				V += call.vertices_visited;
				E += call.edges_visited;
				UV += call.unique_vertices_visited;
			}
			cout << nu << "\t" << calls.size() << "\t" << V << "\t" << E << "\t" << UV << endl;
		}
	}



	return 0;
}