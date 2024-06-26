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

class CPublicTransport {
public:
    CPublicTransport()= default;

    CPublicTransport& addLine(unsigned cost, const vector<string>& stops) {
        m_vAllStops.emplace_back(cost, stops);
        return *this;
    }

    CPublicTransport& optimize() {
        for (auto& [cost, stops]: m_vAllStops) {
            stable_sort(stops.begin(), stops.end());
        }
        return *this;
    }

    unsigned findCheapest(const string& from, const string& to) {
        unsigned cheapest = UINT32_MAX;
        unsigned price_for_first_ride = 0;
        bool one_line = false, from_found = false;
        if(from == to)
            return 0;

        F_hrany.clear();
        vector<pair<unsigned , vector<string>>> tmp_linky(m_vAllStops);
        for (auto& [price, stops] : m_vAllStops) {
            auto from_on_line = lower_bound(stops.begin(), stops.end(), from);
            if(from_on_line != stops.end() && *from_on_line == from)
            {
                from_found = true;
                auto to_on_line = lower_bound(stops.begin(), stops.end(), to);
                if(to_on_line != stops.end() &&  *to_on_line == to)
                {
                    one_line = true;
                    if(price < cheapest)
                        cheapest = price;
                }
                else
                {
                    F_hrany.emplace_back(price, stops);
                    auto t = lower_bound(tmp_linky.begin(),tmp_linky.end(), make_pair(price,stops));
                    if(t != tmp_linky.end() && t->second == stops && t->first == price)
                        tmp_linky.erase(t);
                    price_for_first_ride = price;
                }
                break;
            }
        }
        if(!from_found)
            throw logic_error("NO NO FROM IS NOT HERE");
        if(one_line)
            return cheapest;

        C.clear();

        // Dokud F neni prazdna
        while (!F_hrany.empty())
        {
            // 1. prvek z F uloz a smaz ho z F
            auto elem = F_hrany.front();
            F_hrany.pop_front();

            for (auto x = tmp_linky.begin(); x != tmp_linky.end(); ++x) {
                if(x->first == elem.first){
                    tmp_linky.erase(x);
                    break;
                }
            }

            // pro kazdou zastavku spoje
            for (auto & x : elem.second) {
                for (auto & y : tmp_linky) {
                    auto prestup = lower_bound(y.second.begin(), y.second.end(), x);
                    if(prestup != y.second.end() && *prestup == x)
                    {
                        F_hrany.emplace_back(y);
                        C.insert(y);
                    }
                }
            }

            for (const auto & it : C) {
                auto destination = lower_bound(it.second.begin(), it.second.end(), to);
                if(destination != it.second.end() && *destination == to)
                {
                    unsigned cash = it.first + price_for_first_ride;
                    if(cash < cheapest)
                        cheapest=cash;
                }//OPRAVA MYSLIM
            }
            price_for_first_ride = cheapest;
            C.clear();

        }
        if(cheapest == UINT32_MAX)
            throw logic_error("m");
        return cheapest;
    }


private:
    set<pair<unsigned , vector<string>>> C;
    deque<pair<unsigned , vector<string>>> F_hrany;
    vector<pair<unsigned , vector<string>>> m_vAllStops;
};

int main() {

    CPublicTransport t;

    t.addLine(1, {"Staromestska", "Muzeum", "Mustek", "Namesti Miru", "Jiriho z Podebrad"}).
            addLine(3, {"Hlavni Nadrazi", "Orionka", "Baterie", "Petriny", "Jiriho z Podebrad"}).
            addLine(2, {"Kamenicka", "Letenske namesti", "Baterie", "Petriny", "Namesti Miru", "Malovanka"}).optimize();


    assert(t.findCheapest("Staromestska", "Staromestska") == 0);
    assert(t.findCheapest("Staromestska", "Namesti Miru") == 1);
    assert(t.findCheapest("Staromestska", "Baterie") == 3);
    assert(t.findCheapest("Staromestska", "Hlavni Nadrazi") == 4);
    assert(t.findCheapest("Orionka", "Kamenicka") == 5);

    t.addLine(6, {"Vitezne namesti", "I P Pavlova", "Andel"});

    try {t.findCheapest("Orionka", "Andel"); }
    catch ( const logic_error & e ) { assert("m"s == e.what());}

}