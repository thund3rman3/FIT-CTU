#ifndef __PROGTEST__
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cctype>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <set>
#include <list>
#include <map>
#include <vector>
#include <queue>
#include <string>
#include <stack>
#include <queue>
#include <deque>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <functional>
#include <iterator>
#include <stdexcept>
using namespace std;
#endif /* __PROGTEST__ */

template <typename M_>
class CContest
{
public:
    // default constructor
    CContest() = default;

    // destructor
    ~CContest() = default;

    // addMatch ( contestant1, contestant2, result )
    CContest& addMatch(const string& team1, const string& team2, M_ result)
    {
        if(m_vMatch.find({team1,team2}) != m_vMatch.end() ||
            m_vMatch.find({team2,team1}) != m_vMatch.end())
                throw std::logic_error("already there");

        m_vMatch.insert({{team1,team2},result});
        return *this;
    }

    // isOrdered ( comparator )
    bool isOrdered(function<int(M_)> comparator) const
    {
        map<string,pair<set<string>,size_t>> tmp;
        for(const auto& x: m_vMatch)
        {
            if(comparator(x.second) < 0)
            {
                if( tmp.find(x.first.second) == tmp.end())
                    tmp[x.first.second].second = 0;
                tmp[x.first.second].first.emplace(x.first.first);


                if(tmp.find(x.first.first) == tmp.end())
                    tmp[x.first.first].second = 1;
                else
                    tmp[x.first.first].second++;
            }
            else if(comparator(x.second) > 0)
            {
                if( tmp.find(x.first.first) == tmp.end())
                    tmp[x.first.first].second = 0;
                tmp[x.first.first].first.emplace(x.first.second);


                if(tmp.find(x.first.second) == tmp.end())
                    tmp[x.first.second].second = 1;
                else
                    tmp[x.first.second].second++;
            }
            else
                return false;
        }

        bool done = false;
        while(tmp.empty() == false)
        {
            size_t zeros=0;
            for(const auto&x: tmp)
            {
                if(x.second.second == 0) {
                    zeros++;
                }
                if(zeros>1)
                    return false;
            }
            done = false;
            for(const auto& x: tmp)
            {
                if(x.second.second == 0) {
                    for (auto &y: tmp) {
                        if (y.first == x.first)
                            continue;
                        auto iit = x.second.first.find(y.first);
                        if (x.second.first.end() != iit)
                            y.second.second--;
                    }
                    tmp.erase(x.first);
                    done = true;
                    break;
                }
            }
            if(done == false)
                return false;
        }
        return true;
    }

    // results ( comparator )
    list<string> results(function<int(M_)> comparator) const
    {
        list<string> list_res;
        map<string,pair<set<string>,size_t>> tmp;
        for(const auto& x: m_vMatch)
        {
            if(comparator(x.second) < 0)
            {
                if( tmp.find(x.first.second) == tmp.end())
                    tmp[x.first.second].second = 0;
                tmp[x.first.second].first.emplace(x.first.first);


                if(tmp.find(x.first.first) == tmp.end())
                    tmp[x.first.first].second = 1;
                else
                    tmp[x.first.first].second++;
            }
            else if(comparator(x.second) > 0)
            {
                if( tmp.find(x.first.first) == tmp.end())
                    tmp[x.first.first].second = 0;
                tmp[x.first.first].first.emplace(x.first.second);


                if(tmp.find(x.first.second) == tmp.end())
                    tmp[x.first.second].second = 1;
                else
                    tmp[x.first.second].second++;
            }
            else
                throw std::logic_error("bad");
        }

        bool done = false;
        while(tmp.empty() == false)
        {
            size_t zeros=0;
            for(const auto&x: tmp)
            {
                if(x.second.second == 0) {
                    zeros++;
                }
                if(zeros>1)
                    throw std::logic_error("bad");
            }
            done = false;
            for(const auto& x: tmp)
            {
                if(x.second.second == 0) {
                    for (auto &y: tmp) {
                        if (y.first == x.first)
                            continue;
                        auto iit = x.second.first.find(y.first);
                        if (x.second.first.end() != iit)
                            y.second.second--;
                    }
                    list_res.emplace_back(x.first);
                    tmp.erase(x.first);
                    done = true;
                    break;
                }
            }
            if(done == false)
                throw std::logic_error("bad");
        }
        return list_res;
    }

private:
    map<pair<string,string>, M_> m_vMatch;
};

#ifndef __PROGTEST__
struct CMatch
{
public:
    CMatch(int a,
           int b)
            : m_A(a),
              m_B(b)
    {
    }

    int m_A;
    int m_B;
};

class HigherScoreThreshold
{
public:
    HigherScoreThreshold(int diffAtLeast)
            : m_DiffAtLeast(diffAtLeast)
    {
    }
    int operator()(const CMatch &x) const
    {
        return (x.m_A > x.m_B + m_DiffAtLeast) - (x.m_B > x.m_A + m_DiffAtLeast);
    }

private:
    int m_DiffAtLeast;
};

int HigherScore(const CMatch &x)
{
    return (x.m_A > x.m_B) - (x.m_B > x.m_A);
}

int main(void)
{
    CContest<CMatch> x;

    x.addMatch("C++", "Pascal", CMatch(10, 3))
            .addMatch("C++", "Java", CMatch(8, 1))
            .addMatch("Pascal", "Basic", CMatch(40, 0))
            .addMatch("Java", "PHP", CMatch(6, 2))
            .addMatch("Java", "Pascal", CMatch(7, 3))
            .addMatch("PHP", "Basic", CMatch(10, 0));

   assert(!x.isOrdered(HigherScore));
  try
  {
    list<string> res = x.results(HigherScore);
    assert("Exception missing!" == nullptr);
  }
  catch (const logic_error &e)
  {
  }
  catch (...)
  {
    assert("Invalid exception thrown!" == nullptr);
  }

  x.addMatch("PHP", "Pascal", CMatch(3, 6));

  assert(x.isOrdered(HigherScore));
  try
  {
    list<string> res = x.results(HigherScore);
    assert((res == list<string>{"C++", "Java", "Pascal", "PHP", "Basic"}));
  }
  catch (...)
  {
    assert("Unexpected exception!" == nullptr);
  }

  assert(!x.isOrdered(HigherScoreThreshold(3)));
    try
    {
      list<string> res = x.results(HigherScoreThreshold(3));
      assert("Exception missing!" == nullptr);
    }
    catch (const logic_error &e)
    {
    }
    catch (...)
    {
      assert("Invalid exception thrown!" == nullptr);
    }

    assert(x.isOrdered([](const CMatch &x)
                       { return (x.m_A < x.m_B) - (x.m_B < x.m_A); }));
    try
    {
      list<string> res = x.results([](const CMatch &x)
                                   { return (x.m_A < x.m_B) - (x.m_B < x.m_A); });
      assert((res == list<string>{"Basic", "PHP", "Pascal", "Java", "C++"}));
    }
    catch (...)
    {
      assert("Unexpected exception!" == nullptr);
    }

    CContest<bool> y;

    y.addMatch("Python", "PHP", true)
        .addMatch("PHP", "Perl", true)
        .addMatch("Perl", "Bash", true)
        .addMatch("Bash", "JavaScript", true)
        .addMatch("JavaScript", "VBScript", true);

    assert(y.isOrdered([](bool v)
                       { return v ? 10 : -10; }));
    try
    {
      list<string> res = y.results([](bool v)
                                   { return v ? 10 : -10; });
      assert((res == list<string>{"Python", "PHP", "Perl", "Bash", "JavaScript", "VBScript"}));
    }
    catch (...)
    {
      assert("Unexpected exception!" == nullptr);
    }

    y.addMatch("PHP", "JavaScript", false);
    assert(!y.isOrdered([](bool v)
                        { return v ? 10 : -10; }));
    try
    {
      list<string> res = y.results([](bool v)
                                   { return v ? 10 : -10; });
      assert("Exception missing!" == nullptr);
    }
    catch (const logic_error &e)
    {
    }
    catch (...)
    {
      assert("Invalid exception thrown!" == nullptr);
    }

    try
    {
      y.addMatch("PHP", "JavaScript", false);
      assert("Exception missing!" == nullptr);
    }
    catch (const logic_error &e)
    {
    }
    catch (...)
    {
      assert("Invalid exception thrown!" == nullptr);
    }

    try
    {
      y.addMatch("JavaScript", "PHP", true);
      assert("Exception missing!" == nullptr);
    }
    catch (const logic_error &e)
    {
    }
    catch (...)
    {
      assert("Invalid exception thrown!" == nullptr);
    }
    return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */

