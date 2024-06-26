#ifndef __PROGTEST__

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include <list>
#include <vector>
#include <stack>
#include <queue>
#include <deque>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <algorithm>
#include <memory>

using namespace std;

class Cbase {
public:
    Cbase(string sector, string species, int when) : m_Sector(std::move(sector)), m_Species(std::move(species)),
                                                     m_When(when) {}

    ~Cbase() = default;

    string m_Sector;
    string m_Species;
    int m_When;
};

struct CVisitParam {
    string rasa;
    int cas_obsazeni = 0;
    bool kolonizovana_jednou = true;
};

struct CNextParam {
    string planeta;
    string rasa;
    int aktualni_cas = 0;
    int start_kolonizace = 0;
};

#endif /* __PROGTEST__ */

class CUniverse {
public:

    CUniverse &addConnection(const string &from, const string &to) {
        m_wormholes.emplace(from, to);
        m_wormholes.emplace(to, from);
        return *this;
    }

    CUniverse &optimize() {
        return *this;
    }

    map<string, string> colonise(const vector<Cbase> &bases) const {
        //planeta, rasa
        map<string, string> res;
        queue<Cbase> nextvisit;
        set<string> visited;
        map<string, CVisitParam> tmp;

        for (const auto &x: bases) {
            if (m_wormholes.lower_bound(make_pair(x.m_Sector, "")) == m_wormholes.end())
                res.emplace(x.m_Sector, x.m_Species);
            else {
                nextvisit.emplace(x.m_Sector, x.m_Species, x.m_When);
                tmp.emplace(x.m_Sector, CVisitParam{x.m_Species, x.m_When, true});
            }
        }
        while (!nextvisit.empty()) {
            auto elem = nextvisit.front();
            nextvisit.pop();

            if (tmp.find(elem.m_Sector) == tmp.end()) {
                tmp.emplace(elem.m_Sector, CVisitParam{elem.m_Species, elem.m_When + 1, true});
            } else {
                if (elem.m_When + 1 == tmp[elem.m_Sector].cas_obsazeni &&
                    elem.m_Species != tmp[elem.m_Sector].rasa)
                    tmp[elem.m_Sector].kolonizovana_jednou = false;
                else if (elem.m_When + 1 < tmp[elem.m_Sector].cas_obsazeni &&
                         elem.m_Species != tmp[elem.m_Sector].rasa)
                    tmp[elem.m_Sector] = {CVisitParam{elem.m_Species, elem.m_When + 1, true}};
            }

            for (auto it = m_wormholes.lower_bound(make_pair(elem.m_Sector, ""));
                 it != m_wormholes.end() && it->first == elem.m_Sector; ++it) {
                if (visited.find(it->second) == visited.end()) {
                    // visited.emplace(it->second);
                    nextvisit.emplace(it->second, elem.m_Species, elem.m_When + 1);
                }
            }

            visited.emplace(elem.m_Sector);

        }
        for (const auto &x: tmp) {
            if (x.second.kolonizovana_jednou)
                res[x.first] = x.second.rasa;
        }
//        //planeta - rasa, cas obasazeni - kolonizovana 1x
//        map<string, CVisitParam> visited;
//        //planeta, rasa - aktualni cas, cas zacatku kolonizace
//        queue<CNextParam> nextvisit;
//
//        for (const auto &x: bases) {
//            if (m_wormholes.lower_bound(make_pair(x.m_Sector, "")) == m_wormholes.end())
//                res.emplace(x.m_Sector, x.m_Species);
//            else {
//                visited.emplace(x.m_Sector, CVisitParam{x.m_Species, 0, true});
//                nextvisit.push(CNextParam{x.m_Sector, x.m_Species, 0, x.m_When});
//            }
//        }
//
//        while (!nextvisit.empty()) {
//            auto elem = nextvisit.front();
//            nextvisit.pop();
//            if (!visited[elem.planeta].kolonizovana_jednou)
//                continue;
//            if (elem.aktualni_cas < elem.start_kolonizace) {
//                nextvisit.push(CNextParam{elem.planeta, elem.rasa, elem.aktualni_cas + 1, elem.start_kolonizace});
//                continue;
//            }
//
//            for (auto it = m_wormholes.lower_bound(make_pair(elem.planeta, ""));
//                 it != m_wormholes.end() && it->first == elem.planeta; ++it) {
//                auto it2 = visited.find(it->second);
//                if (it2 == visited.end()) {
//                    nextvisit.push(CNextParam{it->second, elem.rasa, elem.aktualni_cas + 1, elem.start_kolonizace});
//                    visited.emplace(it->second, CVisitParam{elem.rasa, elem.aktualni_cas + 1, true});
//                } else if (it2 != visited.end() && visited[it->second].cas_obsazeni == elem.aktualni_cas + 1 &&
//                           visited[elem.planeta].kolonizovana_jednou &&
//                           visited[it->second].rasa != elem.rasa) {
//                    visited[it->second].kolonizovana_jednou = false;
//                } else if (it2 != visited.end() && visited[it->second].cas_obsazeni > elem.aktualni_cas + 1 &&
//                           visited[elem.planeta].kolonizovana_jednou &&
//                           visited[it->second].kolonizovana_jednou &&
//                           visited[it->second].rasa != elem.rasa) {
//                    visited[it->second].rasa = elem.rasa;
//                }
//            }
//
//        }
//        for (const auto &x: visited) {
//            if (x.second.kolonizovana_jednou)
//                res[x.first] = x.second.rasa;
//        }
        return res;
    }

private:
    set<pair<string, string>> m_wormholes;
};


#ifndef __PROGTEST__

int main() {
//    CUniverse u1;
//
//    u1.addConnection("Earth", "X1")
//            .addConnection("Earth", "Y1")
//            .addConnection("Vulcan", "X1")
//            .addConnection("Vulcan", "Y2")
//            .addConnection("Y2", "Y1")
//            .addConnection("Kronos", "X1")
//            .addConnection("X1", "X2")
//            .addConnection("X2", "X3")
//            .optimize();
//
//    auto r1 = u1.colonise({
//                                  {"Earth",  "Humans",   0},
//                                  {"Vulcan", "Vulcans",  0},
//                                  {"Kronos", "Clingons", 0}});
//    assert(r1 == (map<string, string>({
//                                              {"Earth",  "Humans"},
//                                              {"Y1",     "Humans"},
//                                              {"Vulcan", "Vulcans"},
//                                              {"Y2",     "Vulcans"},
//                                              {"Kronos", "Clingons"}})));
//    auto r2 = u1.colonise({
//                                  {"Earth",  "Humans",  0},
//                                  {"Vulcan", "Vulcans", 0},
//                                  {"Kronos", "Humans",  0}});
//    assert(r2 == (map<string, string>({
//                                              {"Earth",  "Humans"},
//                                              {"Y1",     "Humans"},
//                                              {"Vulcan", "Vulcans"},
//                                              {"Y2",     "Vulcans"},
//                                              {"Kronos", "Humans"}})));
//    auto r3 = u1.colonise({
//                                  {"Unknown", "Unknown", 0}});
//    assert(r3 == (map<string, string>({{"Unknown", "Unknown"}})));
//    auto r4 = u1.colonise({});
//    assert(r4 == (map<string, string>({})));
//
//
//    CUniverse u2;
//
//    u2.addConnection("Earth", "Z1")
//            .addConnection("Earth", "Y1")
//            .addConnection("Earth", "Kronos")
//            .addConnection("Earth", "Vulcan")
//            .addConnection("Vulcan", "Y3")
//            .addConnection("Vulcan", "X1")
//            .addConnection("Kronos", "Z2")
//            .addConnection("Kronos", "X4")
//            .addConnection("Kronos", "Vulcan")
//            .addConnection("Y1", "Y2")
//            .addConnection("Y2", "Y3")
//            .addConnection("Z1", "Z2")
//            .addConnection("X1", "X2")
//            .addConnection("X2", "X3")
//            .addConnection("X1", "X3")
//            .addConnection("X3", "X4")
//            .addConnection("Bajor", "Cassiopea Prime")
//            .optimize();
//
//    auto r5 = u2.colonise({{"Earth",           "Humans",      0},
//                           {"Vulcan",          "Vulcans",     0},
//                           {"Kronos",          "Clingons",    0},
//                           {"Cassiopea Prime", "Cassiopeans", 0}});
//    assert(r5 == (map<string, string>({{"Earth",           "Humans"},
//                                       {"Kronos",          "Clingons"},
//                                       {"Vulcan",          "Vulcans"},
//                                       {"Cassiopea Prime", "Cassiopeans"},
//                                       {"Bajor",           "Cassiopeans"},
//                                       {"Z1",              "Humans"},
//                                       {"Z2",              "Clingons"},
//                                       {"Y1",              "Humans"},
//                                       {"Y3",              "Vulcans"},
//                                       {"X1",              "Vulcans"},
//                                       {"X2",              "Vulcans"},
//                                       {"X4",              "Clingons"}})));

    //Harder tests when != 0 for all colonies
    CUniverse u3;

    u3.addConnection("Earth", "Z1")
            .addConnection("Earth", "Y1")
            .addConnection("Earth", "Kronos")
            .addConnection("Earth", "Vulcan")
            .addConnection("Vulcan", "Y3")
            .addConnection("Vulcan", "X1")
            .addConnection("Kronos", "Z2")
            .addConnection("Kronos", "X4")
            .addConnection("Y1", "Y2")
            .addConnection("Y2", "Y3")
            .addConnection("Z1", "Z2")
            .addConnection("X1", "X2")
            .addConnection("X1", "X3")
            .addConnection("X2", "X3")
            .addConnection("X3", "X4")
            .addConnection("Bajor", "Cassiopea Prime")
            .optimize();

    auto r8 = u3.colonise({
                                  {"Earth",           "Humans",      1},
                                  {"Vulcan",          "Vulcans",     0},
                                  {"Kronos",          "Clingons",    2},
                                  {"Cassiopea Prime", "Cassiopeans", 10000},
                                  {"a",               "a",           100}});

    assert(r8 == (map<string, string>({
                                              {"Earth",           "Humans"},//
                                              {"Kronos",          "Clingons"},//
                                              {"Vulcan",          "Vulcans"},//
                                              {"Y1",              "Humans"},//
                                              {"Z1",              "Humans"},//
                                              {"Y3",              "Vulcans"},//
                                              {"Y2",              "Vulcans"},//
                                              {"X1",              "Vulcans"},//
                                              {"X2",              "Vulcans"},//
                                              {"X3",              "Vulcans"},//
                                              {"Cassiopea Prime", "Cassiopeans"},//
                                              {"Bajor",           "Cassiopeans"},
                                              {"a",               "a"}})));//

    return EXIT_SUCCESS;
}

#endif /* __PROGTEST__ */