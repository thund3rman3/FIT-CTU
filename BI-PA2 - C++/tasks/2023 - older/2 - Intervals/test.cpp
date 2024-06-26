#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <ctime>
#include <climits>
#include <cmath>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <functional>
#include <stdexcept>
#include <memory>
using namespace std;
#endif /* __PROGTEST__ */

// uncomment if your code implements initializer lists
// #define EXTENDED_SYNTAX

class CRange
{
public:
  // constructor
  CRange(long long lo, long long hi)
  {
    if (lo > hi)
      throw logic_error("logic error");
    m_lo = lo;
    m_hi = hi;
  }

  CRange(long long val)
      : m_lo(val), m_hi(val) {}

  long long get_lo() const
  {
    return m_lo;
  }

  long long get_hi() const
  {
    return m_hi;
  }

  void set_lo(long long lo)
  {
    m_lo = lo;
  }

  void set_hi(long long hi)
  {
    m_hi = hi;
  }

private:
  // todo
  long long m_lo;
  long long m_hi;
};

class CRangeList
{
public:
  // constructor
  CRangeList() {}

  CRangeList(const CRangeList &right)
  {
    for (auto &x : right.lInterval)
    {
      this->lInterval.push_back(x);
    }
  }

  ~CRangeList() {}

  CRangeList &operator+(const CRange &right)
  {
    *this += right;
    return *this;
  }

  CRangeList &operator-(const CRange &right)
  {
    *this -= right;
    return *this;
  }

  // includes long long / range
  bool includes(long long num) const
  {
    for (auto &x : this->lInterval)
    {
      long long lo = x.get_lo();
      long long hi = x.get_hi();
      if ((hi == lo && lo == num) ||
          (num >= lo && num <= hi))
        return true;
    }
    return false;
  }
  bool includes(const CRange &range) const
  {
    for (auto &x : this->lInterval)
    {
      long long lo = x.get_lo();
      long long hi = x.get_hi();
      long long search_lo = range.get_lo();
      long long search_hi = range.get_hi();
      if (search_lo >= lo && search_hi <= hi)
        return true;
    }
    return false;
  }
  // += range / range list

  bool find_CRange_in_list(const CRangeList &a, long long add_lo, long long add_hi) const
  {
    // is it in range already?
    bool found = false;
    for (auto &x : a.lInterval)
    {
      if (x.get_lo() <= add_lo && x.get_hi() >= add_hi)
      {
        found = true;
        break;
      }
    }
    return found;
  }

  bool check_intersection(const long long x_lo, const long long x_hi, const long long add_lo, const long long add_hi) const
  {
    if (x_lo == LLONG_MIN || x_hi == LLONG_MAX)
    {
      if (x_lo == LLONG_MIN && x_hi < LLONG_MAX && x_lo <= add_hi && x_hi + 1 >= add_lo)
        return true;
      else if (x_lo > LLONG_MIN && x_hi == LLONG_MAX && x_lo - 1 <= add_hi && x_hi >= add_lo)
        return true;
      else if (x_lo <= add_hi && x_hi >= add_lo)
        return true;
    }
    else if (x_lo - 1 <= add_hi && x_hi + 1 >= add_lo)
    {
      return true;
    }
    return false;
  }

  CRangeList &operator+=(const CRange &range_right)
  {
    long long add_lo = range_right.get_lo();
    long long add_hi = range_right.get_hi();

    if (find_CRange_in_list(*this, add_lo, add_hi) == true)
      return *this;

    bool happend = true;
    long idx = -1;
    long long it_hi = 0, it_lo = 0;

    while (happend == true)
    {
      happend = false;
      for (auto it = this->lInterval.begin(); it != this->lInterval.end(); ++it)
      {
        it_lo = it->get_lo();
        it_hi = it->get_hi();
        if (idx == it - this->lInterval.begin())
          continue;
        if (check_intersection(it_lo, it_hi, add_lo, add_hi) == true)
        {
          it->set_lo(it_lo < add_lo ? it_lo : add_lo);
          it->set_hi(it_hi > add_hi ? it_hi : add_hi);
          add_lo = it->get_lo();
          add_hi = it->get_hi();
          happend = true;
          if (idx != -1)
          {
            auto it_erase = --it;
            // advance(it_erase, idx);
            this->lInterval.erase(it_erase);
            // it--;
          }
          idx = it - this->lInterval.begin();
          break;
        }
      }
      if (idx == -1)
      {
        auto pos = lower_bound(this->lInterval.begin(), this->lInterval.end(),
                               CRange(add_lo),
                               [](const CRange &a, const CRange &b)
                               { return a.get_lo() < b.get_lo(); });
        this->lInterval.emplace(pos, CRange(add_lo, add_hi));
      }
    }

    return *this;
  }
  CRangeList &operator+=(const CRangeList &list_right)
  {
    if (this == &list_right)
      return *this;
    for (auto &x : list_right.lInterval)
    {
      *this += x;
    }
    return *this;
  }

  // -= range / range list
  CRangeList &operator-=(const CRange &range_right)
  {
    long long rem_lo = range_right.get_lo();
    long long rem_hi = range_right.get_hi();
    long long it_hi = 0, it_lo = 0;
    bool it_change = false;

    for (auto it = this->lInterval.begin(); it != this->lInterval.end(); ++it)
    {
      if (this->lInterval.empty())
        return *this;
      if (it_change == true)
      {
        it_change = false;
        it = this->lInterval.begin();
      }
      it_lo = it->get_lo();
      it_hi = it->get_hi();
      if ((it_lo <= rem_hi && it_hi >= rem_lo) == true)
      {
        if ((it_hi == rem_hi && it_lo == rem_lo) ||
            (rem_lo < it_lo && rem_hi == it_hi)) // eq and whole+over_left
        {
          this->lInterval.erase(it);
          break;
        }
        else if (it_hi > rem_hi && rem_lo <= it_lo) // left and over_left
        {
          it->set_lo(rem_hi + 1);
          break;
        }
        else if (it_lo < rem_lo && rem_hi == it_hi) // right
        {
          it->set_hi(rem_lo - 1);
          break;
        }
        else if (rem_lo > it_lo && rem_hi > it_hi) // right_over
        {
          it->set_hi(rem_lo - 1);
        }
        else if ((it_hi < rem_hi && it_lo > rem_lo) ||
                 (rem_lo >= it_lo && it_hi < rem_hi)) // over and whole+over_right
        {
          //size_t idx = it - this->lInterval.begin();
          it = this->lInterval.erase(it) - 1;
          it_change = true;
        }
        else if (rem_hi < it_hi && rem_lo > it_lo) // inside split
        {
          size_t idx = it - this->lInterval.begin();
          auto it_em = it + 1;
          this->lInterval.emplace(it_em, CRange(rem_hi + 1, it_hi));
          it = idx + this->lInterval.begin();
          it->set_hi(rem_lo - 1);
          break;
        }
      }
    }
    return *this;
  }

  CRangeList &operator-=(const CRangeList &list_right)
  {
    if (this == &list_right)
    {
      this->lInterval.clear();
      return *this;
    }
    for (auto &x : list_right.lInterval)
    {
      *this -= x;
    }
    return *this;
  }

  // = range / range list
  CRangeList &operator=(const CRange &right)
  {
    this->lInterval.clear();
    this->lInterval.push_back(right);
    return *this;
  }

  CRangeList &operator=(const CRangeList &right)
  {
    if (this == &right)
      return *this;

    this->lInterval.clear();
    for (const auto &x : right.lInterval)
    {
      this->lInterval.push_back(x);
    }
    return *this;
  }
  // operator ==
  bool operator==(const CRangeList &list_right) const
  {
    if (this->lInterval.size() != list_right.lInterval.size())
      return false;

    auto left = this->lInterval.begin();
    auto right = list_right.lInterval.begin();

    for (; left != this->lInterval.end() || right != list_right.lInterval.end();
         ++left, ++right)
    {
      if ((left->get_lo() != right->get_lo()) ||
          (left->get_hi() != right->get_hi()))
        return false;
    }
    return true;
  }
  // operator !=
  bool operator!=(const CRangeList &list_right) const
  {
    if (this->lInterval.size() != list_right.lInterval.size())
      return true;

    auto left = this->lInterval.begin();
    auto right = list_right.lInterval.begin();

    for (; left != this->lInterval.end() || right != list_right.lInterval.end();
         ++left, ++right)
    {
      if ((left->get_lo() != right->get_lo()) ||
          (left->get_hi() != right->get_hi()))
        return true;
    }
    return false;
  }
  // operator <<
  friend ostream &operator<<(ostream &os_left, const CRangeList &list_right)
  {
    os_left << "{";
    for (auto it = list_right.lInterval.begin(); it != list_right.lInterval.end(); ++it)
    {
      if (it->get_lo() == it->get_hi())
        os_left << it->get_lo();
      else
        os_left << "<" << it->get_lo() << ".." << it->get_hi() << ">";

      if (it + 1 != list_right.lInterval.end())
      {
        os_left << ",";
      }
    }
    os_left << "}";
    return os_left;
  }

private:
  // todo
  vector<CRange> lInterval;
};

CRangeList operator+(const CRange &left, const CRange &right)
{
  CRangeList tmp;
  tmp = left;
  tmp += right;
  return tmp;
}

CRangeList operator-(const CRange &left, const CRange &right)
{
  CRangeList tmp;
  tmp = left;
  tmp -= right;
  return tmp;
}

#ifndef __PROGTEST__
string toString(const CRangeList &x)
{
  ostringstream oss;
  oss << x;
  return oss.str();
}

int main(void)
{
  CRangeList a, b;
  a+={12,17}+CRange(79,79);
  assert(sizeof(CRange) <= 2 * sizeof(long long));
  a = CRange(5, 10);
  a += CRange(25, 100);
  assert(toString(a) == "{<5..10>,<25..100>}");
  a += CRange(-5, 0);
  a += CRange(8, 50);
  assert(toString(a) == "{<-5..0>,<5..100>}");
  a += CRange(101, 105) + CRange(120, 150) + CRange(160, 180) + CRange(190, 210);
  assert(toString(a) == "{<-5..0>,<5..105>,<120..150>,<160..180>,<190..210>}");
  a += CRange(106, 119) + CRange(152, 158);
  assert(toString(a) == "{<-5..0>,<5..150>,<152..158>,<160..180>,<190..210>}");
  a += CRange(-3, 170);
  a += CRange(-30, 1000);
  assert(toString(a) == "{<-30..1000>}");
  b = CRange(-500, -300) + CRange(2000, 3000) + CRange(700, 1001);
  a += b;
  assert(toString(a) == "{<-500..-300>,<-30..1001>,<2000..3000>}");
  a -= CRange(-400, -400);
  assert(toString(a) == "{<-500..-401>,<-399..-300>,<-30..1001>,<2000..3000>}");
  a -= CRange(10, 20) + CRange(900, 2500) + CRange(30, 40) + CRange(10000, 20000);
  assert(toString(a) == "{<-500..-401>,<-399..-300>,<-30..9>,<21..29>,<41..899>,<2501..3000>}");
  try
  {
    a += CRange(15, 18) + CRange(10, 0) + CRange(35, 38);
    assert("Exception not thrown" == nullptr);
  }
  catch (const std::logic_error &e)
  {
  }
  catch (...)
  {
    assert("Invalid exception thrown" == nullptr);
  }
  assert(toString(a) == "{<-500..-401>,<-399..-300>,<-30..9>,<21..29>,<41..899>,<2501..3000>}");
  b = a;
  assert(a == b);
  assert(!(a != b));
  b += CRange(2600, 2700);
  assert(toString(b) == "{<-500..-401>,<-399..-300>,<-30..9>,<21..29>,<41..899>,<2501..3000>}");
  assert(a == b);
  assert(!(a != b));
  b += CRange(15, 15);
  assert(toString(b) == "{<-500..-401>,<-399..-300>,<-30..9>,15,<21..29>,<41..899>,<2501..3000>}");
  assert(!(a == b));
  assert(a != b);
  assert(b.includes(15));
  assert(b.includes(2900));
  assert(b.includes(CRange(15, 15)));
  assert(b.includes(CRange(-350, -350)));
  assert(b.includes(CRange(100, 200)));
  assert(!b.includes(CRange(800, 900)));
  assert(!b.includes(CRange(-1000, -450)));
  assert(!b.includes(CRange(0, 500)));
  a += CRange(-10000, 10000) + CRange(10000000, 1000000000);
  assert(toString(a) == "{<-10000..10000>,<10000000..1000000000>}");
  b += a;
  assert(toString(b) == "{<-10000..10000>,<10000000..1000000000>}");
  b -= a;
  assert(toString(b) == "{}");
  b += CRange(0, 100) + CRange(200, 300) - CRange(150, 250) + CRange(160, 180) - CRange(170, 170);
  assert(toString(b) == "{<0..100>,<160..169>,<171..180>,<251..300>}");
  b -= CRange(10, 90) - CRange(20, 30) - CRange(40, 50) - CRange(60, 90) + CRange(70, 80);
  assert(toString(b) == "{<0..9>,<20..30>,<40..50>,<60..69>,<81..100>,<160..169>,<171..180>,<251..300>}");

#ifdef EXTENDED_SYNTAX
  CRangeList x{{5, 20}, {150, 200}, {-9, 12}, {48, 93}};
  assert(toString(x) == "{<-9..20>,<48..93>,<150..200>}");
  ostringstream oss;
  oss << setfill('=') << hex << left;
  for (const auto &v : x + CRange(-100, -100))
    oss << v << endl;
  oss << setw(10) << 1024;
  assert(oss.str() == "-100\n<-9..20>\n<48..93>\n<150..200>\n400=======");
#endif /* EXTENDED_SYNTAX */
  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
