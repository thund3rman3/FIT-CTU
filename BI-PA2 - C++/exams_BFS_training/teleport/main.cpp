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
#endif /* __PROGTEST__ */

class CTeleport
{
    // todo
    std::map<std::pair<std::string, std::string>, std::set<std::pair<unsigned,unsigned>>> tps;
public:

    // ctor
    CTeleport() = default;
    // dtor
    ~CTeleport() = default;

    CTeleport & Add ( const string & from,
                      const string & to,
                      unsigned fromTime,
                      unsigned toTime )
    {
        tps[{from,to}].emplace(fromTime, toTime);
        return *this;
    }

    CTeleport & Optimize ( void )
    {
        return *this;
    }

    unsigned FindWay ( const string & from,
                       const string & to,
                       unsigned time )
    {
        std::map<std::string, unsigned > visited;
        std::queue<std::string> next;
        visited[from] = time;
        next.emplace(from);

        while(!next.empty()){
            auto elem = next.front();
            next.pop();

            for (auto it = tps.lower_bound({elem,""});
                it!=tps.end() && it->first.first == elem; ++it) {
                for(const auto& [from, to]: it->second){
                    if(from >= visited[elem] && (visited.count(it->first.second) == 0 || visited[it->first.second] > to) ){
                        visited[it->first.second] = to;
                        next.emplace(it->first.second);
                    }
                }
            }
        }
        if(visited.count(to) == 0)
            throw invalid_argument("ojoj");
        return visited[to];
    }
};

#ifndef __PROGTEST__
int main ( void )
{
    CTeleport t;
    t . Add ( "Prague", "Vienna", 0, 7 )
            . Add ( "Vienna", "Berlin", 9, 260 )
            . Add ( "Vienna", "London", 8, 120 )
            . Add ( "Vienna", "Chicago", 4, 3 )
            . Add ( "Prague", "Vienna", 10, 10 ) . Optimize ( );

    assert ( t . FindWay ( "Prague", "Vienna", 0 ) == 7 );
    assert ( t . FindWay ( "Prague", "Vienna", 1 ) == 10 );
    assert ( t . FindWay ( "Prague", "London", 0 ) == 120 );
    assert ( t . FindWay ( "Vienna", "Chicago", 4 ) == 3 );

    try { t . FindWay ( "Prague", "London", 2 ); assert ( "Missing exception" == nullptr ); }
    catch ( const invalid_argument & e ) { }
    catch ( ... ) { assert ( "Invalid exception" == nullptr ); }
    try { t . FindWay ( "Prague", "Chicago", 0 ); assert ( "Missing exception" == nullptr ); }
    catch ( const invalid_argument & e ) { }
    catch ( ... ) { assert ( "Invalid exception" == nullptr ); }

    t . Add ( "Dallas", "Atlanta", 150, 30 )
            . Add ( "Berlin", "Helsinki", 1080, 2560 )
            . Add ( "Chicago", "Frankfurt", 50, 0 )
            . Add ( "Helsinki", "Vienna", 3200, 3 )
            . Add ( "Chicago", "London", 10, 12 )
            . Add ( "London", "Atlanta", 20, 40 )
            . Add ( "Vienna", "Atlanta", 10, 50 )
            . Add ( "Prague", "Vienna", 1, 6 )
            . Add ( "Berlin", "Helsinki", 265, 265 )
            . Add ( "Berlin", "London", 259, 0 ) . Optimize ( );

    assert ( t . FindWay ( "Prague", "Frankfurt", 0 ) == 0 );
    assert ( t . FindWay ( "Prague", "Atlanta", 0 ) == 40 );
    assert ( t . FindWay ( "Prague", "Atlanta", 10 ) == 50 );

    return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */