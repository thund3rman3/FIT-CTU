#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <stack>
#include <queue>
#include <deque>
#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <algorithm>
#include <memory>
#include <cassert>

using namespace std;

class CDelivery {
public:
    // ctor
    CDelivery() = default;

    // dtor
    ~CDelivery() = default;

    CDelivery &addConn(const string &a, const string &b);

    vector<int> findCoverage(const set<string> &depots) const;

private:
    set<pair<string, string>> m_paths;
    set<string> m_warehouses;
};

CDelivery &CDelivery::addConn(const string &a, const string &b) {
    m_paths.emplace(a, b);
    m_paths.emplace(b, a);
    m_warehouses.emplace(a);
    m_warehouses.emplace(b);
    return *this;
}

// index res jen max vzdalenost vsechn moznych skladu a na nem je ulozeny kolik skladu je v te vzdalenosti
vector<int> CDelivery::findCoverage(const set<string> &depots) const {
    vector<int> res;
    map<string, int> visited;
    queue<string> next_visit;
    for (const auto &x: depots) {
        if (m_warehouses.find(x) == m_warehouses.end())
            throw invalid_argument("");
        visited.emplace(x, 0);
        next_visit.push(x);
    }

    size_t idx = 0;
    int cnt = (int) depots.size();
    res.push_back(cnt);

    while (!next_visit.empty()) {
        auto elem = next_visit.front();
        next_visit.pop();
        auto it = m_paths.lower_bound(make_pair(elem, ""));
        for (; it->first == elem && it != m_paths.end(); ++it) {
            if (visited.find(it->second) == visited.end()) {
                visited.emplace(it->second, visited[it->first] + 1);
                next_visit.push(it->second);
                if (res.size() - 1 < visited[it->first] + 1)
                    res.push_back(1);
                else
                    res[visited[it->first] + 1]++;
            }
        }
    }
    int prev = res[0];
    for (size_t i = 1; i < res.size(); ++i) {
        res[i] += prev;
        prev = res[i];
    }

    return res;
}

int main() {
    CDelivery t0;
    vector<int> r;
    t0
            .addConn("Austin", "Berlin")
            .addConn("Chicago", "Berlin")
            .addConn("Chicago", "Dallas")
            .addConn("Dallas", "Essen")
            .addConn("Essen", "Austin")
            .addConn("Frankfurt", "Essen")
            .addConn("Gyor", "Frankfurt")
            .addConn("Helsinki", "Istanbul")
            .addConn("Istanbul", "Jakarta");

    r = t0.findCoverage(set<string>{"Berlin"});
    assert (r == (vector<int>{1, 3, 5, 6, 7}));
    /* result:
     * [0]: Berlin = 1
     * [1]: Austin, Berlin, Chicago = 3
     * [2]: Austin, Berlin, Chicago, Dallas, Essen = 5
     * [3]: Austin, Berlin, Chicago, Dallas, Essen, Frankfurt = 6
     * [4]: Austin, Berlin, Chicago, Dallas, Essen, Frankfurt, Gyor = 7
     */

    r = t0.findCoverage(set<string>{"Berlin", "Essen"});
    assert (r == (vector<int>{2, 6, 7}));

    r = t0.findCoverage(set<string>{"Helsinki"});
    assert (r == (vector<int>{1, 2, 3}));

    r = t0.findCoverage(set<string>{"Istanbul"});
    assert (r == (vector<int>{1, 3}));

    r = t0.findCoverage(set<string>{"Austin", "Jakarta"});
    assert (r == (vector<int>{2, 5, 9, 10}));

    r = t0.findCoverage(set<string>{"Chicago", "Gyor", "Helsinki", "Jakarta"});
    assert (r == (vector<int>{4, 8, 10}));

    try {
        r = t0.findCoverage(set<string>{"Incorrect city"});
        assert ("No invalid_argument exception caught!" == nullptr);
    }
    catch (const invalid_argument &e) {}

    return EXIT_SUCCESS;
}
