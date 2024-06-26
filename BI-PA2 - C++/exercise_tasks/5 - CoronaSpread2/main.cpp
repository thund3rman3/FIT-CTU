#ifndef __PROGTEST__

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <stdexcept>
#include <numeric>
#include <vector>
#include <array>
#include <set>
#include <map>
#include <deque>
#include <queue>
#include <stack>
#include <cassert>

using namespace std;
#endif /* __PROGTEST__ */

class CTimeStamp {
private:
    int year;
    int month;
    int day;
    int hour;
    int min;
    int sec;
public:
    CTimeStamp(int y, int mon, int d, int h, int min, int s) : year(y), month(mon), day(d), hour(h), min(min), sec(s) {
    }

    friend bool within(const CTimeStamp &timeStampThis, const CTimeStamp &timeStamp1, const CTimeStamp &timeStamp2) {
        size_t dhmsThis = timeStampThis.day * 24 * 60 * 60 + timeStampThis.hour * 60 * 60 + timeStampThis.min * 60 +
                          timeStampThis.sec;
        size_t dhms1 = timeStamp1.day * 24 * 60 * 60 + timeStamp1.hour * 60 * 60 + timeStamp1.min * 60 + timeStamp1.sec;
        size_t dhms2 = timeStamp2.day * 24 * 60 * 60 + timeStamp2.hour * 60 * 60 + timeStamp2.min * 60 + timeStamp2.sec;

        if (timeStampThis.year < timeStamp1.year || timeStampThis.year > timeStamp2.year ||
            (timeStampThis.year == timeStamp1.year && timeStampThis.month < timeStamp1.month) ||
            (timeStampThis.year == timeStamp2.year && timeStampThis.month > timeStamp2.month) ||
            (timeStampThis.year == timeStamp1.year && timeStampThis.month == timeStamp1.month && dhmsThis < dhms1) ||
            (timeStampThis.year == timeStamp2.year && timeStampThis.month == timeStamp2.month && dhmsThis > dhms2))
            return false;
        return true;
    }

    bool operator<=(const CTimeStamp &ts) const {
        if (year < ts.year)
            return true;
        else if (year > ts.year)
            return false;
        else if (month < ts.month)
            return true;
        else if (month > ts.month)
            return false;
        else if (day < ts.day)
            return true;
        else if (day > ts.day)
            return false;
        else if (hour < ts.hour)
            return true;
        else if (hour > ts.hour)
            return false;
        else if (min < ts.min)
            return true;
        else if (min > ts.min)
            return false;
        else if (sec < ts.sec)
            return true;
        return sec == ts.sec;
    }
};

class CContact {
private:
    CTimeStamp stamp;
    int one;
    int two;
public:
    CContact(const CTimeStamp &timeStamp, int one, int two) : stamp(timeStamp), one(one), two(two) {
    }

    int getFirstNum() const {
        return one;
    }

    int getSecNum() const {
        return two;
    }

    int check(int infected, const std::vector<int> &vec) const {
        if (one == two) // dont add
            return 0;
        else if (infected == one) // one
        {
            if (vec.end() == std::find(vec.begin(), vec.end(), two))
                return 1;
        } else if (infected == two) {
            if (vec.end() == std::find(vec.begin(), vec.end(), one))
                return 2;
        }
        return 0;
    }

    bool withinTimeStamp(const CTimeStamp &timeStamp1, const CTimeStamp &timeStamp2) const {
        if (within(stamp, timeStamp1, timeStamp2))
            return true;
        return false;
    }

    friend bool operator<=(const CTimeStamp &timeStamp1, const CTimeStamp &timeStamp2) {
        return timeStamp1 <= timeStamp2;
    }

    CTimeStamp getStamp() const {
        return stamp;
    }

};

class CEFaceMask {
private:
    std::vector<CContact> contacts;
public:
    CEFaceMask &addContact(const CContact &contact) {
        if (contact.getFirstNum() == contact.getSecNum())
            return *this;

        contacts.emplace_back(contact);
        return *this;
    }

    //dostane jako parametr časový interval a vrací,
    // který ze sledovaných jedinců mohl nakazit v
    // tomto intervalu nejvíce dalších lidí (počítá
    // se pouze přímý kontakt). Takových jedinců může
    // být i více, proto vraťte seznam jejich telefonních čísel.
    // Pokud k žádnému kontaktu v intervalu nedošlo, vracejte prázdný seznam.
    //Návratová hodnota metody getSuperSpreaders reprezentuje seznam telefonních čísel.
    // Seznam musí obsahovat pouze unikátní čísla a musí být seřazen vzestupně.
    //Konečně, z principu plnění databáze se občas stane, že se nedopatřením vygeneruje kontakt sám se sebou.
    // Takový kontakt ignorujte.

    std::vector<int> getSuperSpreaders(const CTimeStamp &timeStamp1, const CTimeStamp &timeStamp2) const {
        std::vector<int> res;
        size_t MAX = 0;
        std::set<std::pair<int, int>> contactsFiltered;
        for (const auto &c: contacts) {
            if (!(c.getStamp() <= timeStamp2) || !(timeStamp1 <= c.getStamp()))
                continue;
            contactsFiltered.emplace(c.getFirstNum(), c.getSecNum());
            contactsFiltered.emplace(c.getSecNum(), c.getFirstNum());
        }
        std::map<int, size_t> m;
        for (const auto &c: contactsFiltered) {
            m[c.first]++;
            if(m[c.first] > MAX)
                MAX = m[c.first];
        }
        for(const auto& [key, val]: m){
            if(val == MAX)
                res.emplace_back(key);
        }
        if (!res.empty())
            std::sort(res.begin(), res.end());
        return res;
    }
};


#ifndef __PROGTEST__

int main() {
    CEFaceMask test;

    test.addContact(CContact(CTimeStamp(2021, 1, 10, 12, 40, 10), 111111111, 222222222));
    test.addContact(CContact(CTimeStamp(2021, 1, 12, 12, 40, 10), 333333333, 222222222)).addContact(
            CContact(CTimeStamp(2021, 2, 14, 15, 30, 28), 222222222, 444444444));
    test.addContact(CContact(CTimeStamp(2021, 2, 15, 18, 0, 0), 555555555, 444444444));
    assert (test.getSuperSpreaders(CTimeStamp(2021, 1, 1, 0, 0, 0), CTimeStamp(2022, 1, 1, 0, 0, 0)) ==
            (vector<int>{222222222}));
    test.addContact(CContact(CTimeStamp(2021, 3, 20, 18, 0, 0), 444444444, 666666666));
    test.addContact(CContact(CTimeStamp(2021, 3, 25, 0, 0, 0), 111111111, 666666666));
    assert (test.getSuperSpreaders(CTimeStamp(2021, 1, 1, 0, 0, 0), CTimeStamp(2022, 1, 1, 0, 0, 0)) ==
            (vector<int>{222222222, 444444444}));

    return 0;
}

#endif /* __PROGTEST__ */
