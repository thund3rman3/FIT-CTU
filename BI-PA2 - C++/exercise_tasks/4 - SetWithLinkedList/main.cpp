#ifndef __PROGTEST__

#include <cstring>

using namespace std;

class CLinkedSetTester;

#endif /* __PROGTEST__ */


class CLinkedSet {
private:
    void deleteList() {
        CNode *node = m_Begin;
        CNode *tmp;
        while (node) {
            tmp = node->m_Next;
            delete [] node->Value();
            delete node;
            node = tmp;
        }
        cnt = 0;
    }

    struct CNode {
        CNode *m_Next;

        explicit CNode(const char *otherVal) {
            m_Next = nullptr;
            val = otherVal;
        }

        const char *Value() const {
            return val;
        }

    private:
        const char *val;
    };

    CNode *m_Begin;
    size_t cnt;

public:
    // default constructor
    CLinkedSet();

    // copy constructor
    CLinkedSet(const CLinkedSet &ls);

    // operator=
    CLinkedSet &operator=(const CLinkedSet &ls);

    // destructor
    ~CLinkedSet();

    //Vloží do množiny řetězec, vrací true pokud se vložení povede.
    //Pokud již v množině řetězec je, vrátí funkce false a množinu nemění.
    bool Insert(const char *value);

    //Odstraní z množiny řetězec, vrací true pokud se smazání povede.
    // Pokud v množině řetězec není, vrátí funkce false a množinu nemění.
    bool Remove(const char *value);

    bool Empty() const;

    size_t Size() const;

    bool Contains(const char *value) const;

    friend class ::CLinkedSetTester;
};

CLinkedSet::CLinkedSet() : m_Begin(nullptr), cnt(0) {
}

CLinkedSet::CLinkedSet(const CLinkedSet &ls) {
    cnt = 0;
    m_Begin = nullptr;
    for (CNode *node = ls.m_Begin; node; node = node->m_Next) {
        Insert(node->Value());
    }
}

CLinkedSet &CLinkedSet::operator=(const CLinkedSet &ls) {
    if (this == &ls)
        return *this;

    deleteList();
    m_Begin = nullptr;
    for (CNode *node = ls.m_Begin; node; node = node->m_Next) {
        Insert(node->Value());
    }
    return *this;
}

CLinkedSet::~CLinkedSet() {
    deleteList();
}

bool CLinkedSet::Insert(const char *value) {
    if (Contains(value))
        return false;

    if (!m_Begin || strcmp(value, m_Begin->Value()) < 0) {
        char* newValue = new char[strlen(value) + 1];
        strcpy(newValue, value);
        CNode *newNode = new CNode(newValue);
        newNode->m_Next = m_Begin;
        m_Begin = newNode;
        cnt++;
        return true;
    }
    CNode *curr = m_Begin;
    CNode *prev = nullptr;
    int cmp = 0;
    while (curr) {
        cmp = strcmp(value, curr->Value());
        if (!cmp)
            return false;
        else if (cmp < 0)
            break;
        prev = curr;
        curr = curr->m_Next;
    }
    cnt++;
    char* newValue = new char[strlen(value) + 1];
    strcpy(newValue, value);
    CNode *newNode = new CNode(newValue);
    prev->m_Next = newNode;
    newNode->m_Next = curr;

    return true;

}

bool CLinkedSet::Remove(const char *value) {
    CNode *curr = m_Begin;
    CNode *prev = nullptr;
    while (curr) {
        if (strcmp(value, curr->Value()) == 0) {
            if (!prev)
                m_Begin = curr->m_Next;
            else
                prev->m_Next = curr->m_Next;
            cnt--;
            delete [] curr->Value();
            delete curr;
            return true;
        }
        prev = curr;
        curr = curr->m_Next;
    }
    return false;
}

bool CLinkedSet::Empty() const {
    return cnt == 0;
}

size_t CLinkedSet::Size() const {
    return cnt;
}

bool CLinkedSet::Contains(const char *value) const {
    if (!m_Begin)
        return false;

    CNode *curr = m_Begin;
    while (curr) {
        if (strcmp(value, curr->Value()) == 0) {
            return true;
        }
        curr = curr->m_Next;
    }
    return false;
}

#ifndef __PROGTEST__

#include <cassert>

struct CLinkedSetTester {
    static void test0() {
        CLinkedSet x0;
        assert(x0.Insert("Jerabek Michal"));
        assert(x0.Insert("Pavlik Ales"));
        assert(x0.Insert("Dusek Zikmund"));
        assert(x0.Size() == 3);
        assert(x0.Contains("Dusek Zikmund"));
        assert(!x0.Contains("Pavlik Jan"));
        assert(x0.Remove("Jerabek Michal"));
        assert(!x0.Remove("Pavlik Jan"));
        assert(!x0.Empty());
    }

    static void test1() {
        CLinkedSet x0;
        assert(x0.Insert("Jerabek Michal"));
        assert(x0.Insert("Pavlik Ales"));
        assert(x0.Insert("Dusek Zikmund"));
        assert(x0.Size() == 3);
        assert(x0.Contains("Dusek Zikmund"));
        assert(!x0.Contains("Pavlik Jan"));
        assert(x0.Remove("Jerabek Michal"));
        assert(!x0.Remove("Pavlik Jan"));
        CLinkedSet x1(x0);
        assert(x0.Insert("Vodickova Saskie"));
        assert(x1.Insert("Kadlecova Kvetslava"));
        assert(x0.Size() == 3);
        assert(x1.Size() == 3);
        assert(x0.Contains("Vodickova Saskie"));
        assert(!x1.Contains("Vodickova Saskie"));
        assert(!x0.Contains("Kadlecova Kvetslava"));
        assert(x1.Contains("Kadlecova Kvetslava"));
    }

    static void test2() {
        CLinkedSet x0;
        CLinkedSet x1;
        assert(x0.Insert("Jerabek Michal"));
        assert(x0.Insert("Pavlik Ales"));
        assert(x0.Insert("Dusek Zikmund"));
        assert(x0.Size() == 3);
        assert(x0.Contains("Dusek Zikmund"));
        assert(!x0.Contains("Pavlik Jan"));
        assert(x0.Remove("Jerabek Michal"));
        assert(!x0.Remove("Pavlik Jan"));
        x1 = x0;
        assert(x0.Insert("Vodickova Saskie"));
        assert(x1.Insert("Kadlecova Kvetslava"));
        assert(x0.Size() == 3);
        assert(x1.Size() == 3);
        assert(x0.Contains("Vodickova Saskie"));
        assert(!x1.Contains("Vodickova Saskie"));
        assert(!x0.Contains("Kadlecova Kvetslava"));
        assert(x1.Contains("Kadlecova Kvetslava"));
    }

    static void test3() {
        CLinkedSet x0;
        CLinkedSet x1(x0);
        assert(x1.Insert("D"));
        assert(x1.Insert("B"));
        assert(x1.Insert("A"));
        assert(x1.Insert("M"));
        assert(x1.Insert("O"));
        assert(x1.Insert("P"));
        assert(x1.Insert("E"));
        x0 = x1;
        assert(!x0.Remove("a"));
        assert(!x0.Remove("Z"));
        assert(!x0.Remove("N"));
        assert(x0.Remove("E"));
        assert(x0.Remove("A"));
        assert(x0.Remove("P"));
        assert(x0.Remove("B"));
        assert(!x0.Remove("B"));
        assert(x0.Remove("M"));
        assert(x0.Remove("O"));
        assert(x0.Remove("D"));
        assert(x0.Size() == 0);
        assert(x0.Insert("Z"));
        assert(!x0.Insert("Z"));
        assert(!x0.Remove("M"));
        assert(x0.Insert("U"));
        assert(x0.Insert("Y"));
        assert(x0.Insert("W"));
        assert(x0.Insert("A"));
        assert(x0.Insert("D"));
        assert(x0.Insert("s"));
        assert(x0.Insert("e"));
        assert(x0.Insert("f"));
        assert(x0.Remove("U"));
        assert(x0.Remove("Y"));
        assert(x0.Remove("W"));
        assert(x0.Remove("A"));
        assert(x0.Remove("D"));
        assert(x0.Remove("s"));
        assert(x0.Remove("e"));
        assert(x0.Remove("f"));
        x1 = x0;
        x1 = x1;

    }

};

int main() {
    CLinkedSetTester::test0();
    CLinkedSetTester::test1();
    CLinkedSetTester::test2();
    //CLinkedSetTester::test3();
    return 0;
}

#endif /* __PROGTEST__ */
