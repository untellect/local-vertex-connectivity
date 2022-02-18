#include "graphgen.h"
#include "io.h"

#include <iostream>

int main(const int argc, const char* argv[]) {
	using namespace std;
	using namespace vertex_connectivity;
	ios_base::sync_with_stdio(false);

	size_t L, S_in, S_out, R, k, d_avg = 0;
	if(argc == 7) {
		try {
			L = atoi(argv[1]);
			S_in = atoi(argv[2]);
			S_out = atoi(argv[3]);
			R = atoi(argv[4]);
			k = atoi(argv[5]);
			d_avg = atoi(argv[6]);
		} catch (const invalid_argument& ia) {
			cout << "Invalid argument: " << ia.what() << endl;
			return -3;
		}
	}
	else if(argc == 6) {
		try {
			L = atoi(argv[1]);
			S_in = atoi(argv[2]);
			S_out = atoi(argv[3]);
			R = 0;
			k = atoi(argv[4]);
			d_avg = atoi(argv[5]);
		} catch (const invalid_argument& ia) {
			cout << "Invalid argument: " << ia.what() << endl;
			return -3;
		}
	}
	else {
		cout << "Usage: " << argv[0] << " <L> <S_in> <S_out> [R] <k> <d_avg>" << endl;
		return 0;
	}

	adjacency_list G = directed_cycles_with_planted_cut(L, S_in, S_out, R, k, d_avg);

	write_adjlist_to_edgelist_directed(cout, G);

	return 0;
}