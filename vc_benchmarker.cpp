#include "pch.h"

#include "vc_benchmarker.h"
#include "util.h"

#include <iostream>

namespace {
	std::string pad_string(std::string s, size_t target = 15) {
		size_t n = s.size();
		if (n < target)
			return s.append(target - n, ' ');
		else
			return s;
	}

	std::string pad_size_t(std::size_t x, size_t target = 15) {
		return pad_string(std::to_string(x), target);
	}

	std::string time_to_str(const std::chrono::steady_clock::duration& t, size_t sample_size, size_t max_len = 7) {
		double seconds = (double)std::chrono::duration_cast<std::chrono::nanoseconds>(t).count() / 1000000000;
		return std::to_string(seconds / sample_size).substr(0, max_len);
	}
}

namespace vertex_connectivity {
	bool vc_benchmarker::algname_comp(const std::string& alg1, const std::string& alg2) {
		if (alg1 == alg2)
			return false;
		// Which algorithm is first in this list?
		for (std::string alg : { "LOCAL_basic", "LOCAL", "LOCAL+", "LOCALh+", "LOCALF", "LOCALF+", "LOCAFh+", "HRG1", "HRG2", "HRG", "HRG4", "HRG5", "HRG6" }) {
			if (alg == alg1) return true;
			if (alg == alg2) return false;
		}
		return false; // Algorithms not in the list are unordered.
	}

	vc_benchmarker::graph_results::graph_results(const adjacency_list& G, std::string graphid, size_t sample_size) : graphid(graphid), N(sample_size) {
		n = G.size();
		m = 0;
		delta = (n == 0 ? 0 : SIZE_MAX);
		kappa = SIZE_MAX;
		for (const auto& vec : G) {
			m += vec.size();
			delta = std::min(delta, vec.size());
		}
	}

	vc_benchmarker::vc_benchmarker(const std::initializer_list<std::string>& il) {
		std::unordered_set<std::string> algnames = il;
		for (auto& alg : algorithms) {
			alg.active = (algnames.find(alg.name) != algnames.end());
		}
	}
	vc_benchmarker::vc_benchmarker() : vc_benchmarker({ "LOCALF", "LOCALF+", "LOCFh+3", "HRG" }) {}
	vc_benchmarker::vc_benchmarker(std::string algname) : vc_benchmarker({ algname }) {}

	void vc_benchmarker::run_helper(const adjacency_list& G, const std::string& graphid, size_t alg_i) {
		sparsification_labelling labelling = nagamochi_ibaraki_labelling(G);
		labelling.sort(); // After this, sparsification takes O(nk) time per graph. instead of O(m).

		auto t1 = std::chrono::steady_clock::now();
		vertex_set cut = algorithms[alg_i].VC(labelling);
		auto t2 = std::chrono::steady_clock::now();

		if (!is_valid_vertex_cut(G, cut)) {
			std::cout << "Invalid vertex cut!!!" << std::endl;
			std::cout << "graph: " << graphid << std::endl;
			std::cout << "alg: " << algorithms[alg_i].name << std::endl;
			std::cout << "cut:";
			for (vertex v : cut) {
				std::cout << " " << v;
			}
			std::cout << std::endl;
			throw 0;
		}

		size_t k = cut.size();
		results.back().kappa = std::min(results.back().kappa, k);
		results.back().algorithm_results[alg_i].push_back({ k, t2 - t1 });
	}

	void vc_benchmarker::run(const adjacency_list& G, std::string graphid, size_t sample_size) {
		if (!(print_mode & quiet) && results.empty()) {
			print_header(std::cout);
		}

		results.emplace_back(G, graphid, sample_size);
		size_t& kappa = results.back().kappa;

		for (size_t alg_i = 0; alg_i < ALG_VERSIONS; alg_i++) {
			if (!algorithms[alg_i].active) continue;

			for (size_t sample = 0; sample < sample_size; sample++) {
				run_helper(G, graphid, alg_i);

				if (!(print_mode & quiet) && (print_mode == normal)) {
					// Normal: n m k ms alg graph
					auto& latest = results.back().algorithm_results[alg_i].back();
					std::cout << pad_size_t(results.back().n)	<< "\t";
					std::cout << pad_size_t(results.back().m)	<< "\t";
					std::cout << latest.k						<< "\t";
					std::cout << time_to_str(latest.t, 1)		<< "\t";
					std::cout << algorithms[alg_i].name			<< "\t";
					std::cout << graphid						<< std::endl;
				}
			}
		}

		if (!(print_mode & quiet) && (print_mode != normal)) {
			print_latest_results(std::cout);
		}
	}
	void vc_benchmarker::print_header(std::ostream& out) {
		const char del = (print_mode & csv ? ',' : '\t'); // delimiter

		if (print_mode & summary) {
			out << "N" << del;
		}
		
		out << pad_string("n") << del << pad_string("m") << del;
		if (print_mode & summary) {
			out << "kappa" << del;
		}

		if (print_mode & condensed) {
			for (auto& alg : algorithms) {
				if (!alg.active) continue;

				if (print_mode & summary) {
					out << alg.name << del << "success" << del;
				}
				else {
					out << alg.name << del << "k" << del;
				}
			}
		}
		else {
			if (print_mode & summary) {
				out << "seconds" << del << "success" << del;
			}
			else {
				out << "k" << del << "seconds" << del;
			}
			out << "alg" << del;
		}

		out << "graph" << std::endl;
	}

	void vc_benchmarker::print_graph_results(std::ostream& out, graph_results& g_res) {
		const char del = (print_mode & csv ? ',' : '\t'); // delimiter

		size_t N = g_res.N;
		size_t n = g_res.n;
		size_t m = g_res.m;
		size_t kappa = g_res.kappa;
		std::string gid = g_res.graphid;

		if (print_mode & condensed) {
			std::chrono::steady_clock::duration t[ALG_VERSIONS];
			size_t successes[ALG_VERSIONS];
			for (size_t alg_i = 0; alg_i < ALG_VERSIONS; alg_i++) {
				t[alg_i] = std::chrono::seconds(0);
				successes[alg_i] = 0;
			}

			for (size_t res_i = 0; res_i < N; res_i++) {
				if (!(print_mode & summary)) {
					// condensed non-summary (start)
					out << pad_size_t(n) << del << pad_size_t(m) << del;
				}
				for (size_t alg_i = 0; alg_i < ALG_VERSIONS; alg_i++) {
					if (!algorithms[alg_i].active) continue;

					auto& r = g_res.algorithm_results[alg_i][res_i];

					t[alg_i] += r.t;
					if (r.k == kappa)
						successes[alg_i]++;

					if (!(print_mode & summary)) {
						// condensed non-summary (alg)
						out << time_to_str(r.t, 1) << del << r.k << del;
					}
				}
				if (!(print_mode & summary)) {
					// condensed non-summary (end)
					out << gid << std::endl;
				}
			}
			if (print_mode & summary) {
				// condensed summary
				out << N << del << pad_size_t(n) << del << pad_size_t(m) << del << kappa << del;
				for (size_t alg_i = 0; alg_i < ALG_VERSIONS; alg_i++) {
					if (!algorithms[alg_i].active) continue;
					out << time_to_str(t[alg_i], N) << del << successes[alg_i] << del;
				}
				out << gid << std::endl;
			}
		}
		else {
			for (size_t alg_i = 0; alg_i < ALG_VERSIONS; alg_i++) {
				if (!algorithms[alg_i].active) continue;

				std::string alg = algorithms[alg_i].name;
				std::chrono::steady_clock::duration t = std::chrono::seconds(0);
				size_t successes = 0;

				for (size_t res_i = 0; res_i < N; res_i++) {
					auto& r = g_res.algorithm_results[alg_i][res_i];

					t += r.t;
					if (r.k == kappa)
						successes++;

					if (!(print_mode & summary)) {
						// normal
						out << pad_size_t(n) << del << pad_size_t(m) << del << r.k << del << time_to_str(r.t, 1) << del << pad_string(alg) << del << gid << std::endl;
					}
				}
				if (print_mode & summary) {
					// summary
					out << N << del << pad_size_t(n) << del << pad_size_t(m) << del << kappa << del << time_to_str(t, N) << del << successes << del << pad_string(alg) << del << gid << std::endl;
				}
			}
		}
	}

	void vc_benchmarker::print_results(std::ostream& out) {
		for (auto& g_res : results) {
			print_graph_results(out, g_res);
		}
	}

	void vc_benchmarker::print_latest_results(std::ostream& out) {
		print_graph_results(out, results.back());
	}
}