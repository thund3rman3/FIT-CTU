#ifndef __PROGTEST__
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>

#include <algorithm>
#include <functional>

#include <stdexcept>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <cstdint>
#include <cctype>
#include <cassert>
using namespace std;
#endif /* __PROGTEST__ */

template <typename T_>
class CSet {
private:
    void deleteList() {
        CNode *node = m_Begin;
        CNode *tmp;
        while (node) {
            tmp = node->m_Next;
            delete node;
            node = tmp;
        }
        cnt = 0;
    }

    struct CNode {
        CNode *m_Next;
        T_ val;

        explicit CNode(T_  otherVal)
            : m_Next(nullptr), val(std::move(otherVal)) {}

        T_ Value() const {
            return val;
        }

    };

    CNode *m_Begin;
    size_t cnt;

public:
    // default constructor
    CSet();

    // copy constructor
    CSet(const CSet &ls);

    // operator=
    CSet &operator=(const CSet &ls);

    // destructor
    ~CSet();

    //Vloží do množiny řetězec, vrací true pokud se vložení povede.
    //Pokud již v množině řetězec je, vrátí funkce false a množinu nemění.
    bool Insert(const T_& value);

    //Odstraní z množiny řetězec, vrací true pokud se smazání povede.
    // Pokud v množině řetězec není, vrátí funkce false a množinu nemění.
    bool Remove(const T_& value);

    bool Empty() const;

    size_t Size() const;

    bool Contains(const T_& value) const;

};

template<typename T_>
CSet<T_>::CSet() : m_Begin(nullptr), cnt(0) {
}

template<typename T_>
CSet<T_>::CSet(const CSet<T_> &ls) {
    cnt = 0;
    m_Begin = nullptr;
    for (CNode *node = ls.m_Begin; node; node = node->m_Next) {
        Insert(node->Value());
    }
}
template<typename T_>
CSet<T_> & CSet<T_>::operator=(const CSet<T_> &ls) {
    if (this == &ls)
        return *this;

    deleteList();
    m_Begin = nullptr;
    for (CNode *node = ls.m_Begin; node; node = node->m_Next) {
        Insert(node->Value());
    }
    return *this;
}

template<typename T_>
CSet<T_>::~CSet() {
    deleteList();
}

template<typename T_>
bool CSet<T_>::Insert(const T_& value) {
    if (Contains(value))
        return false;
    std::less<T_> lessThan;
    //std::equal_to<T_> equalTo;
    if (!m_Begin || lessThan(value, m_Begin->Value())) {
        CNode *newNode = new CNode(value);
        newNode->m_Next = m_Begin;
        m_Begin = newNode;
        cnt++;
        return true;
    }
    CNode *curr = m_Begin;
    CNode *prev = nullptr;
    while (curr) {
        if (!lessThan(value, curr->Value()) && !lessThan(curr->Value(), value))
            return false;
        else if (lessThan(value, curr->Value()))
            break;
        prev = curr;
        curr = curr->m_Next;
    }
    cnt++;
    CNode *newNode = new CNode(value);
    prev->m_Next = newNode;
    newNode->m_Next = curr;

    return true;

}

template<typename T_>
bool CSet<T_>::Remove(const T_& value) {
    CNode *curr = m_Begin;
    CNode *prev = nullptr;
    //std::equal_to<T_> equalTo;
    std::less<T_> lessThan;
    while (curr) {
        if (!lessThan(value, curr->Value()) && !lessThan(curr->Value(), value)) {
            if (!prev)
                m_Begin = curr->m_Next;
            else
                prev->m_Next = curr->m_Next;
            cnt--;
            delete curr;
            return true;
        }
        prev = curr;
        curr = curr->m_Next;
    }
    return false;
}

template<typename T_>
bool CSet<T_>::Empty() const {
    return cnt == 0;
}

template<typename T_>
size_t CSet<T_>::Size() const {
    return cnt;
}

template<typename T_>
bool CSet<T_>::Contains(const T_& value) const {
    if (!m_Begin)
        return false;

    std::less<T_> lessThan;
    //std::equal_to<T_> equalTo;
    CNode *curr = m_Begin;
    while (curr) {
        if (!lessThan(value, curr->Value()) && !lessThan(curr->Value(), value)) {
        //if (equalTo(value, curr->Value())) {
            return true;
        }
        curr = curr->m_Next;
    }
    return false;
}

#ifndef __PROGTEST__
#include <cassert>
#include <utility>

class CTime {
private:
    int h;
    int min;
    int s;

public:
    CTime() : h(0), min(0), s(0) {}
    friend bool operator<(const CTime& a, const CTime& b){
        return a.h*3600+a.min*60+a.s < b.h*3600+b.min*60+b.s;
    }
};

struct CSetTester
{
    static void test0()
    {
        CSet<string> x0;
        assert( x0 . Insert( "Jerabek Michal" ) );
        assert( x0 . Insert( "Pavlik Ales" ) );
        assert( x0 . Insert( "Dusek Zikmund" ) );
        assert( x0 . Size() == 3 );
        assert( x0 . Contains( "Dusek Zikmund" ) );
        assert( !x0 . Contains( "Pavlik Jan" ) );
        assert( x0 . Remove( "Jerabek Michal" ) );
        assert( !x0 . Remove( "Pavlik Jan" ) );
    }

    static void test1()
    {
        CSet<string> x0;
        assert( x0 . Insert( "Jerabek Michal" ) );
        assert( x0 . Insert( "Pavlik Ales" ) );
        assert( x0 . Insert( "Dusek Zikmund" ) );
        assert( x0 . Size() == 3 );
        assert( x0 . Contains( "Dusek Zikmund" ) );
        assert( !x0 . Contains( "Pavlik Jan" ) );
        assert( x0 . Remove( "Jerabek Michal" ) );
        assert( !x0 . Remove( "Pavlik Jan" ) );
        CSet<string> x1 ( x0 );
        assert( x0 . Insert( "Vodickova Saskie" ) );
        assert( x1 . Insert( "Kadlecova Kvetslava" ) );
        assert( x0 . Size() == 3 );
        assert( x1 . Size() == 3 );
        assert( x0 . Contains( "Vodickova Saskie" ) );
        assert( !x1 . Contains( "Vodickova Saskie" ) );
        assert( !x0 . Contains( "Kadlecova Kvetslava" ) );
        assert( x1 . Contains( "Kadlecova Kvetslava" ) );
    }

    static void test2()
    {
        CSet<string> x0;
        CSet<string> x1;
        assert( x0 . Insert( "Jerabek Michal" ) );
        assert( x0 . Insert( "Pavlik Ales" ) );
        assert( x0 . Insert( "Dusek Zikmund" ) );
        assert( x0 . Size() == 3 );
        assert( x0 . Contains( "Dusek Zikmund" ) );
        assert( !x0 . Contains( "Pavlik Jan" ) );
        assert( x0 . Remove( "Jerabek Michal" ) );
        assert( !x0 . Remove( "Pavlik Jan" ) );
        x1 = x0;
        assert( x0 . Insert( "Vodickova Saskie" ) );
        assert( x1 . Insert( "Kadlecova Kvetslava" ) );
        assert( x0 . Size() == 3 );
        assert( x1 . Size() == 3 );
        assert( x0 . Contains( "Vodickova Saskie" ) );
        assert( !x1 . Contains( "Vodickova Saskie" ) );
        assert( !x0 . Contains( "Kadlecova Kvetslava" ) );
        assert( x1 . Contains( "Kadlecova Kvetslava" ) );
    }

    static void test4()
    {
        CSet<int> x0;
        assert( x0 . Insert( 4 ) );
        assert( x0 . Insert( 8 ) );
        assert( x0 . Insert( 1 ) );
        assert( x0 . Size() == 3 );
        assert( x0 . Contains( 4 ) );
        assert( !x0 . Contains( 5 ) );
        assert( !x0 . Remove( 5 ) );
        assert( x0 . Remove( 4 ) );
        CSet<CTime> xx;
        assert(xx.Insert(CTime()));
    }

};

int main ()
{
    CSetTester::test0();
    CSetTester::test1();
    CSetTester::test2();
    CSetTester::test4();
    return 0;
}
#endif /* __PROGTEST__ */
