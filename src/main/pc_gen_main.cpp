#include "graphgen.h"
#include "io.h"

#include <iostream>

int main(const int argc, const char* argv[]) {
	using namespace std;
	using namespace vertex_connectivity;
	ios_base::sync_with_stdio(false);

	if(argc < 5) {
		cout << "Usage: " << argv[0] << " <L> <S> <R> <k>" << endl;
		return 0;
	}

	size_t L, S, R, k = 0;
	try {
		L = atoi(argv[1]);
		S = atoi(argv[2]);
		R = atoi(argv[3]);
		k = atoi(argv[4]);
	} catch (const invalid_argument& ia) {
		cout << "Invalid argument: " << ia.what() << endl;
		return -3;
	}

	adjacency_list G = random_forest_graph_with_planted_cut(L, S, R, k);

	write_adjlist_to_edgelist_undirected(cout, G);

	return 0;
}