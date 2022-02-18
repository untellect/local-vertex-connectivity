#include <iostream>
#include <algorithm>

#include "io.h"
#include "vertex_connectivity.h"
#include "util.h"

//#include <fstream>
//#include <sstream>

//#include <unordered_set>

int main(const int argc, const char* argv[]) {
	using namespace std;
	using namespace vertex_connectivity;
	ios_base::sync_with_stdio(false);

	if (SIZE_MAX < 0x7FFFFFFF) {
		std::cout << "ERROR: size_t is not at least a 32 bit integer." << endl;
		return -2;
	}

	if(argc != 3) {
		cout << "Usage: " << argv[0] << " <k> <input_file>" << endl;
		return 0;
	}

	if(argc != 3) {
		std::cout << "Expected 1 argument." << endl;
		return 0;
	}

	size_t k = atoi(argv[1]);

	string filename_G = string(argv[2]);
	ifstream in;
	in.open(filename_G);
	if(!in.is_open()) {
		std::cout << "Failed to open " << filename_G << endl;
	}
	adjacency_list G = read_adjlist_from_edgelist_undirected(in);

	vertex_set cut;
	bool found_cut = false;
	for(size_t i = 0; i < 5 && !found_cut; i++)
		vc_decision(G, k, cut, found_cut);

	auto components = get_connected_components_after_cut(G, cut);

	if(components.empty()) {
		return 0;
	}

	vertex_set L = *std::min_element(components.begin(), components.end(), [](vertex_set& a, vertex_set& b) { return a.size() < b.size(); });

	size_t vol = 0;
	for(vertex v : L) {
		vol += G[v].size();
	}

	std::cout << "L " << L.size() << endl;
	std::cout << "LVol " << vol << endl;

	return 0;
}