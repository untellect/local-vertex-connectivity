#include "pch.h"

#include "mains.h"

void main() {
	std::ios_base::sync_with_stdio(false);

	if (!std::chrono::high_resolution_clock::is_steady) {
		std::cout << "WARNING: std::chrono::high_resolution_clock is not steady!" << std::endl;
	}
	if (SIZE_MAX < 0x7FFFFFFF) {
		std::cout << "ERROR: size_t is not at least a 32 bit integer." << std::endl;
		throw 0;
	}

	mains::generate_RFG_data();
	//mains::generate_RFG2_data();

	mains::RFG_benchmarking();
	//mains::RWG_benchmarking();
	//mains::RHG_benchmarking();

	//mains::RFG_internal_stats();

	mains::process_output();
}