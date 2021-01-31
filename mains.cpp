#include "pch.h"

#include "mains.h"

#include "graph_basics.h"
#include "resettable_graph.h"
#include "graphgen.h"
#include "vc_benchmarker.h"
#include "config.h"
#include "graphIO.h"
#include "preprocessing.h"
#include "maxflow.h"
#include "util.h"

#include "debug.h"

#include <filesystem>
#include <algorithm>
#include <map>

namespace {
	bool file_output = true;
}

namespace mains {
	// Using the older, slower generator
	void generate_RFG_data() {
		using namespace std;
		using namespace filesystem;
		using namespace vertex_connectivity;

		struct rfg_vars {
			size_t n = 1000;
			size_t S = 5;
			size_t L = 5;
			size_t k = 60;
			size_t duplicates = 5;
		};

		struct rfg_data {
			string name;
			vector<rfg_vars> vars;
		};

		vector<rfg_data> data;

		///* Variable n */
		data.push_back({ "k4-n_to_1000" });
		for (size_t n = 50; n <= 1000; n += 50) {
			data.back().vars.push_back({ n, 4 });
		}
		//data.push_back({ "k7-n_to_1000" });
		//for (size_t n = 50; n <= 1000; n += 50) {
		//	data.back().vars.push_back({ n, 7 });
		//}
		//data.push_back({ "k8-n_to_1000" });
		//for (size_t n = 50; n <= 1000; n += 50) {
		//	data.back().vars.push_back({ n, 8 });
		//}
		//data.push_back({ "k15-n_to_1000" });
		//for (size_t n = 50; n <= 1000; n += 50) {
		//	data.back().vars.push_back({ n, 15 });
		//}
		//data.push_back({ "k31-n_to_10000" });
		//for (size_t n = 1000; n <= 10000; n += 1000) {
		//	data.back().vars.push_back({ n, 31 });
		//}

		///* Variable |L| */
		//data.push_back({ "n1000-L" });
		//for (size_t L = 1; L <= 500; L = (11 * L / 10) + (L % 10 ? 1 : 0)) { // L = 11/10 L (rounded up)
		//	data.back().vars.push_back({ 1000, 5, L });
		//}
		//data.push_back({ "n4000-L" });
		//for (size_t L = 1; L <= 2000; L = (6 * L / 5) + (L % 5 ? 1 : 0)) { // L = 12/10 L (rounded up)
		//	data.back().vars.push_back({ 4000, 5, L });
		//}
		//data.push_back({ "n4000-L-dense" });
		//for (size_t L = 1; L <= 2000; L = (21 * L / 20) + (L % 20 ? 1 : 0)) { // L = 21/20 L (rounded up)
		//	data.back().vars.push_back({ 4000, 5, L, 60, 1 });
		//}
		///* Variable kappa=|S| */
		//data.push_back({ "n1000-S_to_56" });
		//for (size_t S = 1; S < 20; S++) {
		//	data.back().vars.push_back({ 1000, S });
		//}
		//for (size_t S = 20; S < 32; S += 2) {
		//	data.back().vars.push_back({ 1000, S });
		//}
		//for (size_t S = 32; S <= 56; S += 4) {
		//	data.back().vars.push_back({ 1000, S });
		//}
		///* Variable k */
		//data.push_back({ "n1000-k_to_999" });
		//for (size_t k = 6; k <= 999; k = (6 * k / 5) + (k % 5 ? 1 : 0)) { // k = 12/10 k (rounded up)
		//	data.back().vars.push_back({ 1000, 5, 5, k });
		//}
		//data.push_back({ "n4000-k_to_3999" });
		//for (size_t k = 6; k <= 3999; k = (6 * k / 5) + (k % 5 ? 1 : 0)) { // k = 12/10 k (rounded up)
		//	data.back().vars.push_back({ 4000, 5, 5, k });
		//}
		/* LiveJournal comparison */
		//data.push_back({ "k10_n_to_2000" });
		//for (size_t n = 100; n <= 2000; n += 100) {
		//	data.back().vars.push_back({ n, 10 });
		//}
		//data.push_back({ "k9_n_to_2000" });
		//for (size_t n = 100; n <= 2000; n += 100) {
		//	data.back().vars.push_back({ n, 9 });
		//}
		//data.push_back({ "k8-n_to_2000" });
		//for (size_t n = 100; n <= 2000; n += 100) {
		//	data.back().vars.push_back({ n, 8 });
		//}
		///* BerkStan comparison */
		//data.push_back({ "k2_n_to_6000" });
		//for (size_t n = 500; n <= 6000; n += 500) {
		//	data.back().vars.push_back({ n, 2 });
		//}
		//data.push_back({ "k12_n_to_6000" });
		//for (size_t n = 500; n <= 6000; n += 500) {
		//	data.back().vars.push_back({ n, 12 });
		//}
		///* Epinions comparison */
		//data.push_back({ "k5_n_to_11000" });
		//for (size_t n = 1000; n <= 11000; n += 1000) {
		//	data.back().vars.push_back({ n, 5 });
		//}
		//data.push_back({ "k11_n_to_9000" });
		//for (size_t n = 1000; n <= 9000; n += 1000) {
		//	data.back().vars.push_back({ n, 11 });
		//}
		//data.push_back({ "k16_n_to_7000" });
		//for (size_t n = 1000; n <= 7000; n += 1000) {
		//	data.back().vars.push_back({ n, 16 });
		//}
		//data.push_back({ "k17_n_to_7000" });
		//for (size_t n = 1000; n <= 7000; n += 1000) {
		//	data.back().vars.push_back({ n, 17 });
		//}
		//data.push_back({ "k8-n_to_10000" });
		//for (size_t n = 1000; n <= 10000; n += 1000) {
		//	data.back().vars.push_back({ n, 8 });
		//}
		//data.push_back({ "k4-n_to_20000" });
		//for (size_t n = 2000; n <= 20000; n += 2000) {
		//	data.back().vars.push_back({ n, 4 });
		//}
		//data.push_back({ "k50-n_to_1000" });
		//for (size_t n = 100; n <= 1000; n += 100) {
		//	data.back().vars.push_back({ n, 50 });
		//}
		//data.push_back({ "RWG-comparison" });
		//for (auto& [n, k] : {
		//	pair { 1493, 10 },	// LJ
		//	pair { 1205, 9 },	// LJ
		//	pair { 853, 8 },	// LJ
		//	pair { 781, 8 },	// LJ
		//	pair { 10147, 5 },	// EP
		//	pair { 8106, 11 },	// EP
		//	pair { 6246, 16 },	// EP
		//	pair { 5719, 17 },	// EP
		//	pair { 5480, 2 },	// BS
		//	pair { 5352, 12 },	// BS
		//}) {
		//	data.back().vars.push_back({ n, k });
		//}
		//data.push_back({ "k32-n10000" });
		//data.back().vars.push_back({ 10000, 32, 5, 60, 1 });
		//data.push_back({ "k16-n10000" });
		//data.back().vars.push_back({ 10000, 16, 5, 60, 1 });
		//data.push_back({ "k8-n10000" });
		//data.back().vars.push_back({ 10000, 8, 5, 60, 1 });
		//data.push_back({ "k4-n10000" });
		//data.back().vars.push_back({ 10000, 4, 5, 60, 1 });
		/* End of data */

		for (rfg_data& rfg : data) {
			string dir = config::input_top_directory + "RFG/" + rfg.name + "/";
			create_directories(dir);

			for (rfg_vars& vars : rfg.vars) {
				size_t R = vars.n - vars.L - vars.S;
				string name = to_string(vars.L) + "-" + to_string(vars.S) + "-" + to_string(R) + "-" + to_string(vars.k);

				for (size_t i = 0; i < vars.duplicates; i++) {
					adjacency_list G = random_forest_graph_with_planted_cut(vars.L, vars.S, R, vars.k);
					ofstream out;
					out.open(dir + name + "+" + to_string(i) + ".txt");
					write_adjlist_to_edgelist_undirected(out, G);
					cout << "Generated " << name << "+" << i << endl;
				}
			}
		}
	}

	// Using the fast generator
	void generate_RFG2_data() {
		using namespace std;
		using namespace filesystem;
		using namespace vertex_connectivity;

		struct rfg_vars {
			size_t n = 1000;
			size_t S = 5;
			size_t L = 5;
			size_t k = 60;
			size_t duplicates = 5;
		};

		struct rfg_data {
			string name;
			vector<rfg_vars> vars;
		};

		vector<rfg_data> data;

		//data.push_back({ "k63-n_to_10000" });
		//for (size_t n = 1000; n <= 10000; n += 1000) {
		//	data.back().vars.push_back({ n, 63, 5, 128, 5 });
		//}
		//data.push_back({ "k31-n_to_100000" });
		//for (size_t i = 1; i <= 10; i++) {
		//	size_t n = 10000 * i;
		//	data.back().vars.push_back({ n, 31, 5, 64, 5 });
		//}
		//data.push_back({ "k7-n_to_100000" }); // TODO REDO WITH 64 forests?
		//for (size_t i = 1; i <= 10; i++) {
		//	size_t n = 10000 * i;
		//	data.back().vars.push_back({ n, 7 });
		//}
		//data.push_back({ "k32-n100000" });
		//data.back().vars.push_back({ 100000, 32, 5, 64, 1 });
		//data.push_back({ "k16-n100000" });
		//data.back().vars.push_back({ 100000, 16, 5, 64, 1 });
		//data.push_back({ "k8-n100000" });
		//data.back().vars.push_back({ 100000, 8, 5, 64, 1 });
		//data.push_back({ "k4-n100000" });
		//data.back().vars.push_back({ 100000, 4, 5, 64, 1 });
		//data.push_back({ "n10000-S_to_60_few" }); // TODO REDO WITH 64 forests?
		//for (size_t S = 1; S < 20; S++) {
		//	data.back().vars.push_back({ 10000, S, 5, 128, 1 });
		//}
		//for (size_t S = 20; S < 32; S += 2) {
		//	data.back().vars.push_back({ 10000, S, 5, 128, 1 });
		//}
		//for (size_t S = 32; S <= 60; S += 4) {
		//	data.back().vars.push_back({ 10000, S, 5, 128, 1 });
		//}

		for (rfg_data& rfg : data) {
			string dir = config::input_top_directory + "RFG2/" + rfg.name + "/";
			create_directories(dir);

			for (rfg_vars& vars : rfg.vars) {
				size_t R = vars.n - vars.L - vars.S;
				string name = to_string(vars.L) + "-" + to_string(vars.S) + "-" + to_string(R) + "-" + to_string(vars.k);

				for (size_t i = 0; i < vars.duplicates; i++) {
					cout << "Generating " << name << "+" << i;
					adjacency_list G = random_forest_graph_with_planted_cut_v2(vars.L, vars.S, R, vars.k);
					ofstream out;
					out.open(dir + name + "+" + to_string(i) + ".txt");
					write_adjlist_to_edgelist_undirected(out, G);
					cout << "... done!" << endl;
				}
			}
		}
	}

	void RFG_benchmarking() {
		using namespace std;
		using namespace vertex_connectivity;
		using namespace filesystem;

		if (debug::any_debug) {
			cout << "Debug is on!" << endl;
			return;
		}

		if (!file_output) {
			cout << "No file output." << endl;
		}

		size_t sample_size = 5;

		vector<string> algnames = {
			"LOCALFd",
			"LOCFhd3",
			"LOCALF",
			//"HRG",

			//"LOCAL",
			//"LOCALd",
			//"LOCALhd",

			//"LOCAFdn",
			//"LOFhd3n",

			//"HRG7",
		};

		enum sort_by {
			d = 0, // Duplicate id
			L = 1,
			S = 2,
			R = 3,
			k = 4,
		};

		struct exp {
			string exp_name;
			sort_by s = R;
			string family = "RFG";
		};

		vector < exp > experiments = {
			//{ "n1000-L",		L }, // variable |L|
			//{ "n1000-S_to_56",	S }, // variable kappa
			{ "k4-n_to_1000"}, // Basic variable n
			//{ "k7-n_to_1000"},
			//{ "k8-n_to_1000"},
			//{ "k15-n_to_1000"},
			//{ "k8-n_to_10000"}, // RFG-RHG comparison
			//{ "k4-n_to_20000"},
			//{ "RWG-comparison", d }, // Exact parameters for RWG comparison
// Not used
		//	{ "n4000-L",		L },
		//	{ "n1000-k_to_999",	k }, // variable forest count / degree distribution
		//	{ "n4000-k_to_3999",k }, // variable forest count / degree distribution
		//	{ "k50-n_to_1000"}, // Variable n for high k.

// Replaced by RWG-comparison
		//	{ "k10_n_to_2000"}, // LiveJournal comparison
		//	{ "k9_n_to_2000"},
		//	{ "k2_n_to_6000"}, // BerkStan comparison
		//	{ "k12_n_to_6000"},
		//	{ "k5_n_to_11000"}, // Epinions comparison
		//	{ "k11_n_to_9000"},
		//	{ "k16_n_to_7000"},
		//	{ "k17_n_to_7000"},

			//{"n4000-L-dense",	L},
			//{ "k16-n10000"},
			//{ "k8-n10000"},
			//{ "k4-n10000"},

			//{"k31-n_to_10000"},
			//{"k63-n_to_10000", R, "RFG2"},
			//{"k31-n_to_100000", R, "RFG2"},
			//{"k7-n_to_100000", R, "RFG2"},

			//{ "k4-n100000", R, "RFG2" },
			//{ "k8-n100000", R, "RFG2" },
			//{ "k16-n100000", R, "RFG2" },
			//{ "k32-n100000", R, "RFG2" },

			//{"n10000-S_to_60_few", S, "RFG2"},
		};
		for (const auto& [exp_name, s, family] : experiments) {
			for (const string& algname : algnames) {
				string dir_in = config::input_top_directory + family + "/" + exp_name + "/";
				vector<string> files;
				for (const directory_entry& file : directory_iterator(dir_in)) {
					if (file.is_regular_file())
						files.push_back(file.path().filename().string());
				}
				const sort_by& sb = s;
				sort(files.begin(), files.end(), [sb](string a, string b) {
					// Save duplicate identifier
					int i = stoi(a.substr(a.find("+") + 1));
					int j = stoi(b.substr(b.find("+") + 1));

					if (sb == d)
						return i < j;

					// Remove the duplicate identfier.
					a = a.substr(0, a.find("+"));
					b = b.substr(0, b.find("+"));
					switch (sb) {
					case k: // Remove everything before the first three "-"
						a = a.substr(a.find("-") + 1);
						b = b.substr(b.find("-") + 1);
					case R: // Remove everything before the first two "-"
						a = a.substr(a.find("-") + 1);
						b = b.substr(b.find("-") + 1);
					case S: // Remove everything before the first "-"
						a = a.substr(a.find("-") + 1);
						b = b.substr(b.find("-") + 1);
					case L: // Remove everything after the first "-"
						a = a.substr(0, a.find("-"));
						b = b.substr(0, b.find("-"));
					}
					int x = stoi(a);
					int y = stoi(b);
					return x < y || ((x == y) && (i < j)); // Sort as integers with duplicate id as tiebreaker.
				});

				vc_benchmarker bm(algname);

				for (const string& file : files) {
					ifstream in;
					in.open(dir_in + file);
					adjacency_list G = read_adjlist_from_edgelist_undirected(in);
					bm.run(G, file, sample_size);
				}

				if (file_output) {
					ofstream out;
					string dir_out = config::output_top_directory + "RFG/" + exp_name + "/";
					create_directories(dir_out + "detailed/");
					out.open(dir_out + "detailed/" + algname + ".csv");
					bm.print_mode = bm.condensed_csv;
					bm.print_header(out);
					bm.print_results(out);
					out.close();
					out.open(dir_out + algname + ".csv");
					bm.print_mode = bm.condensed_summary_csv;
					bm.print_header(out);
					bm.print_results(out);
					out.close();
				}
			}
		}
	}

	void RHG_benchmarking() {
		using namespace std;
		using namespace filesystem;
		using namespace vertex_connectivity;

		if (debug::any_debug) {
			cout << "Debug is on!" << endl;
			return;
		}

		if (!file_output) {
			cout << "No file output." << endl;
		}

		// Continue where we left off.
		std::string latest_name = "HRG7";  size_t latest_size = 13; size_t next_i = 0;

		size_t sample_size = 5;

		vector<string> algnames = {
			//"HRG",
			//"LOCALFd",
			//"LOCFhd3",
			//"LOCALF",
			//"LOCAL",
			//"LOCALd",
			//"LOCALhd",

			"HRG7",
		};

		for (string algname : algnames) {
			for (size_t size_param = 18; size_param >= 10; size_param--) {
				if (algname == latest_name) {
					// Continue where we left off.
					if (size_param > latest_size)
						size_param = latest_size;
				}
				for (size_t degree_param : {5}) {
					for (size_t power_param : {10}) {
						string rhg_id_base = to_string(size_param) + "-" + to_string(degree_param) + "-" + to_string(power_param);
						for (size_t i = 0; i < 20; i++) {
							if (algname == latest_name) {
								// Continue where we left off.
								if (size_param == latest_size && i < next_i)
									i = next_i;
							}

							/* INPUT */
							string rhg_id = rhg_id_base + "+" + to_string(i);
							string graphname = "RHG/" + to_string(size_param) + "/" + rhg_id;
							string input_file = config::input_top_directory + graphname + ".txt";
							ifstream in;
							in.open(input_file);
							adjacency_list G = read_adjlist_from_edgelist_undirected(in);

							/* BENCHMARK */
							vc_benchmarker bm(algname);
							bm.run(G, graphname, sample_size);

							/* OUTPUT */
							if (file_output) {
								ofstream out;
								string dir_out = config::output_top_directory + "RHG/" + rhg_id_base + "/";
								create_directories(dir_out + "detailed/");
								out.open(dir_out + "detailed/" + algname + ".csv", std::ios_base::app);
								bm.print_mode = bm.condensed_csv;
								if (i == 0) {
									bm.print_header(out);
								}
								bm.print_results(out);
								out.close();
								out.open(dir_out + algname + ".csv", std::ios_base::app);
								bm.print_mode = bm.condensed_summary_csv;
								if (i == 0) {
									bm.print_header(out);
								}
								bm.print_results(out);
								out.close();
							}
						}
					}
				}
			}
		}
	}

	void RWG_benchmarking() {
		using namespace std;
		using namespace filesystem;
		using namespace vertex_connectivity;
		using namespace preprocessing;

		if (debug::any_debug) {
			cout << "Debug is on!" << endl;
			return;
		}

		if (!file_output) {
			cout << "No file output." << endl;
		}

		struct params_t {
			string name;
			vector<size_t> k_values;
		};

		size_t sample_size = 25;

		std::vector<params_t> all_params = {
			{ "web-BerkStan", {80, 93} }, // Berkstan
			{ "soc-Epinions1", {9, 12, 17, 19} }, // Epinions
			{ "com-lj.ungraph", {160, 200, 231, 250} }, // livejournal
		};

		std::vector<string> algnames = {
			//"HRG",
			//"LOCALF",
			//"LOCALFd",
			"LOCFhd3",
			//"LOCAL",
			//"LOCALd",
			//"LOCALhd",
		};

		for (string algname : algnames) {
			for (params_t& params : all_params) {
				vc_benchmarker bm(algname);
				{
					string input_file = config::input_top_directory + "RWG/" + params.name + ".txt";
					size_t skip_lines = 4; // For the SNAP graphs, data starts at the fifth line.

					ifstream in;
					in.open(input_file);
					adjacency_list G = read_adjlist_from_edgelist_undirected(in, skip_lines);
					inplace_remove_duplicate_edges(G);

					std::sort(params.k_values.begin(), params.k_values.end()); // Run k-cores in order.
					for (size_t k : params.k_values) {
						G = kcore_undirected(G, k);
						G = largest_connected_component_undirected(G);
						bm.run(G, params.name + "-k-" + to_string(k), sample_size);
					}
				}
				if (file_output) {
					ofstream out;
					string dir_out = config::output_top_directory + "RWG/" + params.name + "/";
					create_directories(dir_out + "detailed/");
					out.open(dir_out + "detailed/" + algname + ".csv");
					bm.print_mode = bm.condensed_csv;
					bm.print_header(out);
					bm.print_results(out);
					out.close();
					out.open(dir_out + algname + ".csv");
					bm.print_mode = bm.condensed_summary_csv;
					bm.print_header(out);
					bm.print_results(out);
					out.close();
				}
			}
		}
	}

	void RFG_internal_stats() {
		using namespace std;
		using namespace filesystem;
		using namespace vertex_connectivity;

		size_t sample_size = 1;

		if (debug::localec_on == false) {
			cout << "Debugging is off: No internal stats." << endl;
			return;
		}

		if (!file_output) {
			cout << "No file output." << endl;
		}

		vector<string> algnames = {
			"LOCFhd3",
			"LOCALFd",
			"LOCALF",
			//"LOCALd",
			//"LOCALhd",
			//"LOCAL",
		};

		enum sort_by {
			L = 0,
			S = 1,
			R = 2,
			k = 3,
		};
		struct exp {
			string algname;
			sort_by s = R;
			string family = "RFG";
		};
		vector < exp > experiments = {
			//{ "k4-n10000",	},
			//{ "k8-n10000",	},
			//{ "k16-n10000",	},
			//{ "k32-n10000",	},

			//{ "k4-n100000", R, "RFG2" },
			//{ "k8-n100000", R, "RFG2" },
			//{ "k16-n100000", R, "RFG2" },
			//{ "k32-n100000", R, "RFG2" },
			{ "17+17", R, "RHG"},
		};

		for (const string& algname : algnames) {
			for (const auto& [exp_name, s, family] : experiments) {
				string dir_in = config::input_top_directory + family + "/" + exp_name + "/";
				vector<string> files;
				for (const directory_entry& file : directory_iterator(dir_in)) {
					if (file.is_regular_file())
						files.push_back(file.path().filename().string());
				}
				const sort_by& sb = s;
				sort(files.begin(), files.end(), [sb](string a, string b) {
					// Save duplicate identifier
					int i = stoi(a.substr(a.find("+") + 1));
					int j = stoi(b.substr(b.find("+") + 1));
					// Remove the duplicate identfier.
					a = a.substr(0, a.find("+"));
					b = b.substr(0, b.find("+"));
					switch (sb) {
					case k: // Remove everything before the first three "-"
						a = a.substr(a.find("-") + 1);
						b = b.substr(b.find("-") + 1);
					case R: // Remove everything before the first two "-"
						a = a.substr(a.find("-") + 1);
						b = b.substr(b.find("-") + 1);
					case S: // Remove everything before the first "-"
						a = a.substr(a.find("-") + 1);
						b = b.substr(b.find("-") + 1);
					case L: // Remove everything after the first "-"
						a = a.substr(0, a.find("-"));
						b = b.substr(0, b.find("-"));
					}
					int x = stoi(a);
					int y = stoi(b);
					return x < y || ((x == y) && (i < j)); // Sort as integers with duplicate id as tiebreaker.
				});

				vc_benchmarker bm(algname);

				string dir_out = config::debug_top_directory + family + "/" + exp_name + "/";
				//if (file_output) {
				//	create_directories(dir_out + "/detailed");
				//	ofstream out;
				//	out.open(dir_out + "detailed/" + algname + ".csv");
				//	out << "N,n,v,vertices_visited,edges_visited,vertices_visited_avg,edges_visited_avg,unique_vertices,unique_edges,unique_vertices_avg,unique_edges_avg" << endl;
				//	out.close();
				//	out.open(dir_out + algname + ".csv");
				//	out << "N,n,vertices_visited,edges_visited,vertices_visited_avg,edges_visited_avg,unique_vertices,unique_edges,unique_vertices_avg,unique_edges_avg" << endl;
				//}

				size_t vertices_visited = 0;
				size_t edges_visited = 0;
				size_t unique_vertices = 0;
				size_t unique_edges = 0;

				for (const string& file : files) {
					ifstream in;
					in.open(dir_in + file);
					adjacency_list G = read_adjlist_from_edgelist_undirected(in);
					debug::current_debug_id = file.substr(0, file.find("+"));
					bm.run(G, file, sample_size);
				}

				if (file_output) {
					vector<debug::localec_debug> dummy_output;
					vector<debug::localec_debug>& output =
						algname == "LOCAL" || algname == "LOCALF" ? debug::local_debug :
						(algname == "LOCALd" || algname == "LOCALFd" ? debug::localdeg_debug :
						(algname == "LOCALhd" || algname == "LOCFhd3" ? debug::localhdeg_debug :
						dummy_output));

					ofstream out, out2;
					string dir_out = config::debug_top_directory + family + "/" + exp_name + "/";
					create_directories(dir_out + "/detailed");
					out.open(dir_out + "detailed/" + algname + ".csv", ios::app);
					out << "N,n,v,vertices_visited,edges_visited,vertices_visited_avg,edges_visited_avg,unique_vertices,unique_edges,unique_vertices_avg,unique_edges_avg" << endl;
					size_t N = 0;
					string id;
					size_t n = 0;
					size_t v = 0;
					size_t vertices_visited = 0;
					size_t edges_visited = 0;
					size_t unique_vertices = 0;
					size_t unique_edges = 0;
					for (auto& line : output) { // Already sorted by n, then v.
						if (N != 0 && (id != line.get_id() || n != line.get_n() || v != line.get_v())) {
							out << N << "," << n << "," << v << ","
								<< vertices_visited << "," << edges_visited << ","
								<< ((double)vertices_visited / N) << "," << ((double)edges_visited / N) << ","
								<< unique_vertices << "," << unique_edges << ","
								<< ((double)unique_vertices / N) << "," << ((double)unique_edges / N) << endl;
							vertices_visited = 0;
							edges_visited = 0;
							unique_vertices = 0;
							unique_edges = 0;
							N = 0;
						}
						id = line.get_id();
						n = line.get_n();
						v = line.get_v();
						vertices_visited += line.get_vertices_visited();
						edges_visited += line.get_edges_visited();
						unique_vertices += line.get_unique_vertices();
						unique_edges += line.get_unique_edges();
						N++;
					}
					if (N != 0) {
						out << N << "," << n << "," << v << ","
							<< vertices_visited << "," << edges_visited << ","
							<< ((double)vertices_visited / N) << "," << ((double)edges_visited / N) << ","
							<< unique_vertices << "," << unique_edges << ","
							<< ((double)unique_vertices / N) << "," << ((double)unique_edges / N) << endl;
					}
					out.close();
					out.open(dir_out + algname + ".csv");
					out << "N,n,vertices_visited,edges_visited,vertices_visited_avg,edges_visited_avg,unique_vertices,unique_edges,unique_vertices_avg,unique_edges_avg" << endl;
					N = 0;
					vertices_visited = 0;
					edges_visited = 0;
					unique_vertices = 0;
					unique_edges = 0;
					for (auto& line : output) { // Already sorted by n, then v.
						if (N != 0 && id != line.get_id()) {
							out << N << "," << n << ","
								<< vertices_visited << "," << edges_visited << ","
								<< ((double)vertices_visited / N) << "," << ((double)edges_visited / N) << ","
								<< unique_vertices << "," << unique_edges << ","
								<< ((double)unique_vertices / N) << "," << ((double)unique_edges / N) << endl;
							vertices_visited = 0;
							edges_visited = 0;
							unique_vertices = 0;
							unique_edges = 0;
							N = 0;
						}
						id = line.get_id();
						n = line.get_n();
						vertices_visited += line.get_vertices_visited();
						edges_visited += line.get_edges_visited();
						unique_vertices += line.get_unique_vertices();
						unique_edges += line.get_unique_edges();
						N++;
					}
					if (N != 0) {
						out << N << "," << n << ","
							<< vertices_visited << "," << edges_visited << ","
							<< ((double)vertices_visited / N) << "," << ((double)edges_visited / N) << ","
							<< unique_vertices << "," << unique_edges << ","
							<< ((double)unique_vertices / N) << "," << ((double)unique_edges / N) << endl;
					}
					out.close();

					output.clear(); // Clear the vector in case another algorithm uses the same one.
				}
			}
		}
	}
}