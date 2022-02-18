#include "io.h"

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

	for(int file_i = 2; file_i < argc; file_i++) {
		ifstream in;
		const char* filename = argv[file_i];
		in.open(filename);
		if(!in.is_open()) {
			cout << "Failed to open " << filename << endl;
		}

		adjacency_list G;
		try {
			G = read_adjlist_from_edgelist_directed(in);
		}
		catch (...) {
			cout << "Error reading input file." << endl;
			return -4;
		}

		size_t n = G.size();
		size_t m = 0;
		size_t dmin = G.size();
		for(auto& vec : G) {
			m += vec.size();
			dmin = min(dmin, vec.size());
		}
		adjacency_list RG(n);
		for(vertex x = 0; x < n; x++) {
			for(vertex y : G[x])
				RG[y].push_back(x);
		}
		size_t rdmin = G.size();
		for(auto& vec : RG) {
			rdmin = min(rdmin, vec.size());
		}

		cout << argv[file_i] << endl;
		cout << "n\t" << n << endl;
		cout << "m\t" << m << endl;
		cout << "dmin\t" << dmin << endl;
		cout << "rdmin\t" << rdmin << endl;
	}

	return 0;
}