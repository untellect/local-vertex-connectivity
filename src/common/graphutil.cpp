#include "util.h"
#include "random.h"

namespace vertex_connectivity {
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

    vertex_set get_reachable(const adjacency_list& G, vertex x) {
        std::vector<bool> is_visited(G.size(), false);
        vertex_set visited;

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

    
    vertex_set get_reachable(const adjacency_list& G, const vertex_vec& S) {
        std::vector<bool> is_visited(G.size(), false);
        vertex_set visited;

        for(vertex x : S) {
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
        }

        return visited;
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

    vertex_set get_unsplit_cut_from_reachable_set(const vertex_set& L_split, const adjacency_list& G, const std::vector<vertex>& sources) {
        vertex_set output;
        // Invertices in L whose outvertex is not in L are cut vertices.
        for (const vertex& v : L_split) {
            if (v & 1) continue; // outvertex

            if (L_split.find(v + 1) == L_split.end()) {
                output.insert(v / 2);
            }
        }
        // Vertices adjacent to the source whose invertices are not in L are cut vertices.
        for(vertex v : sources) {
            for (const vertex& v : G[v]) {
                if (L_split.find(2 * v) == L_split.end()) {
                    output.insert(v);
                }
            }
        }

        return output;
    }

    std::vector<vertex_set> get_connected_components_after_cut(const adjacency_list& G, const vertex_set& S) {
        std::vector<vertex_set> comps;
        size_t n = G.size();
        std::vector<bool> visited(n, false);
        for(vertex v : S) {
            visited[v] = true; // Do not include in any connected components, nor pass through.
        }
        for(vertex x = 0; x < n; x++) {
            if(visited[x]) continue;
            visited[x] = true;
            vertex_set C;
            vertex_vec Q = { x };
            while (!Q.empty()) {
                vertex v = Q.back(); Q.pop_back();
                C.insert(v);
                for (vertex w : G[v]) {
                    if (visited[w]) continue;
                    visited[w] = true;
                    Q.push_back(w);
                }
            }
            comps.push_back(C);
        }
        return comps;
    }

    bool is_valid_vertex_cut(const adjacency_list& G, const vertex_set& S) {
        size_t n = G.size();
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

    adjacency_list reverse_graph(const adjacency_list& G) {
        size_t n = G.size();
        adjacency_list RG(n);
        for(vertex x = 0; x < n; x++) {
            for(vertex y : G[x])
            RG[y].push_back(x);
        }
        return RG;
    }
    adjacency_list split_graph(const adjacency_list& G) {
        adjacency_list SG;
        vertex s = 0;
        for (const std::vector<vertex>& vec : G) {
            // Invertex
            SG.push_back({s+1});
            ++s;
            // Outvertex
            SG.push_back({});
            for (const vertex& t : vec) {
                SG.back().push_back(2 * t);
            }
            ++s;
        }
        return SG;
    }
}
