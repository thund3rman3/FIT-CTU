#ifndef __PROGTEST__
#include <algorithm>
#include <array>
#include <bitset>
#include <cassert>
#include <cstdint>
#include <deque>
#include <iomanip>
#include <iostream>
#include <limits>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <queue>
#include <random>
#include <set>
#include <stack>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

enum Point : size_t {};

struct Path {
    Point from, to;
    unsigned length;

    Path(size_t f, size_t t, unsigned l) : from{f}, to{t}, length{l} {}

    friend bool operator == (const Path& a, const Path& b) {
        return std::tie(a.from, a.to, a.length) == std::tie(b.from, b.to, b.length);
    }

    friend bool operator != (const Path& a, const Path& b) { return !(a == b); }
};

#endif

std::vector<std::vector<std::pair<size_t, unsigned>>> loadGraph(size_t points, const std::vector<Path>& all_paths) {
    std::vector<std::vector<std::pair<size_t, unsigned >>> graph;
    graph.resize(points);
    for(size_t i = 0; i < all_paths.size(); i++) {
        graph[all_paths[i].from].push_back({all_paths[i].to, all_paths[i].length});
    }
    return graph;
}

std::vector<size_t> topsort(size_t points, const std::vector<Path>& all_paths) {
    std::vector<size_t> in_degree;
    in_degree.resize(points);
    for(size_t i = 0; i < all_paths.size(); i++) {
        in_degree[all_paths[i].to]++;
    }

    std::vector<size_t> in_edges;
    for(size_t i = 0; i < points; i++) {
        if (in_degree[i] == 0) {
            in_edges.push_back(i);
        }
    }
    return in_edges;
}

std::vector<Path> longest_track(size_t points, const std::vector<Path>& all_paths) {
    // vytvorim graf [from]{{to,len}, {to,len}, .....}
    std::vector<std::vector<std::pair<size_t, unsigned >>> graph = loadGraph(points, all_paths);

    // Zjisti vstupne hrany
    std::vector<size_t> in_edges = topsort(points, all_paths);

    std::vector<Path> result;
    size_t max_len = 0;

    for(size_t start_point = in_edges.size()-1; start_point > 0 ; start_point--){
        size_t current_point = in_edges[start_point];

        std::vector<size_t> prev;
        prev.resize(points);
        std::fill(prev.begin(), prev.end(),points);
        std::vector<size_t> distance;
        distance.resize(points);

        std::queue<size_t> toVisit;
        toVisit.push(current_point);

        while(!toVisit.empty()) {
            size_t current = toVisit.front();
            toVisit.pop();

            for(size_t neigh = 0 ; neigh < graph[current].size() ; neigh++) {
                size_t neighbor_edge = graph[current][neigh].first;
                size_t len = graph[current][neigh].second;
                // Vybiram nejdelsiho mozneho souseda podle delky aktualniho souseda
                size_t curr_len = distance[current] + len;
                if(curr_len > distance[neighbor_edge]) {
                    distance[neighbor_edge] = curr_len;
                    // Ulozim predchoziho souseda pro backtracking cesty
                    prev[neighbor_edge] = current;

                    if(neighbor_edge != points) {
                        toVisit.push(neighbor_edge);
                    }
                }
            }
        }

      // Index s nejvetsi hodnotou
      // https://en.cppreference.com/w/cpp/algorithm/max_element
      size_t end_point = std::max_element(distance.begin(), distance.end()) - distance.begin();

      if(distance[end_point] > max_len) {
          max_len = distance[end_point];
          result.clear();
            // end_point == points dosli jsme az na pocatecni vrchol
          while(end_point != points) {
              size_t prev_edge = prev[end_point];
              for(size_t i = 0; i < graph[prev_edge].size(); i++){
                  if (graph[prev_edge][i].first == end_point) {
                      result.insert(result.begin(), {prev_edge, end_point, graph[prev_edge][i].second});
                      break;
                  }
              }
              end_point = prev_edge;
          }
      }
    }
    return result;
}

#ifndef __PROGTEST__


struct Test {
    unsigned longest_track;
    size_t points;
    std::vector<Path> all_paths;
};

inline const Test TESTS[] = {
        {13, 5, { {3,2,10}, {3,0,9}, {0,2,3}, {2,4,1} } },
        {11, 5, { {3,2,10}, {3,1,4}, {1,2,3}, {2,4,1} } },
        {16, 8, { {3,2,10}, {3,1,1}, {1,2,3}, {1,4,15} } },
};

#define CHECK(cond, ...) do { \
    if (cond) break; \
    printf("Fail: " __VA_ARGS__); \
    printf("\n"); \
    return false; \
  } while (0)

bool run_test(const Test& t) {
    auto sol = longest_track(t.points, t.all_paths);

    unsigned length = 0;
    for (auto [ _, __, l ] : sol) length += l;

    CHECK(t.longest_track == length,
          "Wrong length: got %u but expected %u", length, t.longest_track);

    for (size_t i = 0; i < sol.size(); i++) {
        CHECK(std::count(t.all_paths.begin(), t.all_paths.end(), sol[i]),
              "Solution contains non-existent path: %zu -> %zu (%u)",
              sol[i].from, sol[i].to, sol[i].length);

        if (i > 0) CHECK(sol[i].from == sol[i-1].to,
                         "Paths are not consecutive: %zu != %zu", sol[i-1].to, sol[i].from);
    }

    return true;
}
#undef CHECK

int main() {
    int ok = 0, fail = 0;

    for (auto&& t : TESTS) (run_test(t) ? ok : fail)++;

    if (!fail) printf("Passed all %i tests!\n", ok);
    else printf("Failed %u of %u tests.\n", fail, fail + ok);
}

#endif


