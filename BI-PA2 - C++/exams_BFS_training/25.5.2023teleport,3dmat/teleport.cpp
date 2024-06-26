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
    multimap<pair<string,string>, pair<unsigned, unsigned >> m_mmPATHS; // from, to, fromTime, toTime

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
        m_mmPATHS.emplace(make_pair(from,to), make_pair(fromTime,toTime));
        return *this;
    }

    CTeleport & Optimize ( )
    {
       return *this;
    }

    unsigned FindWay ( const string & from,
                       const string & to,
                       unsigned time )
    {
        if(from == to)
            return 0;

        map<string, unsigned> visited; // destination , arriveTime
        deque<string> nextToVisit;

        visited.emplace(from, time);
        nextToVisit.push_back(from);

        while(!nextToVisit.empty())
        {
            string elem = nextToVisit.front();
            nextToVisit.pop_front();
            unsigned takeoff = visited[elem];
            for (auto it = m_mmPATHS.lower_bound(make_pair(elem,""));
                it!= m_mmPATHS.end() &&
                it->first.first == elem; ++it)
            {
                if(it->first.first == from && visited.count(it->first.second) == 1)
                    continue;
                else if( visited.count(it->first.second) == 0 && it->second.first >= takeoff)
                {
                    visited.emplace(it->first.second, it->second.second);
                    nextToVisit.push_back(it->first.second);
                }
                else if( visited.count(it->first.second) == 1 && visited[it->first.second] > it->second.second)
                {
                    visited[it->first.second] = it->second.second;
                    nextToVisit.push_back(it->first.second);
                }
            }
        }
        if(visited.count(to) != 1)
            throw invalid_argument("");
        // return time arrive in to
        return visited[to];
    }
};

#ifndef __PROGTEST__
int main ( )
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