#include <iostream>
#include <fstream>
#include <sstream>

#include <unordered_set>

int main(const int argc, const char* argv[]) {
	using namespace std;
	ios_base::sync_with_stdio(false);

	if (SIZE_MAX < 0x7FFFFFFF) {
		std::cout << "ERROR: size_t is not at least a 32 bit integer." << endl;
		return -2;
	}

	string filename_meta, filename_G;
	if(argc == 3) {
		filename_meta = argv[1];
		filename_G = argv[2];
	}
	else if (argc == 2) {
		filename_meta = string(argv[1]) + ".meta";
		filename_G = string(argv[1]) + ".txt";
	}
	else {
		std::cout << "Expected 1 or 2 arguments." << endl;
		return 0;
	}

	unordered_set<size_t> L;
	{
		ifstream in;
		in.open(filename_meta);
		if(!in.is_open()) {
			std::cout << "Failed to open " << filename_meta << endl;
		}
		while(in.peek() == '#')
			in.ignore(10000, '\n');
		
		while(in) {
			string line;
			std::getline(in, line);
			std::istringstream iss(line);
			string header;
			iss >> header;
			if(header != "L:") continue;
			while(iss) {
				size_t v;
				iss >> v;
				L.insert(v);
			}
		}
	}

	size_t vol_out = 0, vol_in = 0;
	{
		ifstream in;
		in.open(filename_G);
		if(!in.is_open()) {
			std::cout << "Failed to open " << filename_G << endl;
		}
		while(in.peek() == '#')
			in.ignore(10000, '\n');

		size_t s, t;
		while (in >> s >> t) {
			if(L.count(s))
				vol_out++;
			if(L.count(t))
				vol_in++;
		}
	}

	std::cout << "L " << L.size() << endl;
	std::cout << "LVolout " << vol_out << endl;
	std::cout << "LVolin " << vol_in << endl;

	return 0;
}