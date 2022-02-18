#include "vertex_connectivity.h"
#include "io.h"
#include "sparsify.h"

#include <iostream>
#include <fstream>
#include <chrono>

int main(const int argc, const char* argv[]) {
	using namespace std;
	using namespace vertex_connectivity;
	ios_base::sync_with_stdio(false);

	if (SIZE_MAX < 0x7FFFFFFF) {
		cout << "ERROR: size_t is not at least a 32 bit integer." << endl;
		return -2;
	}
	if(argc < 3) {
		cout << "Usage: " << argv[0] << " <sample_size> <input_file> [input_file_2 ...]" << endl;
		return 0;
	}
	size_t sample_size = 1;
	try {
		sample_size = atoi(argv[1]);
	}
	catch (const invalid_argument& ia) {
		cout << "Invalid sample_size: " << argv[1] << endl;
		return -3;
	}

	for(int file_i = 2; file_i < argc; file_i++) {
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

		/* Finished reading input. Ready to benchmark. */

		cout << argv[file_i] << endl;
		cout << "k\tseconds" << endl;
		for(size_t i = 0; i < sample_size; ++i) {
			// TODO replace with better output.
			try {
				//auto t0 = chrono::steady_clock::now();
				sparsification_labelling labelling = sparsification_labelling::nagamochi_ibaraki(G);
				labelling.sort();
				auto t1 = chrono::steady_clock::now();
				vertex_set cut = vc_optimisation(labelling);
				auto t2 = chrono::steady_clock::now();
				//double seconds_sparsification = (double)((t1 - t0).count())
				//	* chrono::steady_clock::period::num / chrono::steady_clock::period::den;
				double seconds = (double)((t2 - t1).count())
					* chrono::steady_clock::period::num / chrono::steady_clock::period::den;
				cout << cut.size() << "\t" << seconds << endl;
			} catch (int e) {
				cout << "An exception occurred. Exception Nr. " << e << '\n';
			}
		}
	}



	return 0;
}