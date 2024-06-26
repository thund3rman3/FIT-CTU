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

    // add connection from -> to, fluent interface
    CDelivery &addConn(const string &from, const string &to);

    // returns a map containing the shortest path to each customer from any of the depots
    // map key is the customer name, value is a list of nodes on the shortest path (depot -> customer)
    // if there is no path, the value is an empty list
    map<string, list<string>> serveCustomers(const set<string> &customers, const set<string> &depots) const;

private:
    set<pair<string, string>> m_paths;
};

CDelivery &CDelivery::addConn(const string &from, const string &to) {
    m_paths.emplace(to, from);
    return *this;
}

map<string, list<string>> CDelivery::serveCustomers(const set<string> &customers, const set<string> &depots) const {

    map<string, list<string>> res;
    for (const string &cus: customers) {
        list<string> way_to_cus;
        map<string, pair<int, string>> visited; // place, distance, place before
        queue<string> next;
        visited.emplace(cus, make_pair(0, ""));
        next.push(cus);
        while (!next.empty()) {
            string elem = next.front();
            next.pop();
            for (auto itt = m_paths.lower_bound(make_pair(elem, ""));
                 itt != m_paths.end() && itt->first == elem; ++itt) {
                if (visited.find(itt->second) == visited.end()) {
                    visited.emplace(itt->second, make_pair(visited[elem].first + 1, elem));
                    next.push(itt->second);
                }
            }
        }

        string search;
        int min = INT32_MAX;
        for (const string &str: depots) {
            if (visited.find(str) != visited.end() && visited[str].first < min) {
                search = str;
                min = visited[str].first;
            }
        }
        if (!search.empty()) {

            way_to_cus.push_back(search);
            while (!visited[search].second.empty()) {
                search = visited[search].second;
                way_to_cus.push_back(search);
            }
        }
        //reverse(way_to_cus.begin(),way_to_cus.end());
        res[cus] = way_to_cus;
    }
    return res;
}

int main() {
    CDelivery t0;
    map<string, list<string>> r;
    t0.addConn("Austin", "Berlin");
    t0.addConn("Chicago", "Berlin");
    t0.addConn("Berlin", "Dallas");
    t0.addConn("Dallas", "Essen");
    t0.addConn("Essen", "Austin");
    t0.addConn("Frankfurt", "Gyor");
    t0.addConn("Gyor", "Helsinki");
    t0.addConn("Helsinki", "Frankfurt");

    r = t0.serveCustomers(set<string>{"Berlin", "Gyor"}, set<string>{"Essen", "Helsinki"});
    assert (r == (map<string, list<string>>{{"Berlin", {"Essen",    "Austin",    "Berlin"}},
                                            {"Gyor",   {"Helsinki", "Frankfurt", "Gyor"}}}));

    r = t0.serveCustomers(set<string>{"Austin", "Gyor", "Chicago"}, set<string>{"Austin", "Dallas"});
    assert (r == (map<string, list<string>>{{"Austin",  {"Austin"}},
                                            {"Chicago", {}},
                                            {"Gyor",    {}}}));
    t0.addConn("Chicago", "Helsinki");

    r = t0.serveCustomers(set<string>{"Austin", "Gyor", "Chicago"}, set<string>{"Austin", "Dallas"});
    assert (r == (map<string, list<string>>{{"Austin",  {"Austin"}},
                                            {"Chicago", {}},
                                            {"Gyor",    {}}}));
    t0.addConn("Berlin", "Chicago");

    r = t0.serveCustomers(set<string>{"Austin", "Gyor", "Chicago"}, set<string>{"Austin", "Dallas"});
    assert (r == (map<string, list<string>>{{"Austin",  {"Austin"}},
                                            {"Chicago", {"Austin", "Berlin", "Chicago"}},
                                            {"Gyor",    {"Austin", "Berlin", "Chicago", "Helsinki", "Frankfurt", "Gyor"}}}));
    t0.addConn("Essen", "Frankfurt");

    r = t0.serveCustomers(set<string>{"Austin", "Gyor", "Chicago"}, set<string>{"Austin", "Dallas"});
    assert (r == (map<string, list<string>>{{"Austin",  {"Austin"}},
                                            {"Chicago", {"Austin", "Berlin", "Chicago"}},
                                            {"Gyor",    {"Dallas", "Essen",  "Frankfurt", "Gyor"}}}));

    return EXIT_SUCCESS;
}