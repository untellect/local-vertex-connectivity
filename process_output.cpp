#include "pch.h"

#include "mains.h"

#include "config.h"
#include "vc_benchmarker.h"

namespace mains {
	/*
	 * Postprocessing.
	 * Combine the files from different algorithm*and* graph duplicate outputs.
	 */
	void process_output() {
		using namespace std;
		using namespace filesystem;
		using namespace vertex_connectivity;

		struct res {
			struct sub_res {
				string alg = "";
				double seconds = 0.0;
				size_t success = 0;
			};

			size_t N = 0;
			size_t n = 0;
			size_t m = 0;
			size_t kappa = 0;

			string graph_id = "";
			string duplicate_id = "";

			size_t L = 0; // RFG only

			vector<sub_res> sres;
		};

		vector<string> graph_families = { "RFG", "RHG", "RWG" };
		unordered_set<string> included_algs = {
			//"LOCAL", "LOCALd", "LOCALhd",
			"LOCALF", "LOCALFd", "LOCFhd3",
			"HRG"
		};

		for (string graph_family : graph_families) {
			string family_dir = config::output_top_directory + graph_family + "/";

			vector<res> results2;

			bool first_experiment = true;
			for (const directory_entry& exp_dir : directory_iterator(family_dir)) {
				if (!exp_dir.is_directory()) continue;

				string experiment = exp_dir.path().filename().string();
				vector<res> results;

				// Iterate over algorithms in the order that the benchmarker would run them.
				vector<directory_entry> file_vec;
				for (const directory_entry& file : directory_iterator(exp_dir)) {
					if (file.is_regular_file())
						file_vec.push_back(file);
				}

				sort(file_vec.begin(), file_vec.end(), [](const directory_entry& a, const directory_entry& b) {
					string alg_a = a.path().filename().string();
					alg_a = alg_a.substr(0, alg_a.find(".")); // Remove ".csv"
					string alg_b = b.path().filename().string();
					alg_b = alg_b.substr(0, alg_b.find(".")); // Remove ".csv"

					return vc_benchmarker::algname_comp(alg_a, alg_b);
				});

				for (const directory_entry& file : file_vec) {
					if (!file.is_regular_file()) continue;



					string alg = file.path().filename().string();
					alg = alg.substr(0, alg.find(".")); // Remove ".csv"

					if (included_algs.count(alg) == 0) {
						continue; // Ignore this file.
					}

					ifstream in;
					in.open(file);
					in.ignore(10000, '\n'); // Skip header.
					string line;
					for (size_t i = 0; in && getline(in, line); i++) {
						size_t N = stoi(line.substr(0, line.find(",")));
						line = line.substr(line.find(",") + 1);
						size_t n = stoi(line.substr(0, line.find(",")));
						line = line.substr(line.find(",") + 1);
						size_t m = stoi(line.substr(0, line.find(",")));
						line = line.substr(line.find(",") + 1);
						size_t kappa = stoi(line.substr(0, line.find(",")));
						line = line.substr(line.find(",") + 1);
						double seconds = stod(line.substr(0, line.find(",")));
						line = line.substr(line.find(",") + 1);
						size_t success = stoi(line.substr(0, line.find(",")));
						line = line.substr(line.find(",") + 1);
						string graph_id = line.substr(0, line.find("+"));
						string duplicate_id = "";
						try {
							if (line.find("+") <= line.size())
								duplicate_id = line.substr(line.find("+"), line.find("."));
						}
						catch (...) {
							cout << "Something unexpected happened!" << endl;
						}

						if (results.size() <= i) {
							results.push_back({ N, n, m, kappa, graph_id, duplicate_id });
						}
						if (graph_family == "RFG") {
							// graph_id has format L-S-R-k
							results.back().L = stoi(graph_id.substr(0, graph_id.find("-")));
						}

						if (results[i].kappa != kappa) {
							cout << "Vertex connectivity mismatch." << endl;
							cout << "exp:\t" << experiment << endl;
							cout << "alg:\t" << alg << endl;
							cout << "i:\t" << i << endl;

							throw 0; // Prefer error over a local 0% success rate.
						}
						results[i].sres.push_back({ alg, seconds, success });
					}
				}
				if (results.empty()) continue;

				// Make sure algorithms are correctly aligned.
				for (res& r : results) {
					sort(r.sres.begin(), r.sres.end(), [](const res::sub_res& x, const res::sub_res& y) { return vc_benchmarker::algname_comp(x.alg, y.alg); });
				}

				ofstream out;

				if (graph_family == "RFG") {
					// Combine results by graph id / parameters.
					// Results are already correctly sorted.

					out.open(family_dir + experiment + "-grouped.csv");
					out << "L,N,n,m,kappa,";
					for (const res::sub_res& r : results.front().sres) {
						out << r.alg << "," + r.alg + "_success,";
					}
					out << "graph" << endl;

					string id = "";
					size_t duplicates = 0;
					res data;
					res data_success_only; size_t success_duplicates = 0;
					for (const res& r : results) {
						// Print results.
						if ((duplicates > 0) && (id != r.graph_id)) {
							out << (data.L / duplicates) << ",";
							out << data.N << ",";
							out << (data.n / duplicates) << ",";
							out << (data.m / duplicates) << ",";
							out << (data.kappa);
							for (auto& sr : data.sres) {
								out << "," << (sr.seconds / duplicates);
								out << "," << (sr.success);
							}
							out << "," << data.graph_id << endl;

							data = res(); // clear;
							duplicates = 0;
						}
						id = r.graph_id;

						// Collect results.
						data.L += r.L;
						data.N += r.N;
						data.n += r.n;
						data.m += r.m;
						data.kappa = r.kappa;
						data.graph_id = r.graph_id;
						for (size_t i = 0; i < r.sres.size(); i++) {
							if (data.sres.size() <= i) {
								data.sres.push_back({ r.sres.at(i).alg });
							}
							if (data.sres.at(i).alg != r.sres.at(i).alg) {
								cout << "Algorithm mismatch." << endl;
								cout << "Was: " << r.sres.at(i).alg << endl;
								cout << "Expected: " << data.sres.at(i).alg << endl;
								throw 0;
							}
							data.sres.at(i).seconds += r.sres.at(i).seconds;
							data.sres.at(i).success += r.sres.at(i).success;
						}
						duplicates++;
					}
					// Print the last data.
					if (duplicates > 0) {
						out << (data.L / duplicates) << ",";
						out << data.N << ",";
						out << (data.n / duplicates) << ",";
						out << (data.m / duplicates) << ",";
						out << (data.kappa);
						for (auto& sr : data.sres) {
							out << "," << (sr.seconds / duplicates);
							out << "," << (sr.success);
						}
						out << "," << data.graph_id << endl;
					}
				}
				else if (graph_family == "RHG") {
					//// Combine results by (size, vertex connectivity).
					//sort(results.begin(), results.end(), [](const res& r1, const res& r2) {
					//	return r1.n < r2.n || (r1.n == r2.n && r1.kappa < r2.kappa);
					//});

					//out.open(family_dir + experiment + "-grouped.csv");
					//out << "N,n,m,kappa";
					//for (const res::sub_res& r : results.front().sres) {
					//	out << "," << r.alg << "," + r.alg + "_success";
					//}
					//out << endl;
					//
					//size_t n = 0;
					//size_t kappa = 0;
					//size_t duplicates = 0;
					//res data;
					//for (const res& r : results) {
					//	// Print results.
					//	if (duplicates > 0 && (n != r.n || kappa != r.kappa)) {
					//		out << data.N << ",";
					//		out << (data.n / duplicates) << ",";
					//		out << (data.m / duplicates) << ",";
					//		out << (data.kappa);
					//		for (auto& sr : data.sres) {
					//			out << "," << (sr.seconds / duplicates);
					//			out << "," << (sr.success);
					//		}
					//		out << endl;

					//		data = res(); // clear;
					//		duplicates = 0;
					//	}
					//	n = r.n;
					//	kappa = r.kappa;

					//	// Collect results.
					//	data.N += r.N;
					//	data.n += r.n;
					//	data.m += r.m;
					//	data.kappa = r.kappa;
					//	for (size_t i = 0; i < r.sres.size(); i++) {
					//		if (data.sres.size() <= i) {
					//			data.sres.push_back({ r.sres.at(i).alg });
					//		}
					//		if (data.sres.at(i).alg != r.sres.at(i).alg) {
					//			cout << "Algorithm mismatch" << endl;
					//			cout << "Was: " << r.sres.at(i).alg << endl;
					//			cout << "Expected: " << data.sres.at(i).alg << endl;
					//			throw 0;
					//		}
					//		data.sres.at(i).seconds += r.sres.at(i).seconds;
					//		data.sres.at(i).success += r.sres.at(i).success;
					//	}
					//	duplicates++;
					//}
					//// Print the last data.
					//if (duplicates > 0) {
					//	out << data.N << ",";
					//	out << (data.n / duplicates) << ",";
					//	out << (data.m / duplicates) << ",";
					//	out << (data.kappa);
					//	for (auto& sr : data.sres) {
					//		out << "," << (sr.seconds / duplicates);
					//		out << "," << (sr.success);
					//	}
					//	out << endl;
					//}
				}
				else if (graph_family == "RWG") {

					string header;
					header += "N,n,m,kappa,delta";
					for (const res::sub_res& r : results.front().sres) {
						header += "," + r.alg + "," + r.alg + "perNms," + r.alg + "_success";
					}
					header += ",graph";

					if (first_experiment) {
						out.open(family_dir + "rwg.csv");
						out << header << endl;
					}
					else {
						out.open(family_dir + "rwg.csv", ios_base::app);
					}

					for (const res& r : results) {
						string line;
						line += to_string(r.N) + ",";
						line += to_string(r.n) + ",";
						line += to_string(r.m) + ",";
						line += to_string(r.kappa) + ",";
						line += r.graph_id.substr(r.graph_id.find_last_of("-") + 1) + ",";
						for (auto& sr : r.sres) {
							line += to_string(sr.seconds) + ",";
							line += to_string(sr.seconds / r.n * 1000) + ",";
							line += to_string(sr.success) + ",";
						}
						line += r.graph_id.substr(0, r.graph_id.find_last_of("-", r.graph_id.find_last_of("-")));

						// All collected
						out << line << endl;

						// Each separate
						ofstream out2;
						out2.open(family_dir + r.graph_id + ".csv");
						out2 << header << endl;
						out2 << line << endl;
						out2.close();
					}
				}
				first_experiment = false;
				if (graph_family == "RHG") {
					for (const res& r : results)
						results2.push_back(r);
				}
			}
			if (graph_family == "RHG") {
				// Combine results by vertex connectivity, then size.
				sort(results2.begin(), results2.end(), [](const res& r1, const res& r2) {
					return r1.kappa < r2.kappa || (r1.kappa == r2.kappa && r1.n < r2.n);
					});

				size_t n = 0;
				size_t kappa = 0;
				size_t duplicates = 0;
				res data;
				ofstream out;

				for (const res& r : results2) {
					// Print results.
					if (duplicates == 0 || kappa != r.kappa || n != r.n) {
						if (duplicates > 0) {
							out << data.N << ",";
							out << (data.n / duplicates) << ",";
							out << (data.m / duplicates) << ",";
							out << (data.kappa);
							for (auto& sr : data.sres) {
								out << "," << (sr.seconds / duplicates);
								out << "," << (sr.seconds / data.n); // * (duplicates/duplicates)
								out << "," << (sr.success);
							}
							out << endl;
						}

						if (duplicates == 0 || kappa != r.kappa) {
							out.close();
							out.open(family_dir + "k" + to_string(r.kappa) + ".csv");
							out << "N,n,m,kappa";
							for (const res::sub_res& r : results2.front().sres) {
								out << "," << r.alg << "," << r.alg << "perN" << "," << r.alg << "_success";
							}
							out << endl;
						}

						data = res(); // clear;
						duplicates = 0;
					}
					n = r.n;
					kappa = r.kappa;

					// Collect results.
					data.N += r.N;
					data.n += r.n;
					data.m += r.m;
					data.kappa = r.kappa;
					for (size_t i = 0; i < r.sres.size(); i++) {
						if (data.sres.size() <= i) {
							data.sres.push_back({ r.sres.at(i).alg });
						}
						if (data.sres.at(i).alg != r.sres.at(i).alg) {
							cout << "Algorithm mismatch" << endl;
							cout << "Was: " << r.sres.at(i).alg << endl;
							cout << "Expected: " << data.sres.at(i).alg << endl;
							throw 0;
						}
						data.sres.at(i).seconds += r.sres.at(i).seconds;
						data.sres.at(i).success += r.sres.at(i).success;
					}
					duplicates++;
				}
				// Print the last data.
				if (duplicates > 0) {
					out << data.N << ",";
					out << (data.n / duplicates) << ",";
					out << (data.m / duplicates) << ",";
					out << (data.kappa);
					for (auto& sr : data.sres) {
						out << "," << (sr.seconds / duplicates);
						out << "," << (sr.seconds / data.n); // * (duplicates/duplicates)
						out << "," << (sr.success);
					}
					out << endl;
				}
			}
		}
		cout << "Processed output" << endl;
	}
}
