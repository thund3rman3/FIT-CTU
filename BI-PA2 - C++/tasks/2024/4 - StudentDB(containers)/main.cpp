#ifndef __PROGTEST__

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cctype>
#include <climits>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <string>
#include <array>
#include <utility>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include <algorithm>
#include <functional>
#include <iterator>
#include <compare>

class CDate {
public:
    CDate(int y, int m, int d) : m_Y(y), m_M(m), m_D(d) {
    }

    std::strong_ordering operator<=>(const CDate &other) const = default;

    friend std::ostream &operator<<(std::ostream &os, const CDate &d) {
        return os << d.m_Y << '-' << d.m_M << '-' << d.m_D;
    }

private:
    int m_Y;
    int m_M;
    int m_D;
};

enum class ESortKey {
    NAME, BIRTH_DATE, ENROLL_YEAR
};
#endif /* __PROGTEST__ */

class CStudent {
public:
    CStudent(const std::string &name, const CDate &born, int enrolled);

    bool operator==(const CStudent &other) const;

    bool operator!=(const CStudent &other) const;

    bool operator<(const CStudent &other) const;

    std::vector<std::string> namesApart;
    std::string name;
    std::string specName;
    CDate born;
    size_t pos;
    int enrolled;
};

CStudent::CStudent(const std::string &name, const CDate &born, int enrolled) : born(born), enrolled(enrolled) {
    std::istringstream istr(name);
    std::string input;
    while ((istr >> input)) {
        this->name += input + " ";
        std::transform(input.begin(), input.end(), input.begin(), ::tolower);
        namesApart.emplace_back(input);
    }
    std::sort(namesApart.begin(), namesApart.end());
    for (const auto &x: namesApart)
        specName += x + " ";
    specName.erase(specName.length() - 1);
    this->name.erase(this->name.length() - 1);
}

bool CStudent::operator==(const CStudent &other) const {
    return name == other.name && born == other.born && enrolled == other.enrolled;
}

bool CStudent::operator!=(const CStudent &other) const {
    return !(*this == other);
}

bool CStudent::operator<(const CStudent &other) const {
    if (enrolled != other.enrolled)
        return enrolled < other.enrolled;
    else if (born != other.born)
        return born < other.born;
    return name < other.name;
}

class CFilter {
public:
    CFilter() = default;

    CFilter &name(const std::string &name);

    CFilter &bornBefore(const CDate &date);

    CFilter &bornAfter(const CDate &date);

    CFilter &enrolledBefore(int year);

    CFilter &enrolledAfter(int year);

    std::vector<std::string> specNames;
    CDate bornBeforeFlt = CDate(0, 0, 0);
    CDate bornAfterFlt = CDate(0, 0, 0);
    int signedBeforeFlt = 0;
    int signedAfterFlt = 0;

};

CFilter &CFilter::name(const std::string &name) {
    std::istringstream istr(name);
    std::string input, fin;
    std::vector<std::string> namesApartFilter;

    while ((istr >> input)) {
        std::transform(input.begin(), input.end(), input.begin(), ::tolower);
        namesApartFilter.emplace_back(input);
    }
    std::sort(namesApartFilter.begin(), namesApartFilter.end());
    for (const auto &x: namesApartFilter)
        fin += x + " ";
    fin.erase(fin.length() - 1);
    specNames.emplace_back(fin);
    return *this;
}

CFilter &CFilter::bornBefore(const CDate &date) {
    bornBeforeFlt = date;
    return *this;
}

CFilter &CFilter::bornAfter(const CDate &date) {
    bornAfterFlt = date;
    return *this;
}

CFilter &CFilter::enrolledBefore(int year) {
    signedBeforeFlt = year;
    return *this;
}

CFilter &CFilter::enrolledAfter(int year) {
    signedAfterFlt = year;
    return *this;
}

class CSort {
public:
    CSort() = default;

    CSort &addKey(ESortKey key, bool ascending);

    std::vector<std::pair<ESortKey, bool>> sortBy;

    bool operator()(const CStudent &a, const CStudent &b) const;
};

CSort &CSort::addKey(ESortKey key, bool ascending) {
    sortBy.emplace_back(key, ascending);
    return *this;
}

bool CSort::operator()(const CStudent &a, const CStudent &b) const {
    for (const auto &s: sortBy) {
        switch (s.first) {
            case ESortKey::ENROLL_YEAR: {
                if (a.enrolled == b.enrolled)
                    break;
                else if (s.second)
                    return a.enrolled < b.enrolled;
                else
                    return a.enrolled > b.enrolled;
            }
            case ESortKey::BIRTH_DATE: {
                if (a.born == b.born)
                    break;
                else if (s.second)
                    return a.born < b.born;
                else
                    return a.born > b.born;
                break;
            }
            case ESortKey::NAME: {
                if (a.name == b.name)
                    break;
                else if (s.second)
                    return a.name < b.name;
                else
                    return a.name > b.name;
            }
        }
    }
    return a.pos < b.pos;
}

class CStudyDept {
public:
    CStudyDept() = default;

    bool addStudent(const CStudent &x);

    bool delStudent(const CStudent &x);

    std::list<CStudent> search(const CFilter &flt, const CSort &sortOpt) const;

    std::set<std::string> suggest(const std::string &name) const;

private:
    std::set<CStudent> studDB;
    size_t pos = 0;
};

bool CStudyDept::addStudent(const CStudent &x) {
    CStudent y(x);
    y.pos = pos++;
    auto v = studDB.emplace(y);
    return v.second;
}

bool CStudyDept::delStudent(const CStudent &x) {
    return studDB.erase(x);
}

std::list<CStudent> CStudyDept::search(const CFilter &flt, const CSort &sortOpt) const {
    std::list<CStudent> listusReturnus;

    for (const auto &student: studDB) {
        if ((flt.signedAfterFlt != 0 && flt.signedAfterFlt >= student.enrolled) ||
            (flt.signedBeforeFlt != 0 && flt.signedBeforeFlt <= student.enrolled) ||
            (flt.bornAfterFlt != CDate(0, 0, 0) && flt.bornAfterFlt >= student.born) ||
            (flt.bornBeforeFlt != CDate(0, 0, 0) && flt.bornBeforeFlt <= student.born))
            continue;

        if (flt.specNames.empty()) {
            listusReturnus.emplace_back(student);
            continue;
        }
        for (const auto &name1: flt.specNames) {
            if (name1 == student.specName) {
                listusReturnus.emplace_back(student);
                break;
            }
        }
    }
    if (sortOpt.sortBy.empty()) {
        listusReturnus.sort([](const CStudent &a, const CStudent &b) {
            return a.pos < b.pos;
        });
        return listusReturnus;
    }
    listusReturnus.sort(sortOpt);
    return listusReturnus;
}

std::set<std::string> CStudyDept::suggest(const std::string &name) const {
    std::set<std::string> setusReturnus;

    std::istringstream istr(name);
    std::string input, fin;
    std::vector<std::string> namesApart;

    while ((istr >> input)) {
        std::transform(input.begin(), input.end(), input.begin(), ::tolower);
        namesApart.emplace_back(input);
    }

    for (const auto& x: studDB) {
        size_t cnt = 0;
        for (size_t j = 0; j < namesApart.size(); ++j) {
            for (size_t i = 0; i < x.namesApart.size(); ++i) {
                if(namesApart[j] == x.namesApart[i]){
                    cnt++;
                    break;
                }
            }
            if(j == namesApart.size() - 1 && cnt == namesApart.size())
                setusReturnus.emplace(x.name);
        }
    }

    return setusReturnus;
}

#ifndef __PROGTEST__

int main(void) {
    CStudyDept x0;
    assert (CStudent("James Bond", CDate(1980, 4, 11), 2010) == CStudent("James Bond", CDate(1980, 4, 11), 2010));
    assert (!(CStudent("James Bond", CDate(1980, 4, 11), 2010) != CStudent("James Bond", CDate(1980, 4, 11), 2010)));
    assert (CStudent("James Bond", CDate(1980, 4, 11), 2010) != CStudent("Peter Peterson", CDate(1980, 4, 11), 2010));
    assert (!(CStudent("James Bond", CDate(1980, 4, 11), 2010) ==
              CStudent("Peter Peterson", CDate(1980, 4, 11), 2010)));
    assert (CStudent("James Bond", CDate(1980, 4, 11), 2010) != CStudent("James Bond", CDate(1997, 6, 17), 2010));
    assert (!(CStudent("James Bond", CDate(1980, 4, 11), 2010) == CStudent("James Bond", CDate(1997, 6, 17), 2010)));
    assert (CStudent("James Bond", CDate(1980, 4, 11), 2010) != CStudent("James Bond", CDate(1980, 4, 11), 2016));
    assert (!(CStudent("James Bond", CDate(1980, 4, 11), 2010) == CStudent("James Bond", CDate(1980, 4, 11), 2016)));
    assert (CStudent("James Bond", CDate(1980, 4, 11), 2010) != CStudent("Peter Peterson", CDate(1980, 4, 11), 2016));
    assert (!(CStudent("James Bond", CDate(1980, 4, 11), 2010) ==
              CStudent("Peter Peterson", CDate(1980, 4, 11), 2016)));
    assert (CStudent("James Bond", CDate(1980, 4, 11), 2010) != CStudent("Peter Peterson", CDate(1997, 6, 17), 2010));
    assert (!(CStudent("James Bond", CDate(1980, 4, 11), 2010) ==
              CStudent("Peter Peterson", CDate(1997, 6, 17), 2010)));
    assert (CStudent("James Bond", CDate(1980, 4, 11), 2010) != CStudent("James Bond", CDate(1997, 6, 17), 2016));
    assert (!(CStudent("James Bond", CDate(1980, 4, 11), 2010) == CStudent("James Bond", CDate(1997, 6, 17), 2016)));
    assert (CStudent("James Bond", CDate(1980, 4, 11), 2010) != CStudent("Peter Peterson", CDate(1997, 6, 17), 2016));
    assert (!(CStudent("James Bond", CDate(1980, 4, 11), 2010) ==
              CStudent("Peter Peterson", CDate(1997, 6, 17), 2016)));
    assert (x0.addStudent(CStudent("John Peter Taylor", CDate(1983, 7, 13), 2014)));
    assert (x0.addStudent(CStudent("John Taylor", CDate(1981, 6, 30), 2012)));
    assert (x0.addStudent(CStudent("Peter Taylor", CDate(1982, 2, 23), 2011)));
    assert (x0.addStudent(CStudent("Peter John Taylor", CDate(1984, 1, 17), 2017)));
    assert (x0.addStudent(CStudent("James Bond", CDate(1981, 7, 16), 2013)));
    assert (x0.addStudent(CStudent("James Bond", CDate(1982, 7, 16), 2013)));
    assert (x0.addStudent(CStudent("James Bond", CDate(1981, 8, 16), 2013)));
    assert (x0.addStudent(CStudent("James Bond", CDate(1981, 7, 17), 2013)));
    assert (x0.addStudent(CStudent("James Bond", CDate(1981, 7, 16), 2012)));
    assert (x0.addStudent(CStudent("Bond James", CDate(1981, 7, 16), 2013)));
    assert (x0.search(CFilter(), CSort()) ==
            (std::list<CStudent>{CStudent("John Peter Taylor", CDate(1983, 7, 13), 2014),
                                 CStudent("John Taylor", CDate(1981, 6, 30), 2012),
                                 CStudent("Peter Taylor", CDate(1982, 2, 23), 2011),
                                 CStudent("Peter John Taylor", CDate(1984, 1, 17), 2017),
                                 CStudent("James Bond", CDate(1981, 7, 16), 2013),
                                 CStudent("James Bond", CDate(1982, 7, 16), 2013),
                                 CStudent("James Bond", CDate(1981, 8, 16), 2013),
                                 CStudent("James Bond", CDate(1981, 7, 17), 2013),
                                 CStudent("James Bond", CDate(1981, 7, 16), 2012),
                                 CStudent("Bond James", CDate(1981, 7, 16), 2013)}));
    assert (x0.search(CFilter(), CSort().addKey(ESortKey::NAME, true)) ==
            (std::list<CStudent>{CStudent("Bond James", CDate(1981, 7, 16), 2013),
                                 CStudent("James Bond", CDate(1981, 7, 16), 2013),
                                 CStudent("James Bond", CDate(1982, 7, 16), 2013),
                                 CStudent("James Bond", CDate(1981, 8, 16), 2013),
                                 CStudent("James Bond", CDate(1981, 7, 17), 2013),
                                 CStudent("James Bond", CDate(1981, 7, 16), 2012),
                                 CStudent("John Peter Taylor", CDate(1983, 7, 13), 2014),
                                 CStudent("John Taylor", CDate(1981, 6, 30), 2012),
                                 CStudent("Peter John Taylor", CDate(1984, 1, 17), 2017),
                                 CStudent("Peter Taylor", CDate(1982, 2, 23), 2011)}));
    assert (x0.search(CFilter(), CSort().addKey(ESortKey::NAME, false)) ==
            (std::list<CStudent>{CStudent("Peter Taylor", CDate(1982, 2, 23), 2011),
                                 CStudent("Peter John Taylor", CDate(1984, 1, 17), 2017),
                                 CStudent("John Taylor", CDate(1981, 6, 30), 2012),
                                 CStudent("John Peter Taylor", CDate(1983, 7, 13), 2014),
                                 CStudent("James Bond", CDate(1981, 7, 16), 2013),
                                 CStudent("James Bond", CDate(1982, 7, 16), 2013),
                                 CStudent("James Bond", CDate(1981, 8, 16), 2013),
                                 CStudent("James Bond", CDate(1981, 7, 17), 2013),
                                 CStudent("James Bond", CDate(1981, 7, 16), 2012),
                                 CStudent("Bond James", CDate(1981, 7, 16), 2013)}));
    assert (x0.search(CFilter(),
                      CSort().addKey(ESortKey::ENROLL_YEAR, false).addKey(ESortKey::BIRTH_DATE, false).addKey(
                              ESortKey::NAME, true)) ==
            (std::list<CStudent>{CStudent("Peter John Taylor", CDate(1984, 1, 17), 2017),
                                 CStudent("John Peter Taylor", CDate(1983, 7, 13), 2014),
                                 CStudent("James Bond", CDate(1982, 7, 16), 2013),
                                 CStudent("James Bond", CDate(1981, 8, 16), 2013),
                                 CStudent("James Bond", CDate(1981, 7, 17), 2013),
                                 CStudent("Bond James", CDate(1981, 7, 16), 2013),
                                 CStudent("James Bond", CDate(1981, 7, 16), 2013),
                                 CStudent("James Bond", CDate(1981, 7, 16), 2012),
                                 CStudent("John Taylor", CDate(1981, 6, 30), 2012),
                                 CStudent("Peter Taylor", CDate(1982, 2, 23), 2011)}));
    assert (x0.search(CFilter().name("james bond"),
                      CSort().addKey(ESortKey::ENROLL_YEAR, false).addKey(ESortKey::BIRTH_DATE, false).addKey(
                              ESortKey::NAME, true)) ==
            (std::list<CStudent>{CStudent("James Bond", CDate(1982, 7, 16), 2013),
                                 CStudent("James Bond", CDate(1981, 8, 16), 2013),
                                 CStudent("James Bond", CDate(1981, 7, 17), 2013),
                                 CStudent("Bond James", CDate(1981, 7, 16), 2013),
                                 CStudent("James Bond", CDate(1981, 7, 16), 2013),
                                 CStudent("James Bond", CDate(1981, 7, 16), 2012)}));
    assert (x0.search(CFilter().bornAfter(CDate(1980, 4, 11)).bornBefore(CDate(1983, 7, 13)).name("John Taylor").name(
            "james BOND"), CSort().addKey(ESortKey::ENROLL_YEAR, false).addKey(ESortKey::BIRTH_DATE, false).addKey(
            ESortKey::NAME, true)) == (std::list<CStudent>{CStudent("James Bond", CDate(1982, 7, 16), 2013),
                                                           CStudent("James Bond", CDate(1981, 8, 16), 2013),
                                                           CStudent("James Bond", CDate(1981, 7, 17), 2013),
                                                           CStudent("Bond James", CDate(1981, 7, 16), 2013),
                                                           CStudent("James Bond", CDate(1981, 7, 16), 2013),
                                                           CStudent("James Bond", CDate(1981, 7, 16), 2012),
                                                           CStudent("John Taylor", CDate(1981, 6, 30), 2012)}));
    assert (x0.search(CFilter().name("james"), CSort().addKey(ESortKey::NAME, true)) == (std::list<CStudent>{}));
    assert (x0.suggest("peter") == (std::set<std::string>{"John Peter Taylor", "Peter John Taylor", "Peter Taylor"}));
    assert (x0.suggest("bond") == (std::set<std::string>{"Bond James", "James Bond"}));
    assert (x0.suggest("peter joHn") == (std::set<std::string>{"John Peter Taylor", "Peter John Taylor"}));
    assert (x0.suggest("peter joHn bond") == (std::set<std::string>{}));
    assert (x0.suggest("pete") == (std::set<std::string>{}));
    assert (x0.suggest("peter joHn PETER") == (std::set<std::string>{"John Peter Taylor", "Peter John Taylor"}));
    assert (!x0.addStudent(CStudent("James Bond", CDate(1981, 7, 16), 2013)));
    assert (x0.delStudent(CStudent("James Bond", CDate(1981, 7, 16), 2013)));
    assert (x0.search(CFilter().bornAfter(CDate(1980, 4, 11)).bornBefore(CDate(1983, 7, 13)).name("John Taylor").name(
            "james BOND"), CSort().addKey(ESortKey::ENROLL_YEAR, false).addKey(ESortKey::BIRTH_DATE, false).addKey(
            ESortKey::NAME, true)) == (std::list<CStudent>{CStudent("James Bond", CDate(1982, 7, 16), 2013),
                                                           CStudent("James Bond", CDate(1981, 8, 16), 2013),
                                                           CStudent("James Bond", CDate(1981, 7, 17), 2013),
                                                           CStudent("Bond James", CDate(1981, 7, 16), 2013),
                                                           CStudent("James Bond", CDate(1981, 7, 16), 2012),
                                                           CStudent("John Taylor", CDate(1981, 6, 30), 2012)}));
    assert (!x0.delStudent(CStudent("James Bond", CDate(1981, 7, 16), 2013)));
    return EXIT_SUCCESS;
}

#endif /* __PROGTEST__ */
