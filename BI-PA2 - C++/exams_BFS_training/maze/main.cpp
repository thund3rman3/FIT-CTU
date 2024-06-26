#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <queue>
#include <stack>
#include <list>
#include <cassert>
#include <algorithm>

using namespace std;
using point = pair<int, int>;
using mazeMap = vector<vector<int>>;

class Maze {
public:
    Maze() = default;

    ~Maze() = default;

    void loadMaze(const vector<vector<int>> &maze);

    list<point> findMaze(const pair<int, int> &start, const pair<int, int> &end) const;

private:
    map<point, int> m_maze;
};

void Maze::loadMaze(const vector<vector<int>> &maze) {

    for (size_t i = 0; i < maze.size(); ++i) {
        for (size_t j = 0; j < maze[i].size(); ++j) {
            m_maze.emplace(make_pair(i, j), maze[i][j]);
        }
    }
}

list<point> Maze::findMaze(const pair<int, int> &start, const pair<int, int> &end) const {
    auto startit = m_maze.find(start);
    auto endit = m_maze.find(end);
    if (startit == m_maze.end() || endit == m_maze.end())
        throw out_of_range("");
    if (startit->second != 1 || endit->second != 1)
        throw logic_error("");
    queue<point> nextvisit;
    map<point, point> visited;
    nextvisit.push(start);
    visited.emplace(start, make_pair(-1, -1));
    list<point> res;
    int row[4] = {-1, 0, 1, 0};
    int col[4] = {0, 1, 0, -1};
    while (!nextvisit.empty()) {
        auto elem = nextvisit.front();
        nextvisit.pop();
        if (elem == end)
            break;
        for (int i = 0; i < 4; ++i) {
            point curr = make_pair(elem.first + row[i], elem.second + col[i]);
            auto it = m_maze.find(curr);
            if (it != m_maze.end() && it->second == 1 && visited.find(curr) == visited.end()) {
                visited.emplace(curr, elem);
                nextvisit.push(curr);
            }
        }
    }
    point endpoint = end;
    if (visited.find(endpoint) == visited.end())
        throw out_of_range("");
    while (endpoint != make_pair(-1, -1)) {
        res.push_back(endpoint);
        endpoint = visited[endpoint];
    }
    reverse(res.begin(), res.end());
    return res;
}

int main() {

    Maze m;

    mazeMap m1 = {{1, 1, 1, 1},
                  {1, 0, 1, 0},
                  {1, 1, 1, 1}};
    point start = {0, 0};
    point end = {0, 3};
    m.loadMaze(m1);
    auto res = m.findMaze(start, end);
    assert(res == list<point>({{0, 0},
                               {0, 1},
                               {0, 2},
                               {0, 3}}));

    Maze m2;
    mazeMap m2m = {{1, 1, 0, 0},
                   {0, 1, 1, 1},
                   {0, 1, 0, 1}};
    start = {0, 0};
    end = {2, 3};
    m2.loadMaze(m2m);
    res = m2.findMaze(start, end);
    assert(res == list<point>({{0, 0},
                               {0, 1},
                               {1, 1},
                               {1, 2},
                               {1, 3},
                               {2, 3}}));

    Maze m3;

    mazeMap m3m = {
            {1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0},
            {1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 1},
            {0, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1},
            {1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1},
            {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1},
            {1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1},
            {1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1}
    };

    start = {0, 1};
    end = {6, 13};

    m3.loadMaze(m3m);
    res = m3.findMaze(start, end);
    assert(res == list<point>({{0, 1},
                               {0, 2},
                               {1, 2},
                               {2, 2},
                               {3, 2},
                               {4, 2},
                               {5, 2},
                               {6, 2},
                               {6, 3},
                               {6, 4},
                               {7, 4},
                               {8, 4},
                               {8, 5},
                               {8, 6},
                               {7, 6},
                               {6, 6},
                               {6, 7},
                               {6, 8},
                               {7, 8},
                               {8, 8},
                               {8, 9},
                               {8, 10},
                               {8, 11},
                               {8, 12},
                               {8, 13},
                               {7, 13},
                               {6, 13}}));

    start = {20, 20};
    end = {0, 0};
    try {
        res = m3.findMaze(start, end);
        assert(false);
    } catch (std::out_of_range &e) {
        assert(true);
    }

    start = {0, 0};
    end = {0, 12};
    try {
        res = m3.findMaze(start, end);
        assert(false);
    }
    catch (std::logic_error &er) {
        assert(true);
    }

    start = {0, 0};
    end = {0, 13};

    try {
        res = m3.findMaze(start, end);
        assert(false);
    }
    catch (std::out_of_range &er) {
        assert(true);
    }

    return 0;
}