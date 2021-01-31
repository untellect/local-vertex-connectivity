#include "pch.h"

#include "util.h"
#include "random.h"

namespace util {
    union_find::union_find(size_t n) : parent(n), rank(n, 0), distinct_sets(n) {
        std::iota(parent.begin(), parent.end(), 0);
    }

	// Using "path halving"
	size_t union_find::find(size_t x) {
		while (parent[x] != x) {
			parent[x] = parent[parent[x]];
			x = parent[x];
		}
		return x;
	}

	// Using "rank"
	void union_find::merge(size_t x, size_t y) {
		x = find(x);
		y = find(y);

		if (x == y) return;

        distinct_sets--;

        if (rank[x] < rank[y])
            std::swap(x, y);
        
        parent[y] = x;
		
		if (rank[x] == rank[y])
			rank[x]++;
	}

    size_t union_find::count_distinct_sets() const {
        return distinct_sets;
    }

    multi_union_find::multi_union_find(size_t n, size_t k) : n(n), k(k) {
        for (size_t i = 0; i < k; i++) {
            data.emplace_back(n);
        }
    }

	size_t multi_union_find::lowest_not_same(size_t x, size_t y) {
		size_t low = lowest_nontrivial;
		size_t high = k; // maximum + 1
		while (low != high) {
			size_t mid = (high + low) / 2;
			bool same = data[mid].find(x) == data[mid].find(y);
			if (same) {
				low = mid + 1;
			}
			else {
				high = mid;
			}
		}
		return low;
	}

	void multi_union_find::merge(size_t x, size_t y, size_t k) {
		data[k].merge(x, y);
        if (data[k].count_distinct_sets() == 1)
            trivialise(k);
	}

    void multi_union_find::trivialise(size_t i) {
        lowest_nontrivial = std::max(lowest_nontrivial, i + 1);
    }

    bool multi_union_find::is_nontrivial() const {
        return lowest_nontrivial != k;
    }
}

namespace graph {
    vertex_set get_neighbors(const adjacency_list& G, const vertex_set& S) {
        vertex_set N;
        for (vertex s : S) {
            for (vertex t : G[s]) {
                if (S.find(t) == S.end()) {
                    N.insert(t);
                }
            }
        }
        return N;
    }
    vertex_set get_neighbors(const resettable_graph& G, const vertex_set& S) {
        return get_neighbors(G._data(), S);
    }

    vertex_set get_reachable(const adjacency_list& G, vertex x) {
        std::vector<bool> is_visited(G.size(), false);
        std::unordered_set<vertex> visited;

        is_visited[x] = true;
        visited.insert(x);
        std::vector<vertex> Q = { x };
        while (!Q.empty()) {
            vertex v = Q.back(); Q.pop_back();
            for (vertex w : G[v]) {
                if (is_visited[w]) continue;
                is_visited[w] = true;
                visited.insert(w);
                Q.push_back(w);
            }
        }

        return visited;
    }
    vertex_set get_reachable(const resettable_graph& G, vertex x) {
        return get_reachable(G._data(), x);
    }

    vertex_set get_unsplit(const vertex_set& S_split) {
        vertex_set S;
        for (const vertex& v : S_split)
            S.insert(v / 2);
        return S;
    }

    vertex_set get_unsplit_cut_from_reachable_set(const vertex_set& L_split, const std::vector<vertex>& adjacent_to_source_unsplit) {
        vertex_set output;
        // Invertices in L whose outvertex is not in L are cut vertices.
        for (const vertex& v : L_split) {
            if (v & 1) continue; // outvertex

            if (L_split.find(v + 1) == L_split.end()) {
                output.insert(v / 2);
            }
        }
        // Vertices adjacent to the source whose invertices are not in L are cut vertices.
        for (const vertex& v : adjacent_to_source_unsplit) {
            if (L_split.find(2 * v) == L_split.end()) {
                output.insert(v);
            }
        }

        return output;
    }

    bool is_valid_vertex_cut(const adjacency_list& G, const vertex_set& S) {
        size_t n = G.size();
        size_t k = S.size();
        vertex x = -1;
        for (vertex v = 0; v < n; v++) {
            if (S.find(v) == S.end()) {
                x = v;
                break;
            }
        }

        std::vector<bool> visited(n, false);
        std::vector<vertex> Q = { x };
        visited[x] = true;
        while (!Q.empty()) {
            vertex v = Q.back(); Q.pop_back();
            for (vertex w : G[v]) {
                if (visited[w]) continue;
                visited[w] = true;
                if (S.find(w) != S.end()) continue; // Do not pass through cut vertices.
                Q.push_back(w);
            }
        }
        
        for (bool v_is_visited : visited) {
            if (!v_is_visited)
                return true; // Exists a vertex that is unreachable from x without passing a cut vertex.
        }
        return false;
    }

    edge_sampler::edge_sampler(const resettable_graph& G) {
        for (vertex s = 0; s < G.size(); s++) {
            for(vertex t : G[s]) {
                data.push_back({ s, t });
            }
        }
    }
    edge_sampler::edge_sampler(const adjacency_list& G) {
        for (vertex s = 0; s < G.size(); s++) {
            for (vertex t : G[s]) {
                data.push_back({ s, t });
            }
        }
    }
    vertex edge_sampler::get_source() {
        if (i == data.size()) reset(); // Restart if all edges have been sampled.
        size_t r = util::random_int(i, data.size() - 1); // Sample one of the unsampled indices.
        std::swap(data[i], data[r]); // Move that edge to the front of the unsampled interval.
        return data[i++].x; // Return that vertex and increment i to make it part of the sampled part of 'data' vector
    }
    vertex edge_sampler::get_target() {
        if (i == data.size()) reset(); // Restart if all edges have been sampled.
        size_t r = util::random_int(i, data.size() - 1); // Sample one of the unsampled indices.
        std::swap(data[i], data[r]); // Move that edge to the front of the unsampled interval.
        return data[i++].y; // Return that vertex and increment i to make it part of the sampled part of 'data' vector
    }
    size_t edge_sampler::count_edges() {
        return data.size();
    }
    void edge_sampler::reset() {
        i = 0;
    }
}
