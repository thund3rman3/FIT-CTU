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

struct STrack{
	STrack(size_t sfrom, size_t slength, size_t slenFromStart)
		:from(sfrom),length(slength), lenFromStart(slenFromStart){}
	size_t from;
	size_t length;
	size_t lenFromStart;
};

//points - pocet krizovatek
//allpaths - pouzitelne cesty mezi krizovatkami
std::vector<Path> longest_track(size_t points, const std::vector<Path>& all_paths)
{
	if(all_paths.empty())
		return {};

	//vrchol, predchudce, vzdalenost, vzdalenost od startu
	//std::map<size_t, std::pair<size_t, std::pair<size_t, size_t>>> track;
	std::vector<STrack> track(points, STrack(0,0,0));

	//[ffrom][] = (length, to)
	std::vector<std::vector<std::pair<size_t, size_t>>> allpaths_reworked(points);

	for(const auto & all_path : all_paths)
		allpaths_reworked[all_path.from].emplace_back(all_path.length, all_path.to);

	std::pair<size_t, size_t> last(0,0); // vertex, lenght from start
	std::queue<size_t > next;
	std::vector<bool > visited(points, false);
	std::vector<size_t> indegrees;
	indegrees.reserve(points);
	for(size_t i = 0; i < points; ++i)
		indegrees.push_back(0);
	for(const auto& path: all_paths)
		indegrees[path.to]++;

	for(size_t idx = 0; idx < points; idx++)
	{
		if(indegrees[idx] == 0)
		{
			next.emplace(idx);
			//visited.emplace(idx);
			visited[idx] = true;
			//track[idx] = std::make_pair(idx, std::make_pair(0,0));
			track[idx].from = idx;
		}
	}

	while(!next.empty())
	{
		auto elem = next.front();
		next.pop();
		size_t lenFromStart = track[elem].lenFromStart; // vzdalenost od startu

		for(size_t i = 0; i < allpaths_reworked[elem].size(); ++i)// rework .to
		{
			size_t to = allpaths_reworked[elem][i].second;
			size_t lenToNB = allpaths_reworked[elem][i].first;
			if(!visited[to])
			{
				next.emplace(to);
				visited[to] = true;
				//track[to] = std::make_pair(elem, std::make_pair(lenToNB, lenToNB + lenFromStart));
				//track[to] = STrack(elem, lenToNB, lenToNB + lenFromStart);
				track[to].from = elem;
				track[to].lenFromStart = lenToNB + lenFromStart;
				track[to].length = lenToNB;
			}
			else if( lenFromStart + lenToNB > track[to].lenFromStart)
			{
				next.emplace(to);
				//track[to] = std::make_pair(elem, std::make_pair(lenToNB, lenToNB + lenFromStart));
				//track[to] = STrack(elem, lenToNB, lenToNB + lenFromStart);
				track[to].from = elem;
				track[to].lenFromStart = lenToNB + lenFromStart;
				track[to].length = lenToNB;
			}
		}

		if(lenFromStart > last.second)
			last = std::make_pair(elem, lenFromStart);
	}

	std::vector<Path> res;
	size_t curr = last.first;
	while(curr!=track[curr].from)
	{
		res.emplace_back(track[curr].from, curr, track[curr].length);
		curr = track[curr].from;
	}
	std::reverse(res.begin(),res.end());
	return res;
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

	size_t pointss = 8;
	std::vector<Path> paths{{0,2,1}, {1,2,2}, {1,3,3}, {1, 4, 4},
							{2,5,5}, {3,5,6}, {3,7,9}, {4,3,7},
							{4,6,10}, {5,7,8}, {6,7,1}};
	std::vector<Path> out = longest_track(pointss, paths);
	std::vector<Path> pathOut{{1,4,4}, {4,3,7}, {3,5,6},{5,7,8}};
	assert(out==pathOut);

	std::vector<Path> path0{{0,2,0}, {1,2,0}, {1,3,0}, {1, 4, 1},
							{2,5,0}, {3,5,0}, {3,7,0}, {4,3,0},
							{4,6,0}, {5,7,1}, {6,7,0}};
	std::vector<Path> out0 = longest_track(pointss, path0);
	std::vector<Path> res0{{1,4,1},{4,3,0},{3,5,0}, {5,7,1}};
	assert(out0 == res0);
}

#endif