#ifndef __PROGTEST__

#include <algorithm>
#include <assert.h>
#include <cassert>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

class CPos {
public:
    CPos(int y, int x, int hour)
            : m_hour(hour), m_posY(y), m_posX(x) {
    }

    int m_hour;
    int m_posY;
    int m_posX;
};

#endif /* __PROGTEST__ */

class CBattleRoyale {
public:
    CBattleRoyale(unsigned height, unsigned width, const std::list<CPos> &zones)
            : m_height(height), m_width(width) {
        for (const auto &x: zones) {
            m_zones.emplace(make_pair(x.m_posX, x.m_posY), x.m_hour);
        }
    }

    // des (if needed)

    bool out_of_range_c(int ySt, int xSt) const {
        return ySt < 0 || ySt >= m_height || xSt < 0 || xSt >= m_width;
    }

    unsigned findRoute(int ySt, int xSt, int yEn, int xEn) const {
        if (out_of_range_c(ySt, xSt) || out_of_range_c(yEn, xEn))
            throw invalid_argument("hehe");
        auto it = m_zones.find({xSt, ySt});
        if (it != m_zones.end() && it->second == 0)
            throw invalid_argument("hihi");

        map<pair<int, int>, int> visited;
        queue<pair<int, int>> next;
        visited.emplace(make_pair(xSt, ySt), 0);
        next.emplace(xSt, ySt);

        int xes[4] = {1, 0, -1, 0};
        int yes[4] = {0, -1, 0, 1};
        while (!next.empty()) {
            auto elem = next.front();
            next.pop();

            for (int i = 0; i < 4; i++) {
                int x = elem.first + xes[i];
                int y = elem.second + yes[i];
                if (!out_of_range_c(x, y) && visited.find({x, y}) == visited.end()) {
                    auto it = m_zones.find({x, y});
                    if ((it != m_zones.end() && it->second > 1 + visited[elem]) ||
                        m_zones.end() == m_zones.find({x, y})) {
                        visited.emplace(make_pair(x, y), visited[elem] + 1);
                        next.emplace(x, y);
                    }
                }
            }
        }
        if (visited.count({xEn, yEn}) == 0)
            throw invalid_argument("hoho");
        return visited[{xEn, yEn}];
    }

private:
    map<pair<int, int>, int> m_zones;
    unsigned m_height, m_width;
};

#ifndef __PROGTEST__

int main() {
    CBattleRoyale r1(5, 5, {});
    assert(r1.findRoute(0, 0, 4, 0) == 4);
    assert(r1.findRoute(4, 4, 4, 4) == 0);

    CBattleRoyale r2(6, 7, {CPos(1, 0, 1), CPos(2, 1, 2), CPos(3, 2, 5)});
    assert(r2.findRoute(0, 0, 4, 0) == 10);


    CBattleRoyale r3(8, 8, {CPos(0, 2, 1), CPos(3, 1, 2), CPos(2, 1, 0)});
    try {
        r3.findRoute(2, 1, 4, 7);
        assert("Exception missing!" == nullptr);
    } catch (const logic_error &e) {
    } catch (...) {
        assert("Invalid exception thrown!" == nullptr);
    }
    assert(r3.findRoute(0, 2, 3, 0) == 5);

    CBattleRoyale b(5, 5, {CPos(0, 1, 1), CPos(1, 1, 0)});
    assert(b.findRoute(0, 0, 2, 2) == 4);
    assert(b.findRoute(0, 0, 0, 2) == 6);
    assert(b.findRoute(3, 3, 3, 3) == 0);
    try {
        assert(b.findRoute(1, 1, 2, 1) == 1);
        assert("Kde vyjimka?" == nullptr);
    } catch (logic_error &x) {}
    try {
        assert(b.findRoute(1, 1, 1, 1) == 0);
        assert("Kde vyjimka? xd" == nullptr);
    } catch (logic_error &x) {}

    CBattleRoyale b1(5, 5, {CPos(2, 0, 2), CPos(2, 1, 1), CPos(2, 2, 1), CPos(2, 3, 3), CPos(2, 4, 4)});
    try {
        b1.findRoute(1, 1, 3, 1);
        assert("Kde vyjimka?" == nullptr);
    } catch (logic_error &x) {}

}

#endif /* __PROGTEST__ */
