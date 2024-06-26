#include <iostream>
#include <set>
#include <string>
#include <map>
#include <queue>
#include <cassert>
#include "algorithm"

/*
BFS - 18 bodů - setkání

Pořádá se setkání lidí a vaším úkolem je nalézt nejvýhodnější místo konání tohoto setkání.
 Na vstupu dostanete seznam cest mezi dvěma městy. Délka každé cesty je 1. Musíte nalézt města,
 kde bude nejvýhodnější uspořádat setkání.
Výhodností se myslí vzdálenost do cílové města člověka, který to má do daného města nejdále.
Implementujte následující rozhraní třídy CClass:

CClass & Add (string from, string to)
Přidá cestu mezi dvěma městy. Cesta je obousměrná

CClass & optimise()
Metoda, kterou nikdo nepoužívá

set<string> find (vector<string> cities)
Metoda najde hledaná města. Ve výsledku může být více různých měst, v tomto případě vratně všechna nejvýhodnější města.
 Pokud město setkání nelze nalézt (neexistuje cesta mezi všemi počátečními městy), vraťte prázdnou množinu.
Cities - vektor měst, kde se nachází všichni účastníci setkání (nebylo řečeno, ale řekl bych, že se města mohou ve vektoru opakovat)
*/
class CConnect {
public:
    void add(std::string from, std::string to) {
        graph.emplace(from, to);
        graph.emplace(to, from);
    }

    std::set<std::string> findNearest(const std::vector<std::string> &src) {
        std::set<std::string> cities(src.begin(), src.end());
        std::map<std::string, std::vector<int>> cnts; //city , times visited
        if (cities.size() == 1 || cities.empty())
            return cities;

        int cnt = 0;
        for (const auto &str: cities) {
            std::map<std::string, int> visited;
            std::queue<std::string> next;
            next.emplace(str);
            visited.emplace(str, 0);

            while (!next.empty()) {
                auto elem = next.front();
                next.pop();

                for (auto it = graph.lower_bound(std::make_pair(elem, ""));
                     it != graph.end() && it->first == elem; ++it) {
                    if (visited.count(it->second) == 0 || visited[it->second] > visited[elem] + 1) {
                        next.emplace(it->second);
                        visited[it->second] = visited[elem] + 1;
                    }
                }
            }
            for (const auto& [city, len]: visited) {
                    cnts[city].emplace_back(len);
            }
            cnt++;
        }
        std::set<std::string> res;
        int max = INT_MAX;
        for (const auto& [city, vec]: cnts) {
            if(vec.size() != cities.size())
                return {};
            auto maxInVec = std::max_element(vec.begin(),vec.end());
            if(*maxInVec < max){
                res.clear();
                max = *maxInVec;
            }
            if(*maxInVec == max){
                res.emplace(city);
            }
        }
        return res;
    }

    std::set<std::pair<std::string, std::string>> graph;
};

//class CConnect {
//public:
//    void add(std::string from, std::string to) {
//        m_graph[from].insert(to);
//        m_graph[to].insert(from);
//    }
//
//    std::set<std::string> findNearest(const std::vector<std::string> &src) {
//        std::set<std::pair<std::string, int>> visited; //visited string and id.
//        std::map<std::pair<std::string, int>, int> distances;//name of a stop and id,  distance
//        std::map<std::string, int> counter;
//        std::set<std::string> final;
//        int cnt = 0;
//        int max_dist = 185156156;
//        std::queue<std::pair<std::string, int>> Q_; // string of the stop and int of the id;
//
//        for (const auto &itr: src) {
//            counter[itr]++;
//            distances[{itr, cnt}] = 0;
//            Q_.push({itr, cnt});
//            cnt++;
//            if(counter[itr]>=src.size()){
//                std::set<std::string>ahah;
//                ahah.insert(itr);
//                return ahah;
//            }
//        }
//
//        while (!Q_.empty()) {
//            auto curr = Q_.front();
//            Q_.pop();
//
//            if (m_graph.count(curr.first) >= 1) {
//                for (const auto &neighbour: m_graph[curr.first]) {
//                    if (!visited.count({neighbour, curr.second})) {
//                        if (distances.count({neighbour, curr.second}) >= 1) {
//                            if (distances[{neighbour, curr.second}]< distances[{curr.first, curr.second}] + 1) {
//                                continue;
//                            }
//
//                            if (distances[{neighbour, curr.second}] > distances[{curr.first, curr.second}] + 1) {
//                                distances[{neighbour, curr.second}] = distances[{curr.first, curr.second}] + 1;
//                            }
//                        } else {
//                            distances[{neighbour, curr.second}] = distances[{curr.first, curr.second}] + 1;
//                        }
//                        counter[neighbour]++;
//                        if (counter[neighbour] >= src.size()) {
//                            if (max_dist < distances[{neighbour, curr.second}]) {
//                                return final;
//                            }
//                            max_dist = distances[{neighbour, curr.second}];
//                            final.insert(neighbour);
//                        }
//                        Q_.push({neighbour, curr.second});
//                        visited.insert({neighbour, curr.second});
//                    }
//                }
//            }
//        }
//        std::set<std::string> gg;
//        return gg;
//
//
//    }
//
//private:
//    std::map<std::string, std::set<std::string>> m_graph;
//};
int main(void) {
    CConnect t0;
    t0.add("Praha", "Moskva");
    t0.add("Praha", "Viden");
    t0.add("Praha", "Helsinki");
    t0.add("Helsinki", "Brno");
    t0.add("Moskva", "Brno");
    t0.add("Brno", "Rim");
    t0.add("Brno", "Bratislava");
    t0.add("Budapest", "Bratislava");

    auto result = t0.findNearest({"Praha", "Bratislava"});
    std::set<std::string> expected = {"Brno", "Moskva", "Helsinki"};
    assert(result == expected);

    result = t0.findNearest({"Prosek", "Praha", "Praha", "Praha"});
    expected = {};
    assert(result == expected);

    result = t0.findNearest({"Praha", "Praha", "Bratislava", "Bratislava"});
    expected = {"Brno", "Moskva", "Helsinki"};
    assert(result == expected);

    result = t0.findNearest({"Rim", "Rim"});
    expected = {"Rim"};
    assert(result == expected);

    result = t0.findNearest({"Praha", "Moskva"});
    expected = {"Praha", "Moskva"};
    assert(result == expected);

    result = t0.findNearest({"Praha", "Praha", "Praha", "Moskva"});
    expected = {"Moskva", "Praha"};
    assert(result == expected);

    result = t0.findNearest({"Praha", "Praha", "Praha", "Praha"});
    expected = {"Praha"};
    assert(result == expected);

    result = t0.findNearest({"Praha", "Praha", "Praha", "Praha"});
    expected = {"Praha"};
    assert(result == expected);


    t0.add("Praha", "Budapest");
    result = t0.findNearest({"Praha", "Budapest"});
    expected = {"Praha", "Budapest"};
    assert(result == expected);

    t0.add("Helsinki", "Budapest");
    result = t0.findNearest({"Praha", "Budapest"});
    expected = {"Praha", "Budapest", "Helsinki"};
    assert(result == expected);


    result = t0.findNearest({"Praha", "Rim"});
    expected = {"Helsinki", "Brno", "Moskva", "Bratislava"};
    assert(result == expected);

    std::cout << "Test passed! The result set matches the expected set." << std::endl;

}
