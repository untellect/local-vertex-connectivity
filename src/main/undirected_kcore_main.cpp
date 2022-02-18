#include "vertex_connectivity.h"
#include "io.h"
#include "util.h"

#include <iostream>
#include <fstream>

int main(const int argc, const char* argv[]) {
	using namespace std;
	using namespace vertex_connectivity;
	ios_base::sync_with_stdio(false);

	if (SIZE_MAX < 0x7FFFFFFF) {
		cout << "ERROR: size_t is not at least a 32 bit integer." << endl;
		return -2;
	}
	if(argc != 3) {
		cout << "Usage: " << argv[0] << " <k> <input_file>" << endl;
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

	adjacency_list G;
	{
		ifstream in;
		const char* filename = argv[2];
		in.open(filename);
		if(!in.is_open()) {
			cout << "Failed to open " << filename << endl;
		}
		try {
			G = read_adjlist_from_edgelist_undirected(in);
			// Some of the inputs are directed, read as undirected (may contain duplicate edges)
			inplace_remove_duplicate_edges(G);
		}
		catch (...) {
			cout << "Error reading input file." << endl;
			return -4;
		}
	}

	G = kcore_undirected(G, k);
	G = largest_connected_component_undirected(G);
	write_adjlist_to_edgelist_undirected(cout, G);

	return 0;
}