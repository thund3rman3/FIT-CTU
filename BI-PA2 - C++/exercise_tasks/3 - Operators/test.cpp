#ifndef __PROGTEST__

#include <iostream>
#include <iomanip>
#include <string>
#include <stdexcept>

using namespace std;

class CTimeTester;

#endif /* __PROGTEST__ */

const int dayInSec = 24 * 3600;

class CTime {
private:
    int m_Hour;
    int m_Minute;
    int m_Second;
public:
    // constructor, destructor
    CTime()
            : m_Hour(0), m_Minute(0), m_Second(0) {
    }

    explicit CTime(int s) {
        if (s >= dayInSec)
            s %= dayInSec;
        while (s < 0)
            s += dayInSec;
        m_Hour = s / 3600;
        s -= m_Hour * 3600;
        m_Minute = s > 0 ? s / 60 : 0;
        s -= m_Minute * 60;
        m_Second = s > 0 ? s : 0;
    }

    CTime(int h, int min) {
        if (h > 23 || h < 0 || min > 59 || min < 0)
            throw std::invalid_argument("");
        m_Hour = h;
        m_Minute = min;
        m_Second = 0;
    }

    CTime(int h, int min, int s) {
        if (h > 23 || h < 0 || min > 59 || min < 0 || s < 0 || s > 59)
            throw std::invalid_argument("");
        m_Hour = h;
        m_Minute = min;
        m_Second = s;
    }

//    CTime(const CTime& time){
//        overwrite(time.sum());
//    }
//    CTime& operator=(const CTime& time){
//        if(this == &time)
//            return *this;
//        overwrite(time.sum());
//        return *this;
//    }
//    ~CTime() = default;

    int sum() const {
        return m_Hour * 3600 + m_Minute * 60 + m_Second;
    }

    void overwrite(int s) {
        if (s >= dayInSec)
            s %= dayInSec;
        while (s < 0)
            s += dayInSec;

        m_Hour = s / 3600;
        s -= m_Hour * 3600;
        m_Minute = s > 0 ? s / 60 : 0;
        s -= m_Minute * 60;
        m_Second = s > 0 ? s : 0;
    }

    // arithmetic operators
    CTime operator+(int s) const {
        return CTime(sum() + s);
    }

    friend CTime operator+(int s, const CTime &timer) {
        return CTime(s + timer.sum());
    }

    CTime &operator+=(int s) {
        overwrite(sum() + s);
        return *this;
    }

    friend int operator-(const CTime &time1, const CTime &time2) {
        int one = time1.sum() - time2.sum();
        int two = time2.sum() - time1.sum();
        while (one < 0)
            one += dayInSec;
        while (two < 0)
            two += dayInSec;
        one %= dayInSec;
        two %= dayInSec;
        return one < two ? one : two;
    }

    CTime operator-(int s) const {
        return CTime(sum() - s);
    }

    friend CTime operator-(int s, const CTime &timer) {
        return CTime(timer.sum() - s);
    }


    CTime &operator-=(int s) {
        overwrite(sum() - s);
        return *this;
    }

    CTime &operator++() { //pre
        overwrite(sum() + 1);
        return *this;
    }

    CTime operator++(int) { // post
        CTime res(*this);
        overwrite(sum() + 1);
        return res;
    }

    CTime &operator--() { // pre
        overwrite(sum() - 1);
        return *this;
    }

    CTime operator--(int) { // post
        CTime res(*this);
        overwrite(sum() - 1);
        return res;
    }

    // comparison operators

    // pouzite ze cviceni
    bool operator<(const CTime &other) const {
        return this->sum() < other.sum();
    }

    bool operator<=(const CTime &other) const {
        return !(*this > other);
    }

    bool operator>(const CTime &other) const {
        return other < *this;
    }

    bool operator>=(const CTime &other) const {
        return !(*this < other);
    }

    bool operator==(const CTime &other) const {
        return this->sum() == other.sum();
    }

    bool operator!=(const CTime &other) const {
        return !(*this == other);
    }

    // output operator
    friend std::ostream &operator<<(std::ostream &os, const CTime &time) {
        os << setw(2) << time.m_Hour << ":" << setfill('0') << setw(2) << time.m_Minute;
        os << ":" << setfill('0') << setw(2) << time.m_Second;
        return os;
    }


    friend class ::CTimeTester;
};

#ifndef __PROGTEST__

struct CTimeTester {
    static bool test(const CTime &time, int hour, int minute, int second) {
        return time.m_Hour == hour
               && time.m_Minute == minute
               && time.m_Second == second;
    }
};

#include <cassert>
#include <sstream>

int main() {
    CTime aaaaa(7200);
    CTime aaaa(-3333);
    int r = aaaaa - aaaa;
    CTime aaaaaaaav = aaaa - 10;
    aaaaaaaav = 10 - aaaa;
    assert(CTimeTester::test(aaaaa, 2, 0, 0));
    CTime a{12, 30};
    assert(CTimeTester::test(a, 12, 30, 0));

    CTime b{13, 30};
    assert(CTimeTester::test(b, 13, 30, 0));

    assert(b - a == 3600);

    assert(CTimeTester::test(a + 60, 12, 31, 0));
    assert(CTimeTester::test(a - 60, 12, 29, 0));

    assert(a < b);
    assert(a <= b);
    assert(a != b);
    assert(!(a > b));
    assert(!(a >= b));
    assert(!(a == b));

    while (++a != b);
    assert(a == b);

    std::ostringstream output;
    assert(static_cast<bool>( output << a ));
    assert(output.str() == "13:30:00");

    assert(a++ == b++);
    assert(a == b);

    assert(--a == --b);
    assert(a == b);

    assert(a-- == b--);
    assert(a == b);

    return 0;
}

#endif /* __PROGTEST__ */
